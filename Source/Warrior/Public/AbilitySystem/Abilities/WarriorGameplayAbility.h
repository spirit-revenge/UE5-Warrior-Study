// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;

//用来描述技能系统中能力的激活策略
UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy : uint8
{
	OnTriggered, // 当被“触发”时才激活（比如玩家按下技能键）
	OnGiven  // 一旦“获得”就立刻激活（不需要玩家手动触发）
};
/**
 * 公共的技能类，获取所有actor的公共技能
 */
UCLASS()
class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayAbility Interface.

	//当这个 Ability 被“授予”(Granted) 给角色时会调用这个函数。
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	//当技能结束（或者被取消）时会调用。 通常在这里做“收尾”工作，比如重置状态、恢复冷却、播放动画结束等。
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	//~ End UGameplayAbility Interface 

	//让它在蓝图编辑器里可以看到和编辑（只能在默认值里改）
	UPROPERTY(EditDefaultsOnly,Category="WarriorAbility")
	//可以选 OnTriggered 或 OnGiven 来决定这个技能的激活方式，默认值为OnTriggered
	EWarriorAbilityActivationPolicy AbilityActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;

	//从 ActorInfo 里获取角色的战斗组件 UPawnCombatComponent，方便在蓝图或 C++ 里调用
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	//拿到自定义的 UWarriorAbilitySystemComponent
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability" ,meta=(DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle,EWarriorSuccessType& OutSuccessType);
};
