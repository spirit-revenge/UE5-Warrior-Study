// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorCharacterAnimInstance.generated.h"

class AWarriorBaseCharacter;
class UCharacterMovementComponent;
/**
 * 专门给“角色”（Character）用的动画实例类
 */
UCLASS()
class WARRIOR_API UWarriorCharacterAnimInstance : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

public:
	/*
	 * 当动画实例被创建时调用（角色生成、动画蓝图绑定 SkeletalMesh 时）。
	 * 这里通常做一次性初始化
	 */
	virtual void NativeInitializeAnimation() override;

	/*
	 * 每帧都会调用，用来更新动画蓝图所需的参数
	 * 注意：“ThreadSafe”
	 * 可能在动画线程执行，不能改游戏状态，只能读数据。
	 * 不能生成 Actor、不能播放音效，只能计算动画数据
	 */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	/*
	 * 保存当前动画实例所属的角色（继承自 AWarriorBaseCharacter）。
	 * 方便蓝图和 C++ 直接访问角色状态。
	 */
	UPROPERTY()
	AWarriorBaseCharacter* OwningCharacter;

	//缓存角色的移动组件
	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;

	//只能在蓝图里看到，不能修改，不能在运行时修改。角色的水平速度大小。
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	float GroundSpeed;

	//是否有加速度。蓝图里可见，不能修改，不能在运行时修改。可以用来切换“跑动开始/停止”动画、触发转身动画等
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	bool bHasAcceleration;
};
