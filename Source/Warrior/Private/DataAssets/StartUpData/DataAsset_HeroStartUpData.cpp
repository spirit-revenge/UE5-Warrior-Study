// Spirit Revenge All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"

void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel)
{
	//调用父类的初始化逻辑
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	//遍历数组，HeroStartUpAbilitySets 是在编辑器里配置的初始技能列表。
	for (const FWarriorHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
	{
		//检查是否有效
		if (!AbilitySet.IsValid()) continue;

		//FGameplayAbilitySpec 是 GAS 用来描述“授予的技能”的结构体
		FGameplayAbilitySpec AbilitySpec (AbilitySet.AbilityToGrant);
		//告诉 GAS 这个技能的来源是谁（通常是当前角色）。
		//对技能里需要访问 AvatarActor 的逻辑非常有用
		AbilitySpec.SourceObject = InASCToGive -> GetAvatarActor();
		//让技能等级可配置（默认为 1）。
		AbilitySpec.Level = ApplyLevel;
		//把技能和 InputTag 关联起来。
		//后续 ASC->OnAbilityInputPressed(InInputTag) 时，会用这个 Tag 来激活对应技能
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		//真正把这个技能添加到 Ability System Component，使其可以被激活
		InASCToGive -> GiveAbility(AbilitySpec);
	}
}
