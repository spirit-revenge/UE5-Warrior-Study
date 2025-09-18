// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "Characters/WarriorEnemyCharacter.h"

AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	//判断是否存在
	if (!CachedWarriorEnemyCharacter.IsValid())
	{
		//第一次调用时用 Cast<AWarriorEnemyCharacter> 转换 AvatarActor，并存储在 CachedWarriorEnemyCharacter。
		CachedWarriorEnemyCharacter = Cast<AWarriorEnemyCharacter>(CurrentActorInfo -> AvatarActor);
	}
	//用三元运算符保证即便缓存失效也返回 nullptr，而不是直接解引用
	return CachedWarriorEnemyCharacter.IsValid() ? CachedWarriorEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	//从actor获取enemy character 并返回 EnemyCombatComponent
	return GetEnemyCharacterFromActorInfo() -> GetEnemyCombatComponent();
}
