// Spirit Revenge All Rights Reserved

#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/PawnUIComponent.h"
#include "Components/UI/HeroUIComponent.h"

#include "WarriorDebugHelper.h"

UWarriorAttributeSet::UWarriorAttributeSet()
{
	//初始化，防止空指针
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
}

void UWarriorAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	//判断缓存是否存在
	if (!CachedPawnUIInterface.IsValid())
	{
		//缓存接口，避免每次都 Cast
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());

		//与上面的代码作用一样
		//CachedPawnUIInterface = Cast<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	//确保 AvatarActor 实现了 IPawnUIInterface，否则报错
	checkf(CachedPawnUIInterface.IsValid(),TEXT("%s didn't implement IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	//获取 Pawn UI 组件
	//调用接口获取 通用 UI 组件（血量、怒气、装备等）
	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();

	//确保组件存在，否则报错
	checkf(PawnUIComponent, TEXT("Couldn't extrac a PawnUIComponemt from %s"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	//如果属性为CurrentHealth
	if ( Data.EvaluatedData.Attribute == GetCurrentHealthAttribute() )
	{
		//取最新血量并 Clamp（防止溢出或小于0）
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(),0.f,GetMaxHealth());
		
		//调用 SetCurrentHealth 更新 AttributeSet
		SetCurrentHealth(NewCurrentHealth);

		//广播 OnCurrentHealthChanged 给 UI，让血条刷新
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	//如果属性为CurrentRage
	if ( Data.EvaluatedData.Attribute == GetCurrentRageAttribute() )
	{
		////取最新怒气并 Clamp（防止溢出或小于0）
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(),0.f,GetMaxRage());

		//更新怒气 Attribute。
		SetCurrentRage(NewCurrentRage);

		//判断UI component是否存在
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface -> GetHeroUIComponent())
		{
			//广播到 UI（只适用于 HeroUIComponent，敌人没有怒气）
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
		}
	}

	//如果属性为DamageTaken
	//伤害处理
	if ( Data.EvaluatedData.Attribute == GetDamageTakenAttribute() )
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();

		//计算扣血后的血量，Clamp 范围 0~MaxHealth。
		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone,0.f,GetMaxHealth());

		//调用 SetCurrentHealth 更新 AttributeSet。
		SetCurrentHealth(NewCurrentHealth);

		//打印调试信息
		const FString DebugString = FString::Printf(
			TEXT("Old Health: %f, Damage Done: %f, NewCurrentHealth: %f"),
			OldHealth,DamageDone,NewCurrentHealth
		);

		Debug::Print(DebugString,FColor::Green);

		//广播血量变化事件到 UI。
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());

		//如果血量为 0，添加死亡标签
		if (NewCurrentHealth == 0.f )
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGameplayTags::Shared_Status_Dead);

		}
	}
}