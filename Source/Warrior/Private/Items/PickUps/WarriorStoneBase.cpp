// Spirit Revenge All Rights Reserved


#include "Items/PickUps/WarriorStoneBase.h"

#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	//防御性检查语句。
	//在开发或调试时确保 StoneGameplayEffectClass 不为空。
	//如果你忘记在蓝图里给石头设置 GameplayEffect 类，程序会在这里断言（崩溃提示）
	check(StoneGameplayEffectClass);

	//这里从类引用中获取 GameplayEffect 的 Class Default Object (CDO)。
	//TSubclassOf<UGameplayEffect> 保存的是类类型，不是实例；
	//GetDefaultObject<>() 获取该类的默认对象；
	//GAS 里通常不会手动 NewObject 效果实例，而是传入 CDO，让 ApplyGameplayEffectToSelf() 内部自己生成新的 ActiveEffect。
	UGameplayEffect* EffectCDO = StoneGameplayEffectClass -> GetDefaultObject<UGameplayEffect>();

	//EffectCDO：要应用的效果模板；
	//ApplyLevel：应用的等级（通常取决于能力等级或石头品质）；
	//MakeEffectContext()：生成一个 FGameplayEffectContextHandle，包含：
	//	Source（施加者，一般是自己或拾取物）
	//	Instigator（触发者）
	//	Optional hit info（如果是从命中事件触发）
	AbilitySystemComponent -> ApplyGameplayEffectToSelf(EffectCDO, ApplyLevel, AbilitySystemComponent -> MakeEffectContext());

	//调用蓝图实现的事件，用于视觉或音效反馈。
	BP_OnStoneConsumed();
}

void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                            const FHitResult& SweepResult)
{
	//判断进入触发范围的是否是“玩家角色”。
	//只有主角（Hero）才有拾取逻辑，AI 或其他对象不触发。
	if (AWarriorHeroCharacter* OverlappedHeroCharacter = Cast<AWarriorHeroCharacter>(OtherActor))
	{
		//主动让玩家的 AbilitySystemComponent 尝试激活“拾取石头”能力。
		OverlappedHeroCharacter -> GetWarriorAbilitySystemComponent() -> TryActivateAbilityByTag(WarriorGameplayTags::Player_Ability_PickUp_Stones);
	}
}
