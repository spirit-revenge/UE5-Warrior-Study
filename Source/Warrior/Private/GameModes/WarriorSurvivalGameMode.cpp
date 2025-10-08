// Spirit Revenge All Rights Reserved


#include "GameModes/WarriorSurvivalGameMode.h"

#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

void AWarriorSurvivalGameMode::BeginPlay()
{
	//调用父类的 BeginPlay 实现，确保引擎与父类逻辑（如内置初始化）能先运行。
	Super::BeginPlay();

	//断言（带格式信息）。如果 EnemyWaveSpawnerDataTable 为 nullptr，运行时会触发断言并在编辑器/日志中输出提供的错误消息。
	//用于在开发阶段快速发现未配置的必需资源。checkf 在打包的构建中通常仍然会触发崩溃（视构建配置），所以它是强断言而非仅日志。
	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid data table in survival game mode blueprint "));

	//设置当前生存模式状态为 WaitSpawnNewWave（等待生成新波次）。
	//会调用实现的状态切换函数（见 SetCurrentSurvivalGameModeState），该函数会更新状态并广播事件
	SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);

	//读取 DataTable 的所有行名并计数，结果保存到 TotalWavesToSpawn。
	//GetRowNames() 返回 TArray<FName>，.Num() 返回元素数量。意味着 DataTable 中每一行代表一波，行数即总波数
	TotalWavesToSpawn = EnemyWaveSpawnerDataTable -> GetRowNames().Num();

	//预加载下一波（当前波）所需的敌人类（见该函数），通常用来异步加载软引用资源以减小卡顿
	PreLoadNextWaveEnemies();
}

//Tick 覆写定义；DeltaTime 是从上一帧到当前帧的时间（秒）。
//GameMode 的 Tick 只有在你在构造中启用 PrimaryActorTick 时才会被调用（你之前启用了）
void AWarriorSurvivalGameMode::Tick(float DeltaTime)
{
	//调用父类 Tick，保持父类行为一致。
	Super::Tick(DeltaTime);

	//如果当前状态是“等待生成新波次”。
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaitSpawnNewWave)
	{
		//累加计时器；用于实现等待时间（基于帧时间）。
		TimePassedSinceStart += DeltaTime;

		//当累计时间超过或等于等待时间阈值时，触发下列逻辑。
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			//重置计时器，为下一阶段计时清零。
			TimePassedSinceStart = 0.f;

			//切换状态到“正在生成新波次”。
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::SpawningNewWave);
		}
	}

	//检查是否为“生成新波次”状态。
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::SpawningNewWave)
	{
		//同样累加计时器，控制生成的延迟（例如先播放提示动画、音效后再实际生成）。
		TimePassedSinceStart += DeltaTime;

		//当延迟时间到达时执行生成逻辑。
		if (TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			//调用 TrySpawnWaveEnemies() 生成一批敌人，返回实际生成的数量，
			//并将其加到 CurrentSpawnedEnemiesCounter（表示当前“仍存活/已生成并未销毁”的敌人计数）
			CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();

			//重置计时器。
			TimePassedSinceStart = 0.f;

			//将状态切换到 InProgress —— 也就是这一波已经生成并处于战斗中，系统将等待敌人全部被击杀或其它条件。
			SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::InProgress);
		}
	}

	//如果状态为“波次完成”。
	if (CurrentSurvivalGameModeState == EWarriorSurvivalGameModeState::WaveCompleted)
	{
		//累计等待完成波次后的过渡时间（比如播放胜利动画、给玩家短暂休息等）。
		TimePassedSinceStart += DeltaTime;

		//如果等待时间到了，进入下一波或结束。
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			//重置计时器。
			TimePassedSinceStart = 0.f;

			//将当前波次计数增加 —— 说明下一波次序号前移。
			CurrentWaveCount++;

			//检查是否已经完成所有配置的波次（参见 HasFinishedAllWaves() 的定义）。
			if (HasFinishedAllWaves())
			{
				//如果是最后一波已完成，则切换到 AllWavesDone 状态（可供 Blueprint 或 UI 监听，触发胜利等流程）
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::AllWavesDone);
			}
			else
			{
				//若仍有后续波次，切换回等待生成新波的状态，并预加载下一波敌人（PreLoadNextWaveEnemies()）
				SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AWarriorSurvivalGameMode::SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState)
{
	//将类成员 CurrentSurvivalGameModeState 更新为传入的状态。
	CurrentSurvivalGameModeState = InState;

	//通过 BlueprintAssignable 的多播委托广播状态变化事件。
	//任何在 Blueprint/C++ 中绑定了 OnSurvivalGameModeStateChanged 的监听器都会收到通知并可作响应（例如更新 HUD、播放音效等）
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	//如果 CurrentWaveCount 大于 TotalWavesToSpawn 则返回 true。
	//注意：
	//	这意味着当 CurrentWaveCount 从 1 开始计数时，完成判定在你把 CurrentWaveCount++ 后（Tick 中的实现）发生，
	//	当 CurrentWaveCount 超过最大行数才认为“全部完成”。这是初始化 CurrentWaveCount 时的约定（你在 header 中把它设为 1）。
	return CurrentWaveCount > TotalWavesToSpawn;
}

