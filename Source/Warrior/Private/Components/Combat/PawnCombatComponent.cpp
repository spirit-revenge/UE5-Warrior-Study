// Spirit Revenge All Rights Reserved


#include "Components/Combat/PawnCombatComponent.h"
#include "WarriorDebugHelper.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

//把生成的武器存到 CharacterCarriedWeaponMap
void UPawnCombatComponent::RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister,
	AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	//检查，确保不会重复注册相同 Tag 的武器，否则直接报错（防止逻辑错误）
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),TEXT("A attack named %s has already been addedas carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);
	
	//确保传进来的武器不是 nullptr
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister,InWeaponToRegister);

	//BindUObject 用于把委托绑定到 UObject 类的成员函数
	//this → 当前战斗组件实例
	//&ThisClass::OnHitTargetActor → 当委托触发时调用的函数
	//绑定后，武器事件发生时会自动调用战斗组件的函数。
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this,&ThisClass::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this,&ThisClass::OnWeaponPulledFromTargetActor);
	
	// 如果要求注册的同时装备，就更新当前装备武器的 Tag
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}

	// 打印调试信息，方便在输出日志里看到是哪把武器被注册了
	// const FString WeaponString = FString::Printf(TEXT("A weapon named: %s has been registered using  hte tag %s"),*InWeaponToRegister->GetName(),*InWeaponTagToRegister.ToString());
	// Debug::Print(WeaponString);
}

//通过 GameplayTag 查询角色是否持有指定武器。如果有，返回那把武器的指针。如果没有，返回 nullptr
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	//确保已经有Tag注册的该武器
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		// Find 返回 value 的指针，如果找到了就解引用返回
		if(AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}

	return nullptr;
}

//检查当前是否有装备武器（Tag 有效性）
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	//只处理当前装备的武器碰撞
	if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
	{
		//	GetCharacterCurrentEquippedWeapon() → 获取角色当前装备的武器
		AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();

		//check(WeaponToToggle) → 确保武器不为空，避免空指针崩溃
		check(WeaponToToggle);

		if (bShouldEnable)
		{
			//SetCollisionEnabled(ECollisionEnabled::QueryOnly)
			//启用碰撞查询（Overlap），但不产生物理响应
			//用于检测武器与目标的接触
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			//Debug::Print(WeaponToToggle->GetName()+TEXT(" Collision Enabled"),FColor::Green);
		}
		else
		{
			//SetCollisionEnabled(ECollisionEnabled::NoCollision) → 禁用碰撞
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//OverLappedActors.Empty() → 清空已记录的碰撞目标，避免下一次攻击重复命中
			OverLappedActors.Empty();
			//Debug::Print(WeaponToToggle->GetName()+TEXT(" Collision Disabled"),FColor::Red);
		}
	}
	//TODO：Handle body collision boxes
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}
