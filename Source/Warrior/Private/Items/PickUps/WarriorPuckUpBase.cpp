// Spirit Revenge All Rights Reserved


#include "Items/PickUps/WarriorPuckUpBase.h"

#include "Components/SphereComponent.h"

AWarriorPuckUpBase::AWarriorPuckUpBase()
{
	//关闭 Tick，提高性能（拾取物通常不需要帧更新）。
	PrimaryActorTick.bCanEverTick = false;

	//创建一个球形碰撞体组件，命名为 "PickUpCollisionSphere"。
	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	//将它设为该 Actor 的根组件。
	SetRootComponent(PickUpCollisionSphere);
	//设置默认半径为 50cm（可在蓝图里调整）。
	PickUpCollisionSphere -> InitSphereRadius(50.f);
	//将组件的重叠事件 绑定 到你的函数 OnPickUpCollisionSphereBeginOverlap。
	PickUpCollisionSphere -> OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnPickUpCollisionSphereBeginOverlap);
}

void AWarriorPuckUpBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	//由子类实现该方法
}


