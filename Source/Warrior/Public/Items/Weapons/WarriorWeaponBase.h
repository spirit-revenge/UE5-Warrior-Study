// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarriorWeaponBase();//构造函数
protected:
	//武器的网格
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UStaticMeshComponent * WeaponMesh;

	//武器的碰撞体
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;

public:
	//提供一个内联函数，让外部（比如角色的战斗组件）快速访问这个武器的碰撞盒子
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}
};
