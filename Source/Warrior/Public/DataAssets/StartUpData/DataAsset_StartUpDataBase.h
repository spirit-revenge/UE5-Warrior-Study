// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;
class UGameplayEffect;
/**
* 继承自 UDataAsset：
* UDataAsset 是 UE 的一种数据资源，主要用来存放配置数据（而不是行为逻辑）。
* 这样你可以在 内容浏览器 里创建不同的 StartUpData 资产，为不同角色配置不同的起始能力和被动技能
 */
UCLASS()
class WARRIOR_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	/*
	 * 这是启动的入口函数。
	 * 传入一个 UWarriorAbilitySystemComponent（角色身上的 AbilitySystemComponent），以及技能等级（默认 1）。
	 * 在这里会把本 DataAsset 里配置的技能，授予这个 ASC。
	 * 也就是角色一出生，就自动获得这里配置的技能
	*/
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
	
protected:
	//授予时立刻激活的能力
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;

	//反应类能力，一般不会立即激活，而是等条件触发
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;
	
	//实际授予能力的内部函数。
	//遍历传入的技能数组，逐个授予到 AbilitySystemComponent。
	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
};
