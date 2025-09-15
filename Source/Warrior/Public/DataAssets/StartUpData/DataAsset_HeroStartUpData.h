// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "DataAsset_HeroStartUpData.generated.h"

/**
 * 给“Hero”角色用的配置数据资产类
 */
UCLASS()
class WARRIOR_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	//把此 Data Asset 的能力赋予指定的 Ability System Component
	//ApplyLevel 决定授予能力的等级（通常是 1）
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1) override;
	
private:
	UPROPERTY(EditDefaultsOnly,Category="StartUpData",meta = (TitleProperty = "InputTag"))
	//可以在编辑器里直接为“英雄角色”配置初始能力
	//FWarriorHeroAbilitySet为hero的AbilitySet
	TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;
};
