// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_PickUpStones.generated.h"

class AWarriorStoneBase;

/**
 * 用来实现角色 拾取石头（Pick Up Stones） 的能力逻辑
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_PickUpStones : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayAbility Interface
	//激活能力,在 按键触发 / 事件触发 时调用。这里是开始拾取石头逻辑的入口点。
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;	
	//结束能力,在技能自然结束或被取消时调用。用于清理状态
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface

	//可在蓝图中调用，通常用于从 C++ 激活逻辑转入蓝图逻辑。
	//主要功能：
	//使用 Box Trace 搜索周围的 AWarriorStoneBase。
	//把检测到的石头加入 CollectedStones
	UFUNCTION(BlueprintCallable)
	void CollectStones();

	//可在蓝图中触发“使用已收集石头”的逻辑。
	//用途：
	//消耗石头以回复能量或生成投掷物。
	//通知其他系统（UI更新、任务进度）
	UFUNCTION(BlueprintCallable)
	void ConsumeStones();

private:
	//只能在蓝图里编辑，运行时不可编辑
	UPROPERTY(EditDefaultsOnly)
	//定义了检测盒体（Box Trace）的大小
	float BoxTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly)
	////定义了检测盒体（Box Trace）的距离
	FVector TraceBoxSize = FVector(100.f);

	UPROPERTY(EditDefaultsOnly)
	//指定哪些物体类型（ObjectType）会被检测。
	TArray<TEnumAsByte<	EObjectTypeQuery> > StoneTraceChannel;

	UPROPERTY(EditDefaultsOnly)
	//是否绘制 Debug Box（绿色或红色框）来可视化检测区域
	bool bDrawDebugShape = false;

	UPROPERTY()
	//存储已检测到并确认可拾取的石头对象。
	//方便在 ConsumeStones() 阶段统一处理。
	//使用 UPROPERTY() 避免 GC 清理引用。
	TArray<AWarriorStoneBase*> CollectedStones;
};
