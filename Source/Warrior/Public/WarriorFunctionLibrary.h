// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorFunctionLibrary.generated.h"

class UWarriorAbilitySystemComponent;
class UPawnCombatComponent;

/**
 * 自定义的 蓝图函数库 UWarriorFunctionLibrary，让 GAS 和 GameplayTag 的操作更方便地在蓝图中使用
 * 继承自 UBlueprintFunctionLibrary，这意味着：
 * - 所有函数都应该是 static
 * - 可以从任何蓝图直接调用（不需要放在关卡或对象上）
 */
UCLASS()
class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//纯 C++ 的工具函数，不暴露给蓝图。
	//作用：从一个 Actor 获取它的 UWarriorAbilitySystemComponent。
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	//检查 Actor 是否已经有该 GameplayTag。如果没有，则添加
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	//如果 Actor 身上有这个 Tag，就移除它
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	//一个纯 C++ 的工具函数。
	//检查给定 Actor 是否包含指定 Tag。
	//返回一个 bool，适合在 C++ 代码里调用。
	static  bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	/*
	 * 上面 NativeDoesActorHaveTag 的蓝图友好版本。
	 * ExpandEnumAsExecs → 会把 OutConfirmType 变成两个蓝图执行引脚：Yes 和 No。
	 * 蓝图里可以这样用：
	 * 输入 Actor 和 Tag
	 * 根据结果走不同的执行路径（就像 Branch 节点）
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary",meta=(DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,EWarriorConfirmType& OutConfirmType);

	//这是一个静态函数，可以直接通过类名调用，而不用创建对象实例
	//直接返回从 InActor 找到的战斗组件指针，如果找不到一般返回 nullptr
	//native给c++使用
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	//DisplayName：蓝图节点的显示名字
	//BP_ 开头给蓝图使用
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary",meta=(DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
};
