// Spirit Revenge All Rights Reserved


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/BoxComponent.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	//作用：记录已经命中的目标，避免同一次攻击多次触发伤害或事件。
	//OverLappedActors 是父类 UPawnCombatComponent 的成员，保存当前攻击已经触碰的对象列表。
	if (OverLappedActors.Contains(HitActor))
	{
		return;
	}

	//AddUnique 保证同一个目标只添加一次。
	OverLappedActors.AddUnique(HitActor);
	
	//如果玩家正在阻挡 (bIsPlayerBlocking == true) 且敌人的攻击不是无法被阻挡 (bIsMyAttackUnblockable == false)
	//则进一步判断阻挡是否成功 (bIsValidBlock)
	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor, WarriorGameplayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(),WarriorGameplayTags::Enemy_Status_Unblockable);

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	//创建 FGameplayEventData 用于 Gameplay Ability System 事件传递。
	//Instigator 和 Target 信息在 AbilitySystem 中用于伤害计算、特效播放等。
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	//如果攻击被阻挡 (bIsValidBlock == true)
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else
	{
		//如果攻击未被阻挡
		//使用 SendGameplayEventToActor 给目标发送一个 “MeleeHit” 事件
		//目标的 AbilitySystemComponent 会收到事件，并触发伤害计算、击退或其他效果
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarriorGameplayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	//GetOwningPawn<T>() 是从战斗组件所属的 Pawn（即敌人角色）里拿到一个特定类型的指针
	AWarriorEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<AWarriorEnemyCharacter>();

	//如果拿不到角色实例（比如组件没有正确附加），就会中断运行并提示错误
	check(OwningEnemyCharacter);

	//从角色身上获取左右手的碰撞组件（之前在构造函数中创建的）。
	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter -> GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter -> GetRightHandCollisionBox();

	//再次使用 check() 确保它们都存在
	check(LeftHandCollisionBox && RightHandCollisionBox);

	//通过 EToggleDamageType（枚举类型）区分是哪只手要启用碰撞
	//当 bShouldEnable == true 时，开启 QueryOnly：
	//→ 意味着组件能检测碰撞（Overlap / Hit），但不会物理阻挡。
	//当 bShouldEnable == false 时，设置为 NoCollision：
	//→ 禁用碰撞检测。
	switch (ToggleDamageType)
	{
		case EToggleDamageType::LeftHand:
			LeftHandCollisionBox -> SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision);
			break;
		case EToggleDamageType::RightHand:
			RightHandCollisionBox -> SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::Type::QueryOnly : ECollisionEnabled::Type::NoCollision);
			break;
		default:
			break;
	}

	//重置已命中的 Actor 缓存
	if (!bShouldEnable)
	{
		OverLappedActors.Empty();
	}
}
