// Spirit Revenge All Rights Reserved


#include "WarriorFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "WarriorTypes/WarriorCountDownAction.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	//保证传入的 Actor 不为空。
	check(InActor);

	//CastChecked 会在类型转换失败时报错，非常适合“必须存在”的情况
	//直接用 UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent，兼容任何实现了 IAbilitySystemInterface 的 Actor。
	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	//获取Actor伤的AbilitySystemComponent
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	//检查 Actor 是否已有该 Tag
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		//如果没有，就通过 ASC 添加一个 LooseGameplayTag
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	//判断是否有该Tag
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		//如果有，移除这个 Tag
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	//判断是否有该Tag
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	//利用 ExpandEnumAsExecs 直接给蓝图提供 Yes/No 执行流
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck)?EWarriorConfirmType::Yes:EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	//检查是否空指针
	check(InActor);

	//判断这个 Actor 有没有实现 IPawnCombatInterface 接口。
	//如果实现了，就通过接口调用 GetPawnCombatComponent() 返回组件
	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	//蓝图版本封装了C++版的函数的调用，并且根据返回值设置 OutValidType。
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;

	//返回值仍然是 CombatComponent，这样蓝图里可以直接拿到引用。
	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	//保证传入的两个 Pawn 都有效
	check(QueryPawn && TargetPawn);

	//获取两个 Pawn 的 Controller，并尝试转换成 IGenericTeamAgentInterface
	//IGenericTeamAgentInterface 是 UE5 用于团队/阵营系统的接口。
	//通过这个接口，可以查询 Controller 所属的团队 ID (GetGenericTeamId())，从而判断敌我。
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	//检查两个 Controller 是否成功获取了团队接口
	if (QueryTeamAgent && TargetTeamAgent)
	{
		//比较团队 ID，判断是否为敌对关系
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	//FScalableFloat 是 GAS 的可扩展浮点类型，支持按等级动态计算数值（如伤害、持续时间）。
	//InLevel 默认 1，可以传任意等级
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutSingleDifference)
{
	//检查攻击者和受害者是否都存在
	check(InAttacker && InVictim);

	//VictimForward → 受击者正前方向
	const FVector VictimForward = InVictim->GetActorForwardVector();
	//VictimToAttackerNormalized → 从受击者指向攻击者的方向向量
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	//Dot + Cross → 计算夹角及左右方向
	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	//OutSingleDifference → 返回夹角，可用于 debug 或动画 blending
	OutSingleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(VictimForward,VictimToAttackerNormalized);

	if (CrossResult.Z < 0.f)
	{
		OutSingleDifference *= -1.f;
	}

	/*
	 * 根据角度判断方向：
	 * [-45°, 45°] → 前
	 * [-135°, -45°] → 左
	 * [-180°, -135°] 或 [135°, 180°] → 后
	 * [45°, 135°] → 右
	 */
	if (OutSingleDifference >= -45.f && OutSingleDifference <= 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutSingleDifference < -45.f && OutSingleDifference >= -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutSingleDifference < -135.f || OutSingleDifference > 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutSingleDifference > 45.f && OutSingleDifference <= 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}

	//返回 GameplayTag，蓝图/动画系统可直接用
	return WarriorGameplayTags::Shared_Status_HitReact_Front;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	//通过两者的 ForwardVector 做 点乘
	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	//点乘 < 0 → 两者朝向相反 → 视为有效格挡
	//点乘 ≥ 0 → 攻击和防御方向相同 → 无效格挡
	return DotResult < -0.f;
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	//从施法者和目标 Actor 获取 UWarriorAbilitySystemComponent
	UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	//调用 ApplyGameplayEffectSpecToTarget
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC -> ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

	//返回是否应用成功
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,
	UPARAM(DisplayName = "Output") EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	//定义一个指向当前世界的指针。
	UWorld* World = nullptr;

	//GEngine 是全局引擎实例。
	//GetWorldFromContextObject 是 Unreal 提供的标准方式，根据传入对象找到对应的 UWorld。
	//如果找不到，会打印错误日志并返回 nullptr。
	if (GEngine)
	{
		World = GEngine -> GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	//找不到世界就直接返回（防止崩溃）
	if (!World)
	{
		return;
	}

	//获取当前世界的延迟动作管理器（FLatentActionManager）。它负责管理所有延迟节点（比如 Delay、Timeline、自定义延迟任务等）
	FLatentActionManager& LatentActionManager = World -> GetLatentActionManager();

	//查找当前世界中是否已有同一个 UUID 的倒计时任务。意思是：一个蓝图节点若已启动一个倒计时，再次执行 CountDown(Start) 时不会重复创建。
	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	//如果输入是 Start
	if (CountDownInput == EWarriorCountDownActionInput::Start)
	{
		//当前没有正在执行的倒计时
		if (!FoundAction)
		{
			//那就创建一个新的 FWarriorCountDownAction 对象
			//注册到 LatentActionManager；
			//从此刻开始，它会自动在 Tick 中执行 UpdateOperation()
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FWarriorCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo)
			);
		}
	}

	//如果输入是 Cancel
	if (CountDownInput == EWarriorCountDownActionInput::Cancel)
	{
		//查找到对应的倒计时任务
		if (FoundAction)
		{
			//调用 CancelAction() → 将 bNeedToCancel 设为 true；
			//在下一帧 UpdateOperation() 中检测到此标志，就会立即结束倒计时
			FoundAction -> CancelAction();
		}
	}
}

