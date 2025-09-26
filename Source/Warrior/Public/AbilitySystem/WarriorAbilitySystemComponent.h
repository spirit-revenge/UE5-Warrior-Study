// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

/*
 * 技能组件类，用来处理角色的技能（Abilities）、技能输入（Input），以及动态授予/移除技能的逻辑
 */
UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	//这两个函数用来响应 技能输入事件。
	//通常会在玩家按下某个按键时调用 OnAbilityInputPressed，然后通知 GAS 去激活对应的技能
	//当按下摸个按键是调用
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	//当松开某个按键时调用
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	//UFUNCTION(BlueprintCallable)：表示这个函数可以在 Blueprint 里被调用。
	//遍历传入的 InDefaultWeaponAbilities 数组（里面可能是技能类、输入绑定等信息）。
	//根据 ApplyLevel 为角色授予技能。
	//把授予的技能的 FGameplayAbilitySpecHandle 保存到 OutGrantedAbilitySpecHandles 数组里，以便后续移除。
	UFUNCTION(BlueprintCallable, Category="Warrior|Ability",meta=(ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	//移除之前授予的技能。
	UFUNCTION(BlueprintCallable, Category="Warrior|Ability")
	void RemovedGrantedHeroWeaponAbilities(UPARAM(ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category="Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
