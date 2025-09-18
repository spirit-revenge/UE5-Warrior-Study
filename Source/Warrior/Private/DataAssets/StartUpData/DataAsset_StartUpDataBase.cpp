// Spirit Revenge All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"

#include "WarriorDebugHelper.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "HAL/Platform.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
                                                              int32 ApplyLevel)
{
	//确保传入的 AbilitySystemComponent 不为空，否则直接崩溃（方便排查错误）
	check(InASCToGive);

	//分别调用 GrantAbilities 授予
	//ActivateOnGivenAbilities → 初始化时就激活的技能（比如基础攻击、冲刺）
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	//ReactiveAbilities → 被动或反应类技能（比如防御反击、回血）
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);
	//GrantAbilities → 内部函数，遍历数组，把每个技能实例化并通过 ASC 的 GiveAbility 赋予角色

	//判断不为空
	if (!StartUpGameplayEffects.IsEmpty())
	{
		//StartUpGameplayEffects 是 TArray<TSubclassOf<UGameplayEffect>>，存储角色初始效果
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			//对每个有效的 EffectClass：
			
			//通过 GetDefaultObject<UGameplayEffect>() 获取 CDO（Class Default Object），这是 UE 给类实例化的默认对象。
			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
			InASCToGive->ApplyGameplayEffectToSelf(//调用 ApplyGameplayEffectToSelf 应用效果到自己。
				EffectCDO,
				ApplyLevel,//ApplyLevel → 可以根据角色等级调整效果强度。
				InASCToGive->MakeEffectContext()//MakeEffectContext() → 创建效果上下文，包含来源 Actor、技能信息等。
			);
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,
	UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	//没有技能就直接返回，避免多余逻辑。
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	//循环遍历每个 TSubclassOf
	for (const TSubclassOf<UWarriorGameplayAbility>& Ability : InAbilitiesToGive)
	{
		//Ability 是一个类引用（必须是 UWarriorGameplayAbility 的子类）。
		//如果为空（设计师没填），直接跳过
		if (!Ability) continue;

		//FGameplayAbilitySpec 是授予技能所需的完整信息（包含类、等级、Owner）
		FGameplayAbilitySpec AbilitySpec(Ability);
		//SourceObject → 技能的来源对象，一般是角色自己 (AvatarActor)
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		//Level → 技能等级
		AbilitySpec.Level = ApplyLevel;

		//真正把这个技能添加到 ASC 里，让它成为角色可用技能
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
