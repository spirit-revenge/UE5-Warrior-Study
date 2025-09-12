// Spirit Revenge All Rights Reserved


#include "DataAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	//遍历 NativeInputActions 数组
	for (const FWarriorInputActionConfig& InputActionConfig : NativeInputActions)
	{
		//这个 InputActionConfig 的 InputTag 是否和传入的 InInputTag 一样
		//并且 InputActionConfig.InputAction 不是空指针（防止空引用）
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			//找到匹配的就直接返回这个 InputAction，用于后续绑定
			return InputActionConfig.InputAction;
		}
	}

	//如果整个数组都找完了，没有任何匹配，就返回空指针，告诉调用者“没找到”
	return nullptr;
}
