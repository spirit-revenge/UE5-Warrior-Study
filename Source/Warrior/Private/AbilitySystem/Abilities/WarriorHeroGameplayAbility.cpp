// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	//如果缓存里的hero对象不存在，则获取当前AvatarActor对象，转换成Hero对象并存在缓存里
	if (!CashedWarriorHeroCharacter.IsValid())
	{
		CashedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	//判断缓存里是否有有效的角色对象，返回它，否则返回null
	return CashedWarriorHeroCharacter.IsValid()? CashedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	//检查缓存是否有效，如果无效 → 从 ActorInfo 获取 PlayerController，并强制转换成 AWarriorHeroController
	if (!CashedWarriorHeroController.IsValid())
	{
		CashedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}
	//判断缓存里是否有有效的角色控制器，返回它，否则返回null
	return CashedWarriorHeroController.IsValid()? CashedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	//得到当前hero对象的战斗组件
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}
