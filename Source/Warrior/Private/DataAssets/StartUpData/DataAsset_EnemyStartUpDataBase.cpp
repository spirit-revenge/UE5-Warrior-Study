// Spirit Revenge All Rights Reserved


#include "DataAssets/StartUpData/DataAsset_EnemyStartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"

void UDataAsset_EnemyStartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel)
{
	//调用父类 UDataAsset_StartUpDataBase 的实现
	//这样敌人也能继承通用初始化逻辑
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);

	//判断数组是否为空
	if (!EnemyCombatAbilities.IsEmpty())
	{
		//遍历 EnemyCombatAbilities
		for (const TSubclassOf<UWarriorEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;

			//据类类型生成技能实例规格
			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			//指定技能来源为敌人自身
			AbilitySpec.SourceObject = InASCToGive -> GetAvatarActor();
			//设置技能等级
			AbilitySpec.Level = ApplyLevel;

			//将技能添加到敌人的 AbilitySystemComponent
			InASCToGive->GiveAbility(AbilitySpec);
		}
	}
}
