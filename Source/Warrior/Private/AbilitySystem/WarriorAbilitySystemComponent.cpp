// Spirit Revenge All Rights Reserved


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	//检查输入是否有效
	if (!InInputTag.IsValid())
	{
		return;
	}

	//GetActivatableAbilities() 返回所有已经授予并且能激活的技能（FGameplayAbilitySpec）。
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//每个 AbilitySpec 都有一组 DynamicAbilityTags（动态标签），这里用 HasTagExact 判断是否匹配当前输入的标签。
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		//如果匹配，就调用 TryActivateAbility(AbilitySpec.Handle) 来尝试激活这个技能。
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	//TODO
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(
	const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	//检查输入数组是否为空
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	//遍历能力集合 FWarriorHeroAbilitySet 
	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		//，检查当前 AbilitySet 是否有效。
		if (!AbilitySet.IsValid()) continue;

		//创建一个新的 FGameplayAbilitySpec，表示要授予的技能。
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		//SourceObject 设置为当前 AvatarActor（通常是角色自己）。
		AbilitySpec.SourceObject = GetAvatarActor();
		//Level 设置为 ApplyLevel，控制技能等级。
		AbilitySpec.Level = ApplyLevel;
		//DynamicAbilityTags 添加输入标签（用于匹配按键输入）
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		//调用 GiveAbility() 将能力添加到这个 ASC。
		//把返回的 SpecHandle 存到 OutGrantedAbilitySpecHandles 数组，方便以后移除。
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	//判断数组是否为空数组，如果是则直接跳出
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		//检查是否有要移除的句柄
		if (SpecHandle.IsValid())
		{
			//用 ClearAbility(SpecHandle) 移除之前授予的技能。
			ClearAbility(SpecHandle);
		}
	}

	//清空数组，避免数组中保留已经无效的句柄。
	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}
