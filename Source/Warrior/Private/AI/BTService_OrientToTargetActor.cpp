// Spirit Revenge All Rights Reserved


#include "AI/BTService_OrientToTargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{
	//NodeName → 在 BT 编辑器里显示的名字。
	NodeName = TEXT("Native Orient Rotation To Target Actor");

	//INIT_SERVICE_NODE_NOTIFY_FLAGS(); → 保证 Tick 执行。
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	//插值速度，控制旋转朝向的平滑度
	RotationInterpSpeed = 5.f;
	//Interval = 0.f; → 表示每帧执行，对朝向这种需求是合理的。
	Interval = 0.f;
	//向服务的间隔添加随机范围
	RandomDeviation = 0.f;

	//InTargetActorKey.AddObjectFilter → 限制只能选 AActor 类型 Key，避免误选
	//GET_MEMBER_NAME_CHECKED 返回 FName(TEXT("MemberName"))，同时静态验证 ClassName 中是否存在该成员
	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass,InTargetActorKey), AActor::StaticClass());
}

void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	//调用父类函数
	Super::InitializeFromAsset(Asset);

	//判断黑板的资产是否存在
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		//存在则解析所选的key
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

//获取静态描述
FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	//获取InTargetActorKey的关键词描述
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();

	//编辑器描述友好，能显示 "Orient rotation to TargetActor key ...".
	return FString::Printf(TEXT("Orient rotation to %s key  %s"), *KeyDescription, *GetStaticServiceDescription());
}

void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//调用父类函数
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);\

	//从所有者组件获取blackboard组件，从组件通过name得到Target对象
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	//获取目标的Object并转化为AActor
	AActor* TargetActor = Cast<AActor>(ActorObject);

	//从拥有者组件获取AIController获取其Pawn
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();

	//如果拥有者及其对象都存在
	if (OwningPawn && TargetActor)
	{
		//OwnerComp
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(), TargetActor->GetActorLocation());
		//RInterpTo → 平滑旋转，避免瞬间转身
		const FRotator TargetRot = FMath::RInterpTo(OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);

		//SetActorRotation → 直接设置朝向
		OwningPawn->SetActorRotation(TargetRot);
	}
}
