// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"

#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorEnemyCharacter.h"

AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	//判断是否存在
	if (!CachedWarriorEnemyCharacter.IsValid())
	{
		//第一次调用时用 Cast<AWarriorEnemyCharacter> 转换 AvatarActor，并存储在 CachedWarriorEnemyCharacter。
		CachedWarriorEnemyCharacter = Cast<AWarriorEnemyCharacter>(CurrentActorInfo -> AvatarActor);
	}
	//用三元运算符保证即便缓存失效也返回 nullptr，而不是直接解引用
	return CachedWarriorEnemyCharacter.IsValid() ? CachedWarriorEnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	//从actor获取enemy character 并返回 EnemyCombatComponent
	return GetEnemyCharacterFromActorInfo() -> GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UWarriorEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
	check(EffectClass);

	//创建上下文 (EffectContext)
	//用于记录 GameplayEffect 的来源 (Instigator, Causer, SourceObject)。
	//后续在应用 GameplayEffect 时，可以通过 Context 查询是谁造成的伤害，甚至可以在受击者的 HUD 上显示“谁打了我”。
	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo() -> MakeEffectContext();
	//记录是哪一个 Ability 生成了这个 Effect
	ContextHandle.SetAbility(this);
	//AddSourceObject：通常记录技能来源，可以是武器、技能本身等
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	//记录 Instigator 和 Causer，这里你都用了 GetAvatarActorFromActorInfo()，也就是角色自己（合理，普通攻击确实是角色自己发动）
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());

	//生成 SpecHandle
	//使用当前 ASC 生成 OutgoingSpec
	//Level 使用当前 Ability 的等级
	//传入刚刚创建好的 Context
	//MakeOutgoingSpec() 会返回一个 FGameplayEffectSpecHandle，你后续可以用这个句柄调用 ApplyGameplayEffectSpecToTarget()
	FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo() -> MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);

	//把伤害值写入了 SetByCaller，对应之前 UGEExecCalc_DamageTaken 里的读取逻辑。
	//这样就能动态支持不同武器/技能伤害。
	EffectSpecHandle.Data -> SetSetByCallerMagnitude(
		WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
		InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
	);

	return EffectSpecHandle;
}
