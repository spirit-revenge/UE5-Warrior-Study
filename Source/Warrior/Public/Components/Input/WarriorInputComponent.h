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
	/*
	* UserObject：通常是一个类（比如 AWarriorHeroCharacter），表示要接收输入回调的对象类型。
	* CallbackFunc：函数指针类型，表示要绑定的回调函数（按下/松开）。
	*
	* InInputConfig：一个 UDataAsset_InputConfig，里面保存了输入动作和对应的 GameplayTag。
	* ContextObject：要接收回调的对象实例。
	* InputPressedFunc：当输入按下时调用的函数。
	* InputReleasedFunc：当输入松开时调用的函数。
	 */
	template<class UserObject,typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig,const FGameplayTag&  InInputTag,ETriggerEvent TriggerEvent,UserObject* ContextObject,CallbackFunc Func);

	//把输入配置（Data Asset）和角色的输入处理函数动态绑定到一起
	template<class UserObject,typename CallbackFunc>
	void BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig,UserObject* ContextObject,CallbackFunc InputPressedFunc,CallbackFunc InputReleasedFunc);
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

template <class UserObject, typename CallbackFunc>
void UWarriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig,
	UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
	//运行时断言，保证 InInputConfig 不是空指针。
	//如果为空就直接崩溃并输出错误信息，避免后面访问非法内存
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));

	//AbilityInputActions 是一个数组，每个元素包含：
	//一个 UInputAction*
	//一个对应的 FGameplayTag
	for (const FWarriorInputActionConfig AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		//检查配置是否有效
		if (!AbilityInputActionConfig.IsValid()) continue;

		//绑定 InputAction 的 Started 事件到 InputPressedFunc。
		//最后一个参数 AbilityInputActionConfig.InputTag 会传入回调，让回调函数知道是哪个技能的输入被触发。
		BindAction(AbilityInputActionConfig.InputAction,ETriggerEvent::Started,ContextObject,InputPressedFunc,AbilityInputActionConfig.InputTag);
		//同理，绑定 Completed 事件到 InputReleasedFunc
		BindAction(AbilityInputActionConfig.InputAction,ETriggerEvent::Completed,ContextObject,InputReleasedFunc,AbilityInputActionConfig.InputTag);
	}
}
