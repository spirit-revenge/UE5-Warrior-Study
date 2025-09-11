// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorHeroGameplayAbility.generated.h"

class AWarriorHeroCharacter;
class AWarriorHeroController;
/**
 * hero的技能类，可以获取hero的独特技能
 */
UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

public:
	//获取当前hero的角色，方便直接拿到玩家角色对象（Character），不用手动 Cast
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

	//获取当前hero的controller，方便直接访问玩家控制器，比如读取输入、HUD、镜头控制
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorHeroController* GetHeroControllerFromActorInfo();

	//直接访问角色的战斗功能，比如技能连招、专属攻击
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();
private:
	//弱引用指针（TWeakObjectPtr），缓存角色和控制器
	//通常在 GetHeroCharacterFromActorInfo() 里先检查缓存，如果没有就从 ActorInfo 里取一次并保存，减少重复查找，提高性能
	TWeakObjectPtr<AWarriorHeroCharacter> CashedWarriorHeroCharacter;
	TWeakObjectPtr<AWarriorHeroController> CashedWarriorHeroController;
};
