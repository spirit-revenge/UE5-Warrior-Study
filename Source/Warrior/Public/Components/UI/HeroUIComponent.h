// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

//声明了一个 动态多播委托，带一个参数
//TSoftObjectPtr<UTexture2D>（软引用的武器图标纹理）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>,SoftWeaponIcon);

//声明了一个 带两个参数 的 动态多播委托类型，可以在蓝图中绑定多个响应函数。
//AbilityInputTag 技能输入标签（比如 "Input.Ability.Q"、"Input.Ability.Ultimate"），用于区分哪个技能被更新
//SoftAbilityIconMaterial 技能图标的软引用（通常是材质或 UI 图标），用于更新技能栏 UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdatedDelegate, FGameplayTag, AbilityInputTag, TSoftObjectPtr<UMaterialInterface>, SoftAbilityIconMaterial);

//AbilityInputTag 对应哪个技能的冷却事件
//TotalCooldownTime 冷却总时长
//RemainingCooldownTime 当前剩余冷却时间（一般刚开始等于总时长）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBeginDelegate, FGameplayTag, AbilityInputTag, float, TotalCooldownTime, float, RemainingCooldownTime);
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	//BlueprintAssignable 允许蓝图直接绑定这个事件
	UPROPERTY(BlueprintAssignable)
	//用于通知怒气值变化（0~1）
	FOnPercentChangedDelegate OnCurrentRageChanged;

	//BlueprintCallable 允许蓝图在实例上调用这个事件（通常用 Broadcast() 触发）
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	//参数类型是 TSoftObjectPtr<UTexture2D>，意味着武器图标是延迟加载的软引用资源
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	//BlueprintAssignable → 蓝图可以绑定事件（即“在蓝图中响应这个委托”）
	//BlueprintCallable → 蓝图可以调用该委托的函数（不过这里多用于触发器类）
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	//OnAbilityIconSlotUpdated 技能槽发生更新(换技能、学习新技能),更新技能图标、名称、描述等
	FOnAbilityIconSlotUpdatedDelegate OnAbilityIconSlotUpdated;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	//技能进入冷却状态,开启冷却计时条、灰化技能图标、播放冷却动画
	FOnAbilityCooldownBeginDelegate OnAbilityCooldownBegin;
};