void AWarriorSurvivalGameMode::PreLoadNextWaveEnemies()
{
	//若所有波次已完成，则直接返回，不做预加载。
	if (HasFinishedAllWaves())
	{
		return;
	}

	//清空之前的预加载缓存，确保为新一波重新填充
	PreLoadedEnemyClassMap.Empty();

	//遍历当前波对应的 EnemyWaveSpawnerDefinitions 列表（从 DataTable 行取得）。
	//每个 SpawnerInfo 描述一种要生成的敌人类型和每次生成数量区间
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow() -> EnemyWaveSpawnerDefinitions)
	{
		//如果该条目的软类引用为空则跳过（安全检查）
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		//使用 UAssetManager 的 StreamableManager 异步请求加载资源。
		//RequestAsyncLoad 接受一个 FSoftObjectPath（这里从 TSoftClassPtr 转换）和一个回调 FStreamableDelegate。
		//异步加载意味着不会阻塞主线程，资源加载完成后会调用回调
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(), //将软类指针转换为 FSoftObjectPath 作为加载目标。
			//用 lambda 创建回调委托。
			//注意：
			//lambda 捕获了 SpawnerInfo（按值捕获）和 this（指向当前对象）。
			//	按值捕获会复制 SpawnerInfo 的内容进入回调，这在异步场景下能确保回调使用到当时的值（避免迭代变量被覆盖的问题）。
			//	但仍需注意：lambda 的生命周期可能超过 this，如果 GameMode 被销毁，
			//	回调运行时 this 可能无效 —— 通常 GameMode 存活到游戏结束，所以通常安全，但在特殊场景要注意。
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo,this]()
				{
					//在回调中通过 .Get() 同步获取已经加载的类指针（如果加载成功）。Get() 在资源已加载时返回 UClass*，否则 nullptr
					if (UClass* LoadedEnemyClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						//将软引用作为 key，实际 UClass* 作为 value 放入缓存映射表中。后续 TrySpawnWaveEnemies() 会从这里拿到 UClass* 直接做 SpawnActor
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedEnemyClass);
					}
				}
			)
		);
	}
}

///返回指向 DataTable 中当前波次对应行的指针。const 表示该函数不修改对象成员。
FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	//生成用于查找的行名：把字符串 "Wave" 与 CurrentWaveCount 的字符串形式拼接，比如 Wave1, Wave2 等，然后转换为 FName。
	//DataTable 的行名按这个命名规则存在
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));

	//调用 DataTable 的 FindRow<T> 模板函数查找对应行。
	//第二个参数是 ContextString（用于调试/日志），这里传了空字符串。FindRow 返回指向该行的数据，如果不存在则返回 nullptr
	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable -> FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, FString());

	//断言 FoundRow 非空，如果空则崩溃并输出错误信息（包含所请求的行名）。这在开发时能快速找出 DataTable 配置不正确的问题。
	checkf(FoundRow, TEXT("Could not find a valid row under  the name %s"), *RowName.ToString());

	//返回找到的行指针给调用者。
	return FoundRow;
}

