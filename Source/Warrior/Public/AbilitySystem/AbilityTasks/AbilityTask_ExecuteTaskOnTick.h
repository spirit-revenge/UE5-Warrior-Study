// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ExecuteTaskOnTick.generated.h"

/*
 * DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam
	声明一个 动态多播委托，可以在蓝图里绑定多个函数。
	动态（Dynamic）：可以在运行时动态绑定/解绑，而且能暴露给蓝图。
	多播（Multicast）：允许多个监听者同时订阅。
	FOnAbilityTaskTickDelegate
	这是生成的委托类型名。
	float, DeltaTime
	表示这个委托会广播一个参数：float DeltaTime。
	👉意思就是：每次 Tick 都会通知所有监听者，并且告诉他们这帧的 DeltaTime。
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskTickDelegate, float, Deltatime);

/**
 * 自定义 GameplayAbilityTask，它的作用就是在 Ability 激活后，每帧执行一个 Tick 回调
 */
UCLASS()
class WARRIOR_API UAbilityTask_ExecuteTaskOnTick : public UAbilityTask
{
	GENERATED_BODY()

public:
	//构造方法
	UAbilityTask_ExecuteTaskOnTick();

	//这是 GAS 的标准写法：
	//	OwningAbility 是这个 Task 依附的 GameplayAbility。
	//	BlueprintInternalUseOnly = "true" 表示在蓝图中不会显示这个节点，只能通过能力内部调用。
	//	ExecuteTaskOnTick 就是这个 Task 的工厂函数，用来创建一个实例并启动
	UFUNCTION(BlueprintCallable, Category = "Warrior|AbilityTasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_ExecuteTaskOnTick* ExecuteTaskOnTick(UGameplayAbility* OwningAbility);

	//~ Begin UGameplayTask Interface
	//每一帧都会被调用（只要这个 Task 还在运行）。
	//DeltaTime 就是帧间隔
	virtual void TickTask(float DeltaTime) override;
	//~ End UGameplayTask Interface

	//这是一个 BlueprintAssignable 的事件，意思是蓝图可以绑定回调
	UPROPERTY(BlueprintAssignable)
	FOnAbilityTaskTickDelegate OnAbilityTaskTick;
};
