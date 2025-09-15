// Spirit Revenge All Rights Reserved


#include "Items/Weapons/WarriorHeroWeapon.h"

void AWarriorHeroWeapon::AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
	//把外部传进来的 SpecHandles 赋值给武器内部的 GrantedAbilitySpecHandles 数组
	GrantedAbilitySpecHandles = InSpecHandles;
}

TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
	//返回保存下来的 SpecHandles。
	return GrantedAbilitySpecHandles;
}