//尝试在当前波次生成若干敌人，返回本次实际生成的敌人数（int32）。
int32 AWarriorSurvivalGameMode::TrySpawnWaveEnemies()
{
	//如果 TargetPointsArray 为空，则使用 UGameplayStatics::GetAllActorsOfClass 在当前世界中查找所有 ATargetPoint（level 中放置的点）并填充该数组。
	//这样只在第一次需要时才查询一次，提高效率。
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	//断言确保存在至少一个目标点，否则会崩溃并输出世界名和提示信息。这是为了在运行时尽快发现关卡缺少生成点的问题。
	checkf(!TargetPointsArray.IsEmpty(), TEXT("No vaild target point found in level: %s for spawning enemies"), *GetWorld() -> GetName());

	//本次调用实际生成敌人的计数器（无符号 32 位），用于返回给调用者。
	uint32 EnemiesSpawnedThisTime = 0;

	//遍历当前波的每一种敌人生成定义（每个定义包含软引用类与每次生成数量范围）
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow() -> EnemyWaveSpawnerDefinitions)
	{
		//如果软类为空则跳过该定义
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		//随机决定本次这个种类要生成多少个敌人，范围由 MinPerSpawnCount 和 MaxPerSpawnCount 决定。
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPerSpawnCount, SpawnerInfo.MaxPerSpawnCount);

		//在预加载映射中查找对应 UClass*。FindChecked 如果没找到会触发断言/崩溃。
		//这假设已经预加载并缓存了所需类，否则会出错（在 PreLoadNextWaveEnemies 中启动了异步加载并希望在等待阶段加载完成）
		UClass* LoadedEnemyClass = PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		//初始化生成参数结构体，用来指定SpawnActor的细节。
		FActorSpawnParameters SpawnParam;
		//设置碰撞处理策略：若冲突则尽可能调整位置，但仍然生成 Actor。避免因碰撞导致未生成
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		//内层循环：按需生成 NumToSpawn 个该类型的敌人
		for (int32 i = 0; i < NumToSpawn; ++i)
		{
			//随机选择一个目标点索引（作为生成附近的中心点）
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			//获取该目标点位置作为生成中心。
			const FVector SpawnOrigin = TargetPointsArray[RandomTargetPointIndex] -> GetActorLocation();
			//以目标点的前向向量为生成的朝向（把向量转为旋转）。
			const FRotator SpawnRotation = TargetPointsArray[RandomTargetPointIndex] -> GetActorForwardVector().ToOrientationRotator();

			//声明用于接收导航随机点的变量。
			FVector RandomLocation;
			//在导航网格上获取一个以 SpawnOrigin 为中心、半径 400.f 的随机可行走点，结果写入 RandomLocation。
			//K2_GetRandomLocationInNavigableRadius 是 Blueprint 友好的静态函数（C++ 端同样可调用）
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, SpawnOrigin, RandomLocation, 400.f);

			//在 Z 轴上向上偏移 150 单位，通常用于让生成点在空中一定高度，避免陷入地面或覆盖脚下碰撞。
			//注意这可能需要后续调整，以避免敌人掉落或穿模。
			RandomLocation += FVector(0.f, 0.f, 150.f);

			//实际调用 SpawnActor 用已加载 LoadedEnemyClass 生成敌人。返回生成的指针（或 nullptr 表示生成失败）。
			AWarriorEnemyCharacter* SpawnedEnemy = GetWorld() -> SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParam);

			//检查是否生成成功。
			if (SpawnedEnemy)
			{
				//给生成出来的敌人的 OnDestroyed 事件绑定 OnEnemyDestroyed 回调，使用 AddUniqueDynamic 避免重复绑定。
				//这样敌人死后会通知 GameMode（用于计数与触发后续逻辑）。
				SpawnedEnemy -> OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);

				//增加本次生成计数器。
				EnemiesSpawnedThisTime++;
				//统计当前波次累计已生成的敌人总数（用于和配置的 TotalEnemyToSpawnThisWave 做比较，决定是否还要继续生成）
				TotalSpawnEnemiesThisWAveCounter++;
			}

			//每生成一个敌人后检查是否还应继续生成（ShouldKeepSpawnEnemies() 会比较 TotalSpawnEnemiesThisWAveCounter 与本波次的目标总数）。
			//如果不应该继续，则立即返回本次生成总数，提前结束循环。这样可以避免超出每波指定的总生成数量
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}

	//如果所有定义都处理完且仍然可以继续，返回已生成数量（可能为 0）。
	return EnemiesSpawnedThisTime;
}

