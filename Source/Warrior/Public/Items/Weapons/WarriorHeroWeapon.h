// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "GameplayAbilitySpecHandle.h"

#include "WarriorHeroWeapon.generated.h"

/**
 * hero专用的武器类
 */
UCLASS()
class WARRIOR_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
	GENERATED_BODY()

public:
	//EditDefaultsOnly → 只能在蓝图的 类默认值（CDO） 中修改。
	//BlueprintReadOnly → 可以在蓝图访问这个数据，但不能改。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WeaponData")
	//hero武器的数据
	FWarriorHeroWeaponData HeroWeaponData;

	UFUNCTION(BlueprintCallable)
	//通过这个函数把“ASC 授予给武器的技能句柄”记录下来。
	//常见用法：当你装备武器时，会把武器对应的技能授予 ASC，同时把返回的 SpecHandle 存到武器里，方便后续移除。
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

	UFUNCTION(BlueprintPure)
	//返回武器记录下来的能力句柄。
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;
private:
	//存储授予的技能句柄。
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
