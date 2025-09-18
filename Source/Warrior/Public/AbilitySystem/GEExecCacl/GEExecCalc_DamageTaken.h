// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExecCalc_DamageTaken.generated.h"

/**
 * UGameplayEffectExecutionCalculation
 *	是 Unreal Engine GameplayAbilitySystem 的核心类之一
 *	用于 自定义属性计算逻辑（如伤害、治疗、增益减益等）
 *	与普通 GameplayEffect 不同，它可以访问多个属性进行复杂计算
 * 该类的作用：
 *	当一个伤害 GameplayEffect 被应用到目标时，执行 自定义计算逻辑
 *	计算结果写入 FGameplayEffectCustomExecutionOutput
 */
UCLASS()
class WARRIOR_API  UGEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	//构造函数
	UGEExecCalc_DamageTaken();

	//当 GameplayEffect 被应用到目标时，执行该函数
	//ExecutionParams
	//	包含目标和源对象的 AbilitySystemComponent
	//	包含所有 GameplayEffect 的属性数据（如攻击力、目标防御、SetByCaller 数据）
	//OutExecutionOutput
	//	用于输出最终计算结果
	//	可以修改目标属性（如减血 DamageDone）
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