//返回是否还应该继续生成敌人的逻辑判断函数。
bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	//如果当前波累积生成数（TotalSpawnEnemiesThisWAveCounter）小于该波目标生成总数（DataTable 的 TotalEnemyToSpawnThisWave），则返回 true，
	//表示还要继续生成。否则返回 false。
	return TotalSpawnEnemiesThisWAveCounter < GetCurrentWaveSpawnerTableRow() -> TotalEnemyToSpawnThisWave;
}

//被绑定到敌人的 OnDestroyed 事件。当敌人被摧毁时（被销毁）引擎会调用这个函数，参数是被销毁的 Actor 指针。
void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	//把当前“已生成且仍未销毁的敌人计数”减 1。因为一个敌人被销毁了。
	//注意： 需要确保 CurrentSpawnedEnemiesCounter 在任何情况下都不会变为负值（建议在减之前加断言或使用 FMath::Max(0, ... - 1)）
	CurrentSpawnedEnemiesCounter--;

	//Debug::Print(FString::Printf(TEXT("CurrentSpawnedEnemiesCounter: %i, TotalSpawnEnemiesThisWaveCounter: %i, "), CurrentSpawnedEnemiesCounter, TotalSpawnEnemiesThisWAveCounter));

	//判断是否还需要继续生成敌人以满足本波次的总量配置。
	if (ShouldKeepSpawnEnemies())
	{
		//若仍需继续生成，则尝试生成更多敌人，并把实际生成数加到当前计数器中。
		//注意 TrySpawnWaveEnemies() 自身会绑定 OnDestroyed 给每个生成的敌人。
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	//如果不需要继续生成（ShouldKeepSpawnEnemies() 为 false）并且当前存活的敌人数为 0，
	//说明本波已完全清除（既已生成到达目标总数，也没有在世敌人），可判定波次完成。
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		//重置该波次累计生成计数，为下一波重新计数。
		TotalSpawnEnemiesThisWAveCounter = 0;
		//再次确保当前生存敌人计数为 0（对防止负值或残留计数作清零）。
		CurrentSpawnedEnemiesCounter = 0;

		//切换状态到 WaveCompleted，触发后续过渡逻辑（Tick 中的等待、切换下一波或结束）。
		SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState::WaveCompleted);
	}
}

//允许外部（可能是蓝图或其他生成系统）把已经生成的敌人列表注册到 GameMode 中，以便 GameMode 跟踪它们并在销毁时被通知。函数参数是敌人指针的数组引用。
void AWarriorSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister)
{
	//遍历传入的每个敌人指针。
	for (AWarriorEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		//空值检查，过滤有效指针。
		if (SpawnedEnemy)
		{
			//增加当前活跃敌人计数（表示 GameMode 需要跟踪这个敌人）。
			CurrentSpawnedEnemiesCounter++;

			//绑定 OnDestroyed 事件以便 GameMode 在敌人销毁时收到回调。AddUniqueDynamic 防止重复绑定。
			SpawnedEnemy -> OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
		}
	}
}
