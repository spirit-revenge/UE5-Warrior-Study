// Spirit Revenge All Rights Reserved


#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

#include "WarriorDebugHelper.h"

AWarriorWeaponBase::AWarriorWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//禁用了 Tick()，因为武器本身不需要每帧执行逻辑（角色和战斗组件会控制它）
	PrimaryActorTick.bCanEverTick = false;

	//创建武器的静态网格体组件，名字是 "WeaponMesh"。
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	//设置它为根组件，意味着武器的位置、旋转、缩放都由 WeaponMesh 决定
	SetRootComponent(WeaponMesh);

	//创建碰撞盒 WeaponCollisionBox。
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	//	附加到根组件（也就是 WeaponMesh），这样它会跟随武器模型移动。
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	//设置碰撞盒的大小，长宽高都是 20，这是默认值，后续你可以在编辑器里调
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	//默认禁用碰撞，避免武器平时就触发碰撞事件。
	//在攻击动画的关键帧，可以手动启用碰撞，用来检测攻击命中
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxEndOverlap);
}

void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwingPawn = GetInstigator<APawn>();

	checkf(WeaponOwingPawn,TEXT("Forgot to assign an instiagtor as the owning pawn for the weapon: %s"),*GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwingPawn != HitPawn)
		{
			//Debug::Print(GetName() + TEXT("begin overlap with ") + HitPawn -> GetName(),FColor::Green);
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
			//TODO:Implement hit check for enemy characters
		}
	}
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwingPawn = GetInstigator<APawn>();

	checkf(WeaponOwingPawn,TEXT("Forgot to assign an instiagtor as the owning pawn for the weapon: %s"),*GetName());

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwingPawn != HitPawn)
		{
			//Debug::Print(GetName() + TEXT("end overlap with ") + HitPawn -> GetName(),FColor::Red);
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
			//TODO:Implement hit check for enemy characters
		}
	}
}

