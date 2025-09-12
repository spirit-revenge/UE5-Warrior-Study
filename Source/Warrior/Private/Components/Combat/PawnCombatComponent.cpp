// Spirit Revenge All Rights Reserved


#include "Components/Combat/PawnCombatComponent.h"
#include "WarriorDebugHelper.h"
#include "Items/Weapons/WarriorWeaponBase.h"

//把生成的武器存到 CharacterCarriedWeaponMap
void UPawnCombatComponent::RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister,
	AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	//检查，确保不会重复注册相同 Tag 的武器，否则直接报错（防止逻辑错误）
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),TEXT("A attack named %s has already been addedas carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	//确保传进来的武器不是 nullptr
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister,InWeaponToRegister);

	// 如果要求注册的同时装备，就更新当前装备武器的 Tag
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}

	// 打印调试信息，方便在输出日志里看到是哪把武器被注册了
	const FString WeaponString = FString::Printf(TEXT("A weapon named: %s has been registered using  hte tag %s"),*InWeaponToRegister->GetName(),*InWeaponTagToRegister.ToString());
	Debug::Print(WeaponString);
}

//通过 GameplayTag 查询角色是否持有指定武器。如果有，返回那把武器的指针。如果没有，返回 nullptr
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	//确保已经有Tag注册的该武器
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		// Find 返回 value 的指针，如果找到了就解引用返回
		if(AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}

	return nullptr;
}

//检查当前是否有装备武器（Tag 有效性）
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}
