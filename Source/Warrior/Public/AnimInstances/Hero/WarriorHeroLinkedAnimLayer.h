// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorHeroLinkedAnimLayer.generated.h"

class UWarriorHeroAnimInstance;

/**
 * hero 的 Linked Anim Layer
 * Linked Anim Layer 是一种动画蓝图功能，它允许你把动画逻辑拆分成多个蓝图模块，然后动态切换或叠加
 */
UCLASS()
class WARRIOR_API UWarriorHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	UWarriorHeroAnimInstance*  GetHeroAnimInstance() const;
};
