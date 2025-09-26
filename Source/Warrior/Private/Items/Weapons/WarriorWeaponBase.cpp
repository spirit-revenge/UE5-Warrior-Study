// Spirit Revenge All Rights Reserved


#include "Items/Weapons/WarriorWeaponBase.h"

#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "Components/BoxComponent.h"

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
	//GetInstigator<APawn>() 获取这个武器的拥有者（通常是角色）
	APawn* WeaponOwingPawn = GetInstigator<APawn>();
	
	//checkf 确保拥有者存在，否则会触发断言并打印武器名称
	checkf(WeaponOwingPawn,TEXT("Forgot to assign an instiagtor as the owning pawn for the weapon: %s"),*GetName());


	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwingPawn, HitPawn))
		{
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
	
	//检查碰撞对象是否是 Pawn
	// if (APawn* HitPawn = Cast<APawn>(OtherActor))
	// {
	// 	//防止武器“击中自己”：
	// 	if (WeaponOwingPawn != HitPawn)
	// 	{
	// 		//Debug::Print(GetName() + TEXT("begin overlap with ") + HitPawn -> GetName(),FColor::Green);
	// 		
	// 		//调用委托
	// 		//如果有绑定函数（角色监听这个事件），就执行
	// 		//OtherActor 是被击中的 Pawn
	// 		OnWeaponHitTarget.ExecuteIfBound(OtherActor);
	// 	}
	// }
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//与 BeginOverlap 类似
	//这里触发的是“武器离开目标”的事件
	
	APawn* WeaponOwingPawn = GetInstigator<APawn>();
	
	checkf(WeaponOwingPawn,TEXT("Forgot to assign an instiagtor as the owning pawn for the weapon: %s"),*GetName());
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwingPawn, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}

