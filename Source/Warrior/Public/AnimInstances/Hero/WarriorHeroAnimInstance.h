// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "WarriorHeroAnimInstance.generated.h"

class AWarriorHeroCharacter;

/**
 * 用来驱动 Hero 角色 的动画蓝图
 */
UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()

public:
	/*
	 *在动画实例初始化时调用（比如角色刚生成、动画蓝图刚绑定到 SkeletalMesh）。
	 *通常用来缓存角色引用，比如 OwningHeroCharacter = Cast<AWarriorHeroCharacter>(TryGetPawnOwner());
	 *这样后续更新动画时就能直接访问角色。
	 */
	virtual void NativeInitializeAnimation() override;
	/*
	 *每帧都会被调用，用来更新动画数据。
	 *“ThreadSafe” 意味着它可能在动画线程里执行，所以不能直接修改游戏状态，只能读数据。
	 *通常会在这里计算：速度、是否在空中、是否攻击、是否进入休息状态
	 */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	/*保存当前动画实例所属的角色（强引用）。
	 *VisibleDefaultsOnly：只能在蓝图里看到，不能修改。
	 *这样蓝图里可以直接访问角色的属性
	 */
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|Refrences")
	AWarriorHeroCharacter* OwningHeroCharacter;

	/*
	 *标记是否应该进入“放松状态”（Relax）。
	 *这个状态通常用于角色静止不动一段时间后，播放休息/待机动画
	 */
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	//休息状态触发的时间阈值，默认 5 秒
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.f;

	//记录角色已经静止多久（秒数）。 每帧更新，如果角色速度为 0 → 累加，否则重置为 0
	float IdleElapsedTime;
};
