// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorPuckUpBase.generated.h"

class USphereComponent;

UCLASS()
class WARRIOR_API AWarriorPuckUpBase : public AActor
{
	GENERATED_BODY()
	
public:
	//构造方法
	AWarriorPuckUpBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pick Up Interaction")
	USphereComponent* PickUpCollisionSphere;

	UFUNCTION()
	//OverlappedComponent	触发重叠的组件（这里是 PickUpCollisionSphere）。
	//OtherActor	进入范围的 Actor（通常是玩家角色）。
	//OtherComp	玩家身上哪个组件触发了重叠。
	//bFromSweep	是否是通过 Sweep 检测触发的（一般为 true）。
	//SweepResult	包含碰撞点、法线等信息。
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
