// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "GameplayTagContainer.h"

#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;

/**
 * 管理这个 Pawn（角色）拥有的武器
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	//把生成出来的武器注册到组件里，并用一个 GameplayTag 来标识它
	/**
	* InWeaponTagToRegister：用来区分不同的武器，比如 Weapon.Sword、Weapon.Rifle。
	* InWeaponToRegister：传入的武器实例。
	* bRegisterAsEquippedWeapon：如果为 true，说明注册时就把这把武器设为当前装备的武器
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister ,AWarriorWeaponBase* InWeaponToRegister,bool bRegisterAsEquippedWeapon = false);

	//通过 GameplayTag 查找这名角色是否携带指定武器，并返回武器指针
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	//记录当前装备武器的 GameplayTag,可在蓝图里读写
	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	//返回当前装备的武器实例
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;
private:
	//用来存储角色身上携带的所有武器。key 是 GameplayTag。value 是具体的武器对象指针。
	TMap<FGameplayTag,AWarriorWeaponBase* > CharacterCarriedWeaponMap;
};
