// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorEnemyGameplayAbility.generated.h"

class AWarriorEnemyCharacter;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

public:
	//返回当前 Ability 的 敌人角色实例
	//这是对 GetAvatarActorFromActorInfo() 的一个封装，直接强转成 AWarriorEnemyCharacter，方便在蓝图里使用。
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo();

	//直接返回敌人身上的 战斗组件，避免每次都手动 GetComponentByClass 或强转。
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat);
private:
	//TWeakObjectPtr：弱引用，不会阻止 GC（垃圾回收）。
	//作用：缓存敌人角色，避免多次查找（优化性能）。
	//使用弱引用的好处：
	//如果敌人死亡并被销毁，这个指针会自动失效（变成 nullptr），不会造成悬垂指针。
	//比裸指针安全，比 TStrongObjectPtr 性能更好（不会持有引用阻止 GC）。
	TWeakObjectPtr<AWarriorEnemyCharacter> CachedWarriorEnemyCharacter;
};
