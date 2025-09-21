// Spirit Revenge All Rights Reserved


#include "AbilitySystem/GEExecCacl/GEExecCalc_DamageTaken.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorGameplayTags.h"
#include "WarriorDebugHelper.h"

/**
 * 属性捕获结构体 FWarriorDamageCapture
 */
struct FWarriorDamageCapture
{
	//DECLARE_ATTRIBUTE_CAPTUREDEF → 声明属性捕获定义

	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	
	FWarriorDamageCapture()
	{
		//DEFINE_ATTRIBUTE_CAPTUREDEF → 定义属性捕获规则

		//ource → 从技能施放者（攻击者）读取
		//Target → 从被攻击者读取
		//false → 不捕获快照（每次计算实时读取当前值）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower,Source,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower,Target,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken,Target,false)
	}
};

/**
 * 静态单例 GetFWarriorDamageCapture
 * @return 属性捕获结构体
 */
static const FWarriorDamageCapture& GetFWarriorDamageCapture()
{
	//用 静态单例 存储属性捕获定义，避免每次实例化对象都重复创建
	//优化：只生成一次属性捕获对象
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	/* Slow way of doing capture */
	// FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
	// 	UWarriorAttributeSet::StaticClass(),
	// 	GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet,AttackPower)
	// );
	//
	// FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
	// 	AttackPowerProperty,
	// 	EGameplayEffectAttributeCaptureSource::Source,
	// 	false
	// );
	//
	// RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);

	//RelevantAttributesToCapture → GameplayEffect 系统会自动捕获这些属性
	RelevantAttributesToCapture.Add(GetFWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetFWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetFWarriorDamageCapture().DamageTakenDef);
	
}

/**
 * 
 * @param ExecutionParams 当前 GameplayEffect 的上下文（攻击者、目标、SetByCaller 数据等）
 * @param OutExecutionOutput 输出计算结果，最终修改目标属性
 */
void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                     FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//ExecutionParams 是传入的自定义执行参数，包含目标、源对象和 GameplayEffect 数据
	//GetOwningSpec() 返回 当前 GameplayEffect 的完整规格（Spec）
	//GameplayEffectSpec 中包含：
	//	属性捕获数据（Captured Attributes）
	//	SetByCaller 数据（外部传入的数值，如 BaseDamage）
	//	来源和目标标签（GameplayTags，用于判断免疫、抗性、Buff/Debuff）
	//	上下文对象（Ability, Instigator, Causer 等）
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// EffectSpec.GetContext().GetSourceObject();
	// EffectSpec.GetContext().GetAbility();
	// EffectSpec.GetContext().GetInstigator();
	// EffectSpec.GetContext().GetEffectCauser();

	//初始化 Evaluator 参数
	FAggregatorEvaluateParameters EvaluateParams;
	//SourceTags：攻击者或施放者的标签
	EvaluateParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	//TargetTags：目标或被攻击者的标签
	EvaluateParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.f;
	
	//从 Source 捕获的属性中获取攻击力
	//EvaluateParams 包含攻击者/目标的 GameplayTag 信息，可用于条件计算
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetFWarriorDamageCapture().AttackPowerDef,EvaluateParams, SourceAttackPower);
	// Debug::Print(TEXT("SourceAttackPower"), SourceAttackPower);
	
	float BaseDamage = 0.f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;
	//SetByCallerTagMagnitudes → GameplayEffect 里通过 SetByCaller 传入的数据
	//作用：
	//	BaseDamage → 技能或武器基础伤害
	//	UsedLightAttackComboCount / UsedHeavyAttackComboCount → 连击次数，用于计算伤害加成
	for (const TPair<FGameplayTag,float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
			// Debug::Print(TEXT("BaseDamage"), BaseDamage);
		}

		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount = TagMagnitude.Value;
			// Debug::Print(TEXT("UsedLightAttackComboCount"), UsedLightAttackComboCount);
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount = TagMagnitude.Value;
			// Debug::Print(TEXT("UsedHeavyAttackComboCount"), UsedHeavyAttackComboCount);
		}
	}
	
	float TargetDefensePower = 0.f;
	//从目标的 DefensePower 属性获取防御力，用于伤害减免计算
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetFWarriorDamageCapture().DefensePowerDef,EvaluateParams,TargetDefensePower);
	// Debug::Print(TEXT("TargetDefensePower"), TargetDefensePower);

	//轻攻击加成：每次连击增加 5%
	if (UsedLightAttackComboCount != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightAttackComboCount - 1) * 0.05f + 1.f;

		//将基础伤害 BaseDamage 按连击次数缩放
		BaseDamage *= DamageIncreasePercentLight;
		// Debug::Print(TEXT("ScaledBaseDamageLight"), BaseDamage);
	}

	//重攻击加成：每次连击增加 15%
	
	if (UsedHeavyAttackComboCount != 0)
	{
		const float DamageIncreasePercentHeavy = UsedHeavyAttackComboCount * 0.15f + 1.f;

		//将基础伤害 BaseDamage 按连击次数缩放
		BaseDamage *= DamageIncreasePercentHeavy;
		// Debug::Print(TEXT("ScaledBaseDamageHeavy"), BaseDamage);
	}

	//最终伤害计算
	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	//Debug::Print(TEXT("FinalDamageDone"), FinalDamageDone);

	//判断伤害大于零
	if (FinalDamageDone > 0.f)
	{
		//将最终伤害应用到目标的 DamageTaken 属性
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetFWarriorDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,//EGameplayModOp::Override → 覆盖当前值，GameplayEffect 会将其应用到目标血量
				FinalDamageDone
			)
		);
	}
}
