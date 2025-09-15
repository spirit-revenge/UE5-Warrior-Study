// Spirit Revenge All Rights Reserved


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	//父类 UPawnCombatComponent 提供的函数，返回一个 AWarriorWeaponBase*，根据传入的 FGameplayTag 查找角色当前携带的武器
	//安全类型转换，转换成子类AWarriorHeroWeapon
	 return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}
