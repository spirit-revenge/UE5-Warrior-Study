// Spirit Revenge All Rights Reserved


#include "AI/BTTask_RotateToFaceTarget.h"

#include "AIController.h"
#include "GameplayCueNotifyTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	//NodeName：编辑器里显示的名称
	NodeName =TEXT("Native Rotate to Face Target Actor");
	//AnglePrecision：允许的角度误差（单位是度）
	AnglePrecision = 10.f;
	//插值速度，越大朝向越快
	RotationInterpSpeed = 5.f;

	//bNotifyTick = true：表示此节点会收到 TickTask 回调——这是做平滑旋转所必需的
	bNotifyTick = true;
	//允许节点在完成或失败时被通知（通常用于可视化/调试）
	bNotifyTaskFinished = true;
	//表示不为每个节点创建 UObject 实例；我用 NodeMemory 存状态
	bCreateNodeInstance = false;

	//UE 的宏，确保通知标志正常初始化
	INIT_TASK_NODE_NOTIFY_FLAGS();

	//限制黑板 Key 必须是 AActor 类型，便于在编辑器里强类型选择
	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
}

//**把黑板键（FBlackboardKeySelector）解析成内部索引/名称（SelectedKeyName），
//确保后续通过 GetValueAsObject 能拿到正确值。必须在任务初始化阶段做这步
void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	//调用父类函数
	Super::InitializeFromAsset(Asset);

	//判断黑板的资产是否存在
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		//解析资产
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	//Behavior Tree 为每个运行中的节点分配一块「节点实例内存（Node Memory）」。
	//GetInstanceMemorySize() 告诉引擎要为该节点分配多大的内存。
	//你在内存里存 OwningPawn、TargetActor 的缓存（通常用 TWeakObjectPtr），避免每帧重复查找黑板/控制器。
	//这是比 bCreateNodeInstance = true 更轻量、更高效的方式（node instance vs node memory 的差别）。
	return sizeof(FRotateToFaceTargetTaskMemory);
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	//编辑器里节点的文本描述，方便调试和理解用途。没被设置 Key 时最好显示 "None" 或者类似提示
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Smoothly rotates to face %s key until the angle precision %s is reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//从黑板读取目标对象并 Cast 成 AActor*
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);

	//获取 AI 的 Pawn（OwnerComp.GetAIOwner()->GetPawn()）
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	//将 Pawn 与目标缓存到 NodeMemory（FRotateToFaceTargetTaskMemory）里，减少后续重复查询
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	//检查Memory是否创建成功
	check(Memory);

	//存入owning和target
	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;

	//调用 Memory->IsValid()：检查是否两个对象均有效
	if (!Memory->IsValid())
	{
		//返回失败结果
		return EBTNodeResult::Failed;
	}

	//如果已经满足角度精度（瞬时朝向到位），直接返回 Succeeded，
	if (HasReachedAnglePercision(OwningPawn, TargetActor))
	{
		//重置Memory
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}

	//否则返回 InProgress，让引擎调用 TickTask 每帧继续执行
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//首先从 NodeMemory 取回缓存的 Memory（里面保存 TWeakObjectPtr）
	FRotateToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);

	//Memory->IsValid() 用来判断 Pawn/Target 是否仍然有效（没被销毁）。
	//若无效，FinishLatentTask(..., Failed) 终止任务
	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	//当角度达到精度时，调用 FinishLatentTask(..., Succeeded) 并重置 Memory
	if (HasReachedAnglePercision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		//每帧计算理想朝向 LookAtRot（基于 FindLookAtRotation），然后用 FMath::RInterpTo 进行平滑插值生成新的旋转 TargetRot，最后 SetActorRotation 应用
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn->GetActorLocation(), Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot = FMath::RInterpTo(Memory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);
		Memory->OwningPawn->SetActorRotation(TargetRot);
	}
}

//Dot(a,b) = cos(θ)（当两个向量都单位化时）
//DegAcos(dot) = 返回 θ（角度制）。
//当 θ ≤ AnglePrecision（度）时，返回 true。
bool UBTTask_RotateToFaceTarget::HasReachedAnglePercision(APawn* QueryPawn, AActor* TargetActor) const
{
	// 获取 Pawn 面朝的单位方向向量
	const FVector OwnerForward = QueryPawn->GetActorForwardVector();
	// 计算目标位置相对 Pawn 的方向，并归一化为单位向量
	const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();

	// 计算两向量点积：cos(θ) = Forward · Direction
	const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
	// 求出夹角 θ（弧度转角度）
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);

	// 如果夹角小于等于阈值（AnglePrecision），认为已对准目标
	return AngleDiff <= AnglePrecision;
}
