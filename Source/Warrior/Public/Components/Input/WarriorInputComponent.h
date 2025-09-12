// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "WarriorInputComponent.generated.h"

/**
 * 对 UE5 Enhanced Input 系统 的一个封装，让你用 GameplayTag 来绑定输入
 * 比直接写死 UInputAction* 更灵活
 */
UCLASS()
class WARRIOR_API UWarriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	//提供一个模板函数 BindNativeInputAction，用来 绑定输入行为（Input Action）和 C++ 回调函数
	template<class UserObject,typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig,const FGameplayTag&  InInputTag,ETriggerEvent TriggerEvent,UserObject* ContextObject,CallbackFunc Func);
	
};

template <class UserObject, typename CallbackFunc>
void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig,
	const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	//check(InInputConfig);

	// 检查输入配置是否有效
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));

	//// 根据 GameplayTag 查找对应的 InputAction
	if (UInputAction* FoundAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		/*
		* BindAction(...) 是UEnhancedInputComponent 的函数，把 FoundAction 和 ContextObject->Func 绑定起来。
		* TriggerEvent 指定触发时机，比如 ETriggerEvent::Triggered 表示输入轴值变化时触发。
		* ContextObject 通常是你的角色类（比如 this）。
		* Func 是要调用的函数，比如 &ThisClass::Input_Move。
		 */
		BindAction(FoundAction,TriggerEvent,ContextObject,Func);
	}
}
