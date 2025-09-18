// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

//宏，用于声明一个带 1 个参数 的普通 C++ 委托
//AActor* 这是委托的参数类型，表示触发事件时会把 目标 Actor 传递给监听者
DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*);

UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarriorWeaponBase();//构造函数

	//表示“武器命中目标”或者“武器离开目标”的事件
	FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
protected:
	//武器的网格
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UStaticMeshComponent * WeaponMesh;

	//武器的碰撞体
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;

	//当武器碰撞盒开始与其他组件重叠时调用
	//可用于检测命中目标并触发 OnWeaponHitTarget 委托
	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//当武器碰撞盒结束与其他组件重叠时调用
	//可用于触发 OnWeaponPulledFromTarget 或清理状态
	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	//提供一个内联函数，让外部（比如角色的战斗组件）快速访问这个武器的碰撞盒子
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}
};
