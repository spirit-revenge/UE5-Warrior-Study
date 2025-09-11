// Spirit Revenge All Rights Reserved


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"

void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	//调用父类 (UWarriorCharacterAnimInstance) 的初始化逻辑。
	//确保 OwningCharacter 和 OwningMovementComponent 已经被正确缓存
	Super::NativeInitializeAnimation();

	//确认角色引用存在，避免空指针
	if (OwningCharacter)
	{
		//把基类角色指针 OwningCharacter 转成玩家专用的 AWarriorHeroCharacter。
		//缓存在 OwningHeroCharacter 里，后续动画更新时可以直接访问玩家专属逻辑
		OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
	}
}

void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	//先让父类更新 GroundSpeed 和 bHasAcceleration。
	//这样当前类就能用这些基础数据继续扩展逻辑
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	//当角色有加速度（在移动）
	if (bHasAcceleration)
	{
		//清零“站立计时”，并确保不进入休息状态
		IdleElapsedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	//当角色没有加速度
	else
	{
		//站立时间累积。
		//一旦累积时间超过 EnterRelaxStateThreshold（默认 5 秒），就把 bShouldEnterRelaxState 置为 true。
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThreshold);
	}
}
