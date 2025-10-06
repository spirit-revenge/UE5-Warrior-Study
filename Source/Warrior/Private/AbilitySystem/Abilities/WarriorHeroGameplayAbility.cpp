// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorGameplayTags.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	//如果缓存里的hero对象不存在，则获取当前AvatarActor对象，转换成Hero对象并存在缓存里
	if (!CashedWarriorHeroCharacter.IsValid())
	{
		CashedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	//判断缓存里是否有有效的角色对象，返回它，否则返回null
	return CashedWarriorHeroCharacter.IsValid()? CashedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	//检查缓存是否有效，如果无效 → 从 ActorInfo 获取 PlayerController，并强制转换成 AWarriorHeroController
	if (!CashedWarriorHeroController.IsValid())
	{
		CashedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}
	//判断缓存里是否有有效的角色控制器，返回它，否则返回null
	return CashedWarriorHeroController.IsValid()? CashedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	//得到当前hero对象的战斗组件
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	//确保传入的 EffectClass 有效。
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

	//动态传入伤害值
	//用 SetByCaller 方式，把基础伤害写入 Spec
	//在 GameplayEffect 的 Modifiers 里，你应该有一个 SetByCaller 类型的 Modifier，引用 Shared_SetByCaller_BaseDamage
	//这样就可以在运行时灵活控制伤害值，而不用为每种武器、技能配置不同的 GameplayEffect。
	EffectSpecHandle.Data -> SetSetByCallerMagnitude(
		WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
		InWeaponBaseDamage
	);

	if (InCurrentAttackTypeTag.IsValid())
	{
		//额外传入连击数，这里用攻击类型的 Tag 作为 Key，把连击数写入 Spec
		EffectSpecHandle.Data -> SetSetByCallerMagnitude(InCurrentAttackTypeTag,InUsedComboCount);
	}

	//返回 SpecHandle，供调用方应用。
	return EffectSpecHandle;
}

bool UWarriorHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime,
	float& RemainingCooldownTime)
{
	//检查传入的 Tag 是否有效（无效则在调试中触发断言）
	check(InCooldownTag.IsValid());

	//FGameplayEffectQuery 是一个过滤条件，用于查询当前激活的 GameplayEffect。
	//这里用 MakeQuery_MatchAnyOwningTags()，表示查找 拥有任意匹配该冷却标签 的效果。
	//GetSingleTagContainer() 把单个 FGameplayTag 转成 FGameplayTagContainer（GAS 的容器结构）。
	FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());

	//GetActiveEffectsTimeRemainingAndDuration() 返回一个数组：
	//每个元素是 TPair<RemainingTime, TotalDuration>。
	//也就是说： .Key → 剩余时间 .Value → 整体持续时间（冷却总时长）
	TArray< TPair< float, float > > TimeRemainingAndDuration =  GetAbilitySystemComponentFromActorInfo() -> GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);

	//如果有至少一个冷却效果匹配到：
	//取第一个（默认假设一个冷却 tag 只会对应一个冷却效果）。
	//赋值给输出参数。
	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
		TotalCooldownTime = TimeRemainingAndDuration[0].Value;
	}

	//如果剩余时间大于 0，说明冷却还没结束 → 返回 true。
	//否则表示冷却已完成或未激活 → 返回 false。
	return RemainingCooldownTime > 0.f; 
}
