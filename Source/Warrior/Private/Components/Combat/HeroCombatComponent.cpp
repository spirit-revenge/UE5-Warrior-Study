// Spirit Revenge All Rights Reserved


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/WarriorHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGameplayTags.h"

#include "WarriorDebugHelper.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	//父类 UPawnCombatComponent 提供的函数，返回一个 AWarriorWeaponBase*，根据传入的 FGameplayTag 查找角色当前携带的武器
	//安全类型转换，转换成子类AWarriorHeroWeapon
	 return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	//调用父类 UPawnCombatComponent 的 GetCharacterCurrentEquippedWeapon() 获取当前装备武器
	//然后通过 Cast 转为 AWarriorHeroWeapon* 类型
	return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
	//从当前装备的武器中获取 HeroWeaponData.WeaponBaseDamage
	//FScalableFloat::GetValueAtLevel(InLevel) → 根据等级计算伤害值
	return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	//Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT(" hit ")+ HitActor->GetActorNameOrLabel(), FColor::Green);

	//确保同一次攻击不会重复伤害同一目标
	//判断是否存在
	if (OverLappedActors.Contains(HitActor)) return;

	//不存在则添加碰撞对象
	OverLappedActors.AddUnique(HitActor);

	//Instigator → 谁发起了攻击
	//Target → 被击中的目标
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	//发送 GameplayEvent
	//通过 AbilitySystem 向角色发送 MeleeHit 事件
	//能力系统（GameplayAbilities）监听此事件触发伤害技能、特效等逻辑
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Shared_Event_MeleeHit,
		Data
	);
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	//输出碰撞的对象
	Debug::Print(GetOwningPawn()->GetActorNameOrLabel() + TEXT("'s weapon pulled from  ")+ InteractedActor->GetActorNameOrLabel(), FColor::Red);
}
