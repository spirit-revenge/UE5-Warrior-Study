// Spirit Revenge All Rights Reserved


#include "AnimInstances/WarriorCharacterAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	//UAnimInstance 自带函数，返回当前动画实例绑定的 Pawn。在动画蓝图第一次运行时调用，通常角色刚生成、SkeletalMesh 绑定动画蓝图时触发
	//Cast<AWarriorBaseCharacter>(TryGetPawnOwner())把 Pawn 转成基类 AWarriorBaseCharacter
	OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());

	//如果该对象存在
	if (OwningCharacter)
	{
		//缓存角色的移动组件，后续每帧可以直接用它来读取速度、加速度，而不用每次都去
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	//防止角色不存在时访问无效内存，保证安全
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	//GetVelocity（）得到当前的速度向量
	//Size2D，忽略垂直方向
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	
	//GetCurrentAcceleration() 返回角色当前的加速度向量
	//SizeSquared2D() 返回加速度平方长度，效率比 Size2D() 高（避免开方运算）
	//判断是否大于 0，如果大于 0，说明玩家正在输入移动方向（有加速度）
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D()>0.f;

	//CalculateDirection 会返回一个 浮点角度，表示角色移动方向相对于自身前方的偏移角：0° → 向前，90° → 向右，-90° → 向左，180° 或 -180° → 向后
	//Velocity (OwningCharacter->GetVelocity())
	//	获取角色当前的世界速度向量 (X, Y, Z)
	//	速度为零时，角色静止
	//ActorRotation (OwningCharacter->GetActorRotation())
	//	获取角色当前朝向（旋转）
	//	用于将速度向量转换到角色本地空间
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}
