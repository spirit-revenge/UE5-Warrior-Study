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
	
protected:
	//这个函数用来控制敌人攻击碰撞（左右手碰撞盒）的启用/关闭状态
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType) override;
};
