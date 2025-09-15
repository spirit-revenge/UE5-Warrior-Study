// Spirit Revenge All Rights Reserved


#include "WarriorTypes/WarriorStructTypes.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

/*
 * InputTag.IsValid()
 * 确保这个配置的输入标签是合法的（不是空 GameplayTag）。
 * 避免出现授予了一个技能但是没有输入绑定的情况。
 * && AbilityToGrant
 * 确保 AbilityToGrant 指向一个有效的 UWarriorHeroGameplayAbility 类。
 * 避免授予 nullptr，否则 GAS 在 GiveAbility() 时会报错或无效。
 */
bool FWarriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
