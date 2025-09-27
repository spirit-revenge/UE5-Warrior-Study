// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "EnemyCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UEnemyCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()
public:
	//当敌人武器或攻击命中目标时，这个函数会被调用
	virtual void OnHitTargetActor(AActor* HitActor) override;
};
