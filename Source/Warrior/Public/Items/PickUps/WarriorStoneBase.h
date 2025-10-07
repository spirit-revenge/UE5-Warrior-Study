// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Items/PickUps/WarriorPuckUpBase.h"
#include "WarriorStoneBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorStoneBase : public AWarriorPuckUpBase
{
	GENERATED_BODY()

public:
	//当石头被拾取（Consume）时执行。
	void Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel);
	
protected:
	//这是从 AWarriorPuckUpBase 继承来的事件重写。
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	//BlueprintImplementableEvent
	//表示这个函数 不需要在 C++ 里实现，而是让蓝图来实现视觉反馈逻辑。
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Stone Consumed"))
	void BP_OnStoneConsumed();
	
	UPROPERTY(EditDefaultsOnly)
	//它指定 拾取后要施加给角色的 GameplayEffect 类。
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
};
