// Spirit Revenge All Rights Reserved


#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteTaskOnTick.h"

UAbilityTask_ExecuteTaskOnTick::UAbilityTask_ExecuteTaskOnTick()
{
	//表示这个 Task 每帧都会被调用 TickTask
	bTickingTask = true;
}

UAbilityTask_ExecuteTaskOnTick* UAbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	//NewAbilityTask<>() 会分配一个新的 Task 实例，并且挂到对应的 OwningAbility 上
	UAbilityTask_ExecuteTaskOnTick* Node = NewAbilityTask<UAbilityTask_ExecuteTaskOnTick>(OwningAbility);

	return Node;
}

void UAbilityTask_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
	//调用父类逻辑
	Super::TickTask(DeltaTime);

	//检查 Task 是否还能广播委托
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//如果还有效，就广播给蓝图/C++ 绑定的所有回调。
		//相当于一个局部的 TickEvent
		OnAbilityTaskTick.Broadcast(DeltaTime);
	}
	else
	{
		//如果不能再广播，说明 Task 失效，就结束并清理自己
		EndTask();
	}
}
