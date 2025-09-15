// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorHeroLinkedAnimLayer.generated.h"

class UWarriorHeroAnimInstance;

/**
 * hero 的 Linked Anim Layer
 * Linked Anim Layer 是一种动画蓝图功能，它允许你把动画逻辑拆分成多个蓝图模块，然后动态切换或叠加
 * Linked Anim Layer 的典型用途：
 * - 分离上半身、下半身动画。
 * - 让武器动画单独在一个 Layer 播放，而不会干扰全身动作。
 * - 多角色复用同一套 Layer，而不需要复制整个 AnimBP。
 */
UCLASS()
class WARRIOR_API UWarriorHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

public:
	//BlueprintPure：表示这个函数不会修改任何状态，可以直接在蓝图中调用，不需要执行节点。
	//BlueprintThreadSafe：表示这个函数在多线程动画更新中也是安全的（不会修改数据）。
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	//返回主动画实例的指针。
	UWarriorHeroAnimInstance*  GetHeroAnimInstance() const;
};
