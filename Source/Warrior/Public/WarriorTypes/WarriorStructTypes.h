// Spirit Revenge All Rights Reserved

#pragma once

#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "WarriorStructTypes.generated.h"

class UWarriorHeroLinkedAnimLayer;
class UWarriorHeroGameplayAbility;
class UInputMappingContext;

/*
 * 这个结构体是**“一条技能授予规则”**：
 * - 绑定了 技能类型 + 输入标签。
 * - 用于授予 FGameplayAbilitySpec 并附带正确的 InputTag。
 * - 支持数组化，可以配置多个能力（比如普攻、重击、闪避）。
 */
USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()

	/*
	* 标识该能力的输入标签。
	* 用于和输入系统绑定，比如 Ability.Attack.Light、Ability.Dodge。
	* 当玩家按下对应按键时，ASC->OnAbilityInputPressed(InputTag) 会触发激活这个技能。
	 */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories = "InputTag"))
	FGameplayTag InputTag;

	//该结构体要授予的技能类。
	//使用 TSubclassOf 让你在蓝图里直接选择一个 UWarriorHeroGameplayAbility 派生类
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorHeroGameplayAbility> AbilityToGrant;

	//用来检查这个配置是否有效
	bool IsValid() const;
};

//武器数据配置
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY();

	//WeaponAnimLayerToLink
	//指定当该武器被装备时，要链接的动画 Layer。
	//Layer 用于覆盖或扩展主动画蓝图的某些动画逻辑。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	//WeaponInputMappingContext 该武器特有的输入映射上下文。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	//DefaultWeaponAbilities
	//该武器的默认技能列表，每个元素是一个 FWarriorHeroAbilitySet。
	//装备时将这些技能授予 ASC。
	//卸下时移除这些技能。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;

	//武器基础伤害
	//GAS 提供的一个非常实用的结构体，用来存储“可扩展的数值”。
	//可以通过 FScalableFloat::GetValueAtLevel() 获取一个和等级挂钩的最终数值。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;
};
