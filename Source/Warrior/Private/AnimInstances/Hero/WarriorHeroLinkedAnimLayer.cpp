// Spirit Revenge All Rights Reserved


#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"

UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	// GetOwningComponent() 返回这个 AnimInstance 所属的 USkeletalMeshComponent
	// GetAnimInstance() 从 USkeletalMeshComponent 获取当前正在使用的 UAnimInstance。
	// 把 UAnimInstance 强制转换为 UWarriorHeroAnimInstance 类型。
	return Cast<UWarriorHeroAnimInstance> (GetOwningComponent()->GetAnimInstance());
}
