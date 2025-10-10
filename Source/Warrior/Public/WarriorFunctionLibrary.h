// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorFunctionLibrary.generated.h"

class UWarriorGameInstance;
class UWarriorAbilitySystemComponent;
class UPawnCombatComponent;
struct FScalableFloat;

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
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

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

	//判断两个 Pawn 是否是敌对关系
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	////获取一个 FScalableFloat 在指定等级下的值
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary", meta=( CompactNodeTitle = "Get Value At Level" ))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.f);

	//计算受击角色应该播放的受击动画方向（前、后、左、右）
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutSingleDifference);

	//判断防御是否有效
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);

	//将 GameplayEffect 应用到目标 Actor
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary",
		//Latent 声明这个函数是一个 潜在（延迟）动作，类似于蓝图的 Delay 节点，会在后台异步执行
		//WorldContext = "WorldContextObject" 指定哪个参数提供当前世界上下文。
		//LatentInfo = "LatentInfo" 告诉引擎 FLatentActionInfo 是用于追踪此延迟动作的结构。
		//ExpandEnumAsExecs 在蓝图中会看到的针脚节点
		//TotalTime = "1.0" 蓝图节点中默认倒计时总时长为 1.0s。
		//UpdateInterval = "0.1" 蓝图节点中默认每 0.1 秒更新一次。
		meta=(Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo", ExpandEnumAsExecs = "CountDownInput|CountDownOutput",
			TotalTime = "1.0", UpdateInterval = "0.1"))
	//WorldContextObject 当前世界上下文对象（例如 PlayerController、Actor 等）。Unreal 内置机制通过它来找到对应的 UWorld 实例
	//TotalTime 倒计时的总时间长度（秒）。
	//UpdateInterval 每隔多长时间更新一次（触发一次“Updated”输出）
	//OutRemainingTime 输出参数，用于返回当前剩余倒计时。在蓝图中可以直接读取这个值，用于更新 UI（比如冷却时间文本）
	//CountDownInput 输入动作类型（Start 或 Cancel）。用来告诉系统要开始计时还是取消计时
	//CountDownOutput 输出执行结果（Updated、Completed、Cancelled）。蓝图里会作为执行引脚显示出来。
	//FLatentActionInfo LatentInfo Unreal 内置结构，用于标识异步动作的上下文。
	/** 对之前的 FWarriorCountDownAction 的 蓝图接口封装函数，它让蓝图可以以「延迟执行」的方式使用倒计时逻辑 **/
	static void CountDown(const UObject* WorldContextObject, float TotalTime,
		float UpdateInterval, float& OutRemainingTime,
		EWarriorCountDownActionInput CountDownInput,
		UPARAM(DisplayName = "Output") EWarriorCountDownActionOutput& CountDownOutput,
		FLatentActionInfo LatentInfo);

	//WorldContext参数表示：由 BlueprintCallable 函数使用，用于指示哪个参数用于确定操作发生的世界。
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	//从任意 蓝图或C++对象上下文（WorldContextObject） 中获取当前正在运行的 UWarriorGameInstance。
	static  UWarriorGameInstance* GetWarriorGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	//切换当前输入模式（只控制游戏、只控制UI等）。
	static void ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	//保存当前选择的游戏难度到磁盘。
	static void SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	//尝试从存档读取之前保存的游戏难度。
	static bool TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSavedDifficulty);
};
