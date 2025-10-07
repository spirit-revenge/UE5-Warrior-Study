// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStones.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/UI/HeroUIComponent.h"
#include "Items/PickUps/WarriorStoneBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_PickUpStones::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	//GetUIComponentFromActorInfo()
	//一个便捷函数（在父类或工具类中定义），通过 ActorInfo 获取当前拥有者（角色）身上的 UI 组件。
	//这个组件里有一个委托 OnStoneInteracted，负责 UI 层显示“正在拾取”状态。
	//OnStoneInteracted.Broadcast(true)
	//向 UI 广播一个事件：“开始拾取石头”。
	//Blueprint 或 UI Widget 可以绑定到这个事件，比如显示一个“拾取动画”或“高亮提示”。
	GetUIComponentFromActorInfo() -> OnStoneInteracted.Broadcast(true);

	//调用父类的 ActivateAbility，完成 GAS 内部的注册与初始化流程。
	//必须调用，否则 Ability 不会被正确标记为“已激活状态”。
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUpStones::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	//OnStoneInteracted.Broadcast(false)
	//通知 UI “拾取操作结束”，可以关闭拾取动画或提示。
	GetUIComponentFromActorInfo() -> OnStoneInteracted.Broadcast(false);

	//告诉 GAS：此能力结束（无论是正常结束或被取消）。
	//内部会处理 cooldown、tag 清理、输入解锁等。
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_PickUpStones::CollectStones()
{
	//每次检测前清空旧的收集列表，避免重复。
	CollectedStones.Empty();

	//TraceHits 存储 BoxTrace 的命中结果。
	TArray<FHitResult> TraceHits;
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(), //指定执行跟踪的角色
		GetHeroCharacterFromActorInfo() -> GetActorLocation(), //从角色位置往**下方（负 UpVector）**延伸 BoxTraceDistance 进行检测。
		GetHeroCharacterFromActorInfo() -> GetActorLocation() + -GetHeroCharacterFromActorInfo() -> GetActorUpVector() * BoxTraceDistance,
		TraceBoxSize / 2.f, //检测盒体大小。除以 2 是因为 BoxTrace 以“半长宽高”为参数。
		(-GetHeroCharacterFromActorInfo() -> GetActorUpVector()).ToOrientationRotator(), //盒体方向设为向下。
		StoneTraceChannel, //只检测特定 ObjectType 的对象（例如石头）。
		false, //设置为 true 以测试复杂碰撞，设置为 false 以测试简化碰撞。
		TArray<AActor*>(), //忽略的actor
		bDrawDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, //是否绘制调试框，可在编辑器可视化检测范围。
		TraceHits, //输出命中的所有结果。
		true //表示返回物理体信息（有用的碰撞信息）。
	);

	//遍历所有检测到的对象。
	for (const FHitResult& TraceHit : TraceHits)
	{
		//尝试转换为 AWarriorStoneBase（自定义石头类）。
		if (AWarriorStoneBase* FoundStone = Cast<AWarriorStoneBase>(TraceHit.GetActor()))
		{
			//如果成功，加入 CollectedStones 数组。
			//AddUnique 确保不会重复添加同一个石头。
			CollectedStones.AddUnique(FoundStone);
		}
	}

	//没有命中任何石头时，直接取消这个 Ability。
	//CancelAbility() 会触发 EndAbility() 并清理状态。
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

void UHeroGameplayAbility_PickUpStones::ConsumeStones()
{
	//如果没有任何石头缓存，则中断技能。
	//这里防止逻辑错误（比如调用时未先检测）。
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	for (AWarriorStoneBase* CollectedStone : CollectedStones)
	{
		if (CollectedStone)
		{
			//对每个收集到的石头调用 Consume()。
			CollectedStone -> Consume(GetWarriorAbilitySystemComponentFromActorInfo(), GetAbilityLevel());
		}
	}
}
