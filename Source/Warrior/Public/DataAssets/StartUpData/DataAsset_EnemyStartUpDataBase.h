// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpDataBase.generated.h"

class UWarriorEnemyGameplayAbility;

/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_EnemyStartUpDataBase : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	//用于批量赋予敌人初始技能和效果。
	//可以覆盖父类的 GiveToAbilitySystemComponent 方法，添加敌人专有逻辑
	//TArray<TSubclassOf<UWarriorEnemyGameplayAbility>>：存储一组 敌人技能类（不是实例）
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1) override;
	
private:
	//EnemyCombatAbilities → 专门存储敌人的战斗技能
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorEnemyGameplayAbility>> EnemyCombatAbilities;
};
