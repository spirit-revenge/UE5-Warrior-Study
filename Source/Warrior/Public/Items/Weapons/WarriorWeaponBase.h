// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*);

UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarriorWeaponBase();//构造函数

	FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
protected:
	//武器的网格
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UStaticMeshComponent * WeaponMesh;

	//武器的碰撞体
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	//提供一个内联函数，让外部（比如角色的战斗组件）快速访问这个武器的碰撞盒子
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}
};
