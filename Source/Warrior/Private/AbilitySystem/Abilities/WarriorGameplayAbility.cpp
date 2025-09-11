// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"

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
