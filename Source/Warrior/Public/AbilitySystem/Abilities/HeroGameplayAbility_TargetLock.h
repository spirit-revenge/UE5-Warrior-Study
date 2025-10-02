// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Widgets/WarriorWidgetBase.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UInputMappingContext;
class UWarriorWidgetBase;

/**
 * 目标锁定（Target Lock）技能类声明
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayAbility Interface
	//激活能力
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;	
	//结束能力
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

	UFUNCTION(BlueprintCallable)
	//每帧执行（通常绑定在 Ability Task 或 Tick 事件）
	void OnTargetLockTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	//根据 InSwitchDirectionTag来切换最近的可锁定目标
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);
private:
	//尝试找到一个目标来锁定
	void TryLockOnTarget();
	//通过 BoxTrace 获取所有候选目标
	void GetAvailableActorsToLock();
	//从候选中选择最近的目标（通常是初始锁定时使用）
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);
	//以当前目标为中心，找出左右侧可切换的目标
	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight);
	//在 UI 上绘制锁定框并更新位置
	void DrawTargetLockWidget();
	void SetTargetLockWidgetPosition();
	//调整角色移动速度（防止乱冲）
	void InitTargetLockMovement();
	//初始化输入映射，比如支持方向切换目标的按键
	void InitTargetLockMappingContext();
	//主动结束能力
	void CancelTargetLockAbility();
	//清空能力
	void CleanUp();
	//调整角色移动速度（防止乱冲）
	void ResetTargetLockMovement();
	//切换输入映射，比如支持方向切换目标的按键
	void ResetTargetLockMappingContext();
	
	// === 用于目标检测的 BoxTrace（大范围 + 可调试
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float BoxTraceDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	FVector TraceBoxSize = FVector(5000.f, 5000.f, 300.f);

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TArray< TEnumAsByte < EObjectTypeQuery > > BoxTraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	bool bShowPersistentDebugShape = false;
	// ===

	//锁定 UI Widget类
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	// === 控制移动速度 & 角色朝向平滑度
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockRotationInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockMaxWalkSpeed = 150.f;
	// ===

	//输入系统扩展（Enhanced Input），方便增加“切换目标”功能
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	UInputMappingContext* TargetLockMappingContext;

	//相机偏移量
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockCameraOffsetDistance = 20.f;

	//候选目标列表
	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;

	//当前锁定目标
	UPROPERTY()
	AActor* CurrentLockedActor;

	//绘制的锁定 UI Widget
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;

	//锁定 UI Widget大小
	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;

	// 缓存的最大移动速度
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
};


