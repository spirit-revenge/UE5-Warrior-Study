// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "WarriorBaseAnimInstance.generated.h"

/**
 * 基础的 动画实例类
 */
UCLASS()
class WARRIOR_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	//蓝图调用
	/*
	* meta = (BlueprintThreadSafe)
	* 表示可以在 动画蓝图的线程安全环境（例如 Anim Graph Evaluate 阶段）里调用。
	* 很重要，因为 AnimInstance 的 EvaluateGraph 可能运行在 非游戏线程。
	* 加上 BlueprintThreadSafe，意味着这个函数 不会有副作用、也不会依赖非线程安全的数据
	 */
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	//判断当前拥有的角色是否有这个标签
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck) const;
};
