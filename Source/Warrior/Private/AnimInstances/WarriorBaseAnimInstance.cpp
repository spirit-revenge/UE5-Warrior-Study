// Spirit Revenge All Rights Reserved


#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorFunctionLibrary.h"

bool UWarriorBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	//获取当前动画实例所挂载的 Pawn，判断是否存在
	if (APawn* OwningPawn = TryGetPawnOwner())
	{
		//把实际的标签查询逻辑集中到 函数库 里。
		//这样不仅 AnimInstance 可以用，别的系统（比如 AI、UI、GameplayAbility）也能用同一套函数
		return UWarriorFunctionLibrary::NativeDoesActorHaveTag(OwningPawn, TagToCheck);
	}
	//如果 TryGetPawnOwner() 失败，说明动画实例没有挂到任何 Pawn，就直接返回 false
	return false;
}
