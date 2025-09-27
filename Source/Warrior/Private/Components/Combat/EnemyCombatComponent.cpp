// Spirit Revenge All Rights Reserved


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"

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

	//TODO: implement block check
	//如果玩家正在阻挡 (bIsPlayerBlocking == true) 且敌人的攻击不是无法被阻挡 (bIsMyAttackUnblockable == false)
	//则进一步判断阻挡是否成功 (bIsValidBlock)
	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = false;
	const bool bIsMyAttackUnblockable = false;

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		//TODO: check if the block is valid 
	}

	//创建 FGameplayEventData 用于 Gameplay Ability System 事件传递。
	//Instigator 和 Target 信息在 AbilitySystem 中用于伤害计算、特效播放等。
	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	//如果攻击被阻挡 (bIsValidBlock == true)
	if (bIsValidBlock)
	{
		//TODO: handle successful block
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
