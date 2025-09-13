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
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1) override;
	
private:
	UPROPERTY(EditDefaultsOnly,Category="StartUpData",meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;
};
