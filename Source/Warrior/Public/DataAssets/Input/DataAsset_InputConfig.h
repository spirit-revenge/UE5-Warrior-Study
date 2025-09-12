// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DataAsset_InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

//这个结构体就是把 标签 和 动作 绑定成一个配置单元
USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
	GENERATED_BODY()
	
public:
	/*
	 * InputTag
	 * 类型：FGameplayTag
	 * 用来给这个输入动作打上一个唯一的标签，例如 InputTag.Move、InputTag.Attack。
	 * 优点是：通过标签系统，可以用数据驱动的方式来查找、绑定输入，而不是在代码里硬编码某个 UInputAction*。
	 */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories = "InputTag"))
	FGameplayTag InputTag;

	/*
	 * InputAction
	 * 类型：UInputAction*
	 * 就是 Enhanced Input 里的输入动作对象。
	 * 在编辑器里可以直接拖拽一个 InputAction 资源进来
	 */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputAction;
};
/**
 * 数据资产类，用来集中管理输入配置
 */
UCLASS()
class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	//表示当前角色使用的输入映射上下文，可以在 BeginPlay 或 Possessed 时添加到 EnhancedInputSubsystem，让这个角色能响应对应的输入
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;

	//一个数组，存储了多个标签和输入动作的绑定。
	//用了 TitleProperty = "InputAction"，所以在编辑器里会显示 InputAction 的名字，方便查看。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty = "InputAction"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	//用来通过 InputTag 查找对应的 InputAction
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const;
};
