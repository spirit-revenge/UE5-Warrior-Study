// Spirit Revenge All Rights Reserved


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"

#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
                                                                               FGameplayTag EventTag, TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
                                                                               const FVector& SpawnOrigin, float RandomSpawnRadius)
{
	//GAS 提供的工厂方法，创建并把任务挂到 OwningAbility 上，返回一个实例 Node
	UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);

	//将传入参数缓存到任务实例的成员以备后续使用
	Node -> CachedEventTag = EventTag;
	Node -> CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node -> CachedNumToSpawn = NumToSpawn;
	Node -> CachedSpawnOrigin = SpawnOrigin;
	Node -> CachedRandomSpawnRadius = RandomSpawnRadius;
	return Node;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	//从 AbilitySystemComponent 的 GenericGameplayEventCallbacks map 中根据 CachedEventTag 找到对应的 multicast delegate
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent -> GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	//将 OnGameplayEventReceived 以 UObject 方式绑定到该 multicast delegate 上，并保存 DelegateHandle 以便以后移除绑定
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	//再次通过 FindOrAdd 获取同一个 delegate 引用以便移除
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent -> GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	//使用 Delegate.Remove(DelegateHandle) 将之前 AddUObject 添加的绑定移除。
	Delegate.Remove(DelegateHandle);
	
	//然后调用 Super::OnDestroy 做父类清理
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	//然后调用 Super::OnDestroy 做父类清理
	/*
	 * ensure(...)：若条件为 false，会输出错误日志并在调试环境打断点，
	 * 但在发布版本通常会继续执行（会返回 false）。
	 * 你用 if (ensure(...)) 把逻辑放在 true 分支，这样当软类为空时会走 else 分支
	 */
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		/*
		 * 调用 UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(...) 发起异步加载：
		 * ToSoftObjectPath() 把 TSoftClassPtr 转为路径字符串。
		 * FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded) 创建回调，加载完成后会调用 OnEnemyClassLoaded()
		 */
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnEnemyClassLoaded)
		);
	}
	else
	{
		//如果软引用为空（ensure 失败），则通知蓝图 DidNotSpawn 并结束任务。
		//使用 ShouldBroadcastAbilityTaskDelegates() 很好，保证只有在任务仍有效且允许广播时才广播 delegate。
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}

		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemies::OnEnemyClassLoaded()
{
	//CachedSoftEnemyClassToSpawn.Get()：在异步加载完成后，返回加载得到的 UClass*（若加载失败则可能为 nullptr）
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	//GetWorld()：获取当前 world，上下文用于 spawn actor
	UWorld* World = GetWorld();

	//两者都可能为 nullptr —— 接下来有判断
	if (!LoadedClass || !World)
	{
		//若类加载失败或 world 不可用，则广播 DidNotSpawn 并结束任务
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}

		EndTask();
		return;
	}

	//用数组收集成功生成的敌人和属性
	TArray<AWarriorEnemyCharacter*> SpawnedEnemies;
	FActorSpawnParameters SpawnParam;
	//SpawnCollisionHandlingOverride 设为 AdjustIfPossibleButAlwaysSpawn：表示尝试避免碰撞，但如果无法避免仍会 spawn
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//循环 CachedNumToSpawn 次尝试生成。
	for (int32 i = 0; i < CachedNumToSpawn; ++i)
	{
		//使用 UNavigationSystemV1::K2_GetRandomReachablePointInRadius 获取半径内可到达点并写入 RandomLocation
		FVector RandomLocation;
		/*
		 * 该函数有返回值 bool（是否找到点），但你的实现没有检查返回值。如果函数返回 false，RandomLocation 可能未被初始化或为零。
		 * K2_ 前缀的函数一般用于蓝图节点，在 C++ 中直接调用建议使用 UNavigationSystemV1::GetRandomPointInNavigableRadius 或检查返回值。
		 * this 作为 WorldContextObject 是可以的（UAbilityTask 是 UObject）；但如果 this 的 world context 与 GetWorld() 不一致也可能出问题（一般没问题）
		 */
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);

		//把随机点抬高 150 单位
		//这通常是为了避免角色出生卡在地面中，或让角色先出现在空中然后掉落/着陆
		RandomLocation += FVector(0.f, 0.f, 150.f);

		//计算生成时的朝向：取 Ability 的 Avatar（通常是施法者）朝向作为敌人 spawn 的朝向
		const FRotator SpawnFacingRotation = AbilitySystemComponent -> GetAvatarActor() -> GetActorForwardVector().ToOrientationRotator();

		//在 world 中 spawn actor，
		AWarriorEnemyCharacter* SpawnedEnemy = World -> SpawnActor<AWarriorEnemyCharacter>(LoadedClass, RandomLocation, SpawnFacingRotation, SpawnParam);

		//若 spawn 成功则加入 SpawnedEnemies 数组。
		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}

	//在广播之前检查 ShouldBroadcastAbilityTaskDelegates()
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//若有生成到敌人，广播 OnSpawnFinished 并传回数组
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			//否则广播 DidNotSpawn
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
	}

	//任务在处理完成后调用 EndTask()，此时会触发 OnDestroy() 等清理逻辑并最终释放任务实例
	EndTask();
}
