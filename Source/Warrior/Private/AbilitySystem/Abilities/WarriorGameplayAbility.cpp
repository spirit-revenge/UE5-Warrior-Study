// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"

//这个函数会在 Ability 被授予（Granted） 时调用，比如角色学会了一个技能，或者通过装备/被动获得一个技能
void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	//执行父类，保证引擎的默认行为正常执行
	Super::OnGiveAbility(ActorInfo, Spec);

	//检查当前技能的 激活策略 是否是 OnGiven
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		//角色存在并且没有处于激活状态
		if (ActorInfo && !Spec.IsActive())
		{
			//这行代码会尝试立即激活这个技能（相当于自动帮玩家“按下技能键”）
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

//这个函数在技能 结束 的时候调用，不管是自然结束、冷却结束，还是被取消都会触发
void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	//依然检查激活策略是否是 OnGiven。 如果是，并且 ActorInfo 有效，就调用
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		//判断是否存在
		if (ActorInfo)
		{
			//把这个技能从 Ability System 里彻底清除（相当于“移除技能”）
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

//不需要手动去 Cast 或手动拿到 Pawn
UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	//GetAvatarActorFromActorInfo() -> 这是 UGameplayAbility 提供的函数，返回技能的“AvatarActor”，也就是技能所属的角色（Pawn 或 Character）
	//FindComponentByClass<UPawnCombatComponent>() -> 在这个角色身上查找类型为 UPawnCombatComponent 的组件
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

//直接访问扩展的能力系统
UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
	//CurrentActorInfo -> 是 UGameplayAbility 里保存的当前技能的上下文信息，里面有 AbilitySystemComponent
	//AbilitySystemComponent -> UE 的核心组件，管理这个角色的所有 GameplayAbilities、GameplayEffects、Attributes。
	//Cast<UWarriorAbilitySystemComponent>() -> 把基类指针转成你项目里自定义的 UWarriorAbilitySystemComponent，这样就可以访问你扩展的功能
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	//用蓝图库安全获取目标 ASC，支持任何实现了 IAbilitySystemInterface 的 Actor。
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	//确保目标 ASC 和 SpecHandle 有效，开发时直接 Crash，方便调试。
	check(TargetASC && InSpecHandle.IsValid());

	//用当前 Ability 的 Owner ASC 调用 ApplyGameplayEffectSpecToTarget，这是 GAS 推荐做法
	//它会自动处理本地预测、RPC 调用
	//正确设置 Instigator 和 EffectCauser，保证 GameplayEffect 的来源信息正确
	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
{
	//调用 NativeApplyEffectSpecHandleToTarget，保证核心逻辑集中。
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor,InSpecHandle);

	//使用 WasSuccessfullyApplied() 检查应用是否成功，非常好，这是 GAS 官方推荐的检查方式。
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;

	//返回句柄给蓝图，让蓝图有机会存储，后续可以用 RemoveActiveGameplayEffect 移除
	return ActiveGameplayEffectHandle;
}

void UWarriorGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle,
	const TArray<FHitResult>& InHitResults)
{
	//如果没有命中目标，直接返回。
	if (InHitResults.IsEmpty())
	{
		return;
	}

	//GetAvatarActorFromActorInfo() 是从当前 Ability 的上下文（FGameplayAbilityActorInfo）里获取执行该技能的 Pawn。
	//CastChecked 会在转换失败时报错（防御性编程）
	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

	//遍历每个命中的对象，只对 APawn 类型的目标有效（忽略非角色类物体）。
	for (const FHitResult& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			//调用 UWarriorFunctionLibrary::IsTargetPawnHostile 判断敌我关系。
			if (UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				//NativeApplyEffectSpecHandleToTarget 是项目自定义的函数（通常包装了 GAS 的 ApplyGameplayEffectSpecToTarget）。
				//它会把该 GameplayEffectSpec 应用到命中的目标的 AbilitySystemComponent 上。
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				//如果效果成功应用（如伤害命中目标），则构造一个 FGameplayEventData。
				//并且指定发起者和目标
				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;

					//通过 UAbilitySystemBlueprintLibrary::SendGameplayEventToActor 向目标发送一个 Gameplay Event。
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						WarriorGameplayTags::Shared_Event_HitReact,
						Data
					);
				}
			}
		}
	}
}
