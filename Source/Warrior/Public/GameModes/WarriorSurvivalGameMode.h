// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "WarriorSurvivalGameMode.generated.h"

class AWarriorEnemyCharacter;

UENUM(BlueprintType)
//游戏状态枚举
enum class EWarriorSurvivalGameModeState : uint8
{
	WaitSpawnNewWave, //等待生成新的一波敌人
	SpawningNewWave, //正在生成一波敌人
	InProgress, //正在进行战斗中
	WaveCompleted, //生成的一波敌人已完成
	AllWavesDone, //所有波敌人的都已完成
	PlayerDied //玩家死亡
};

USTRUCT(BlueprintType)
//敌人生成信息结构体
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	//要生成的敌人种类。TSoftClassPtr可异步加载、避免强依赖
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	UPROPERTY(EditAnywhere)
	//生成的最小值
	int32 MinPerSpawnCount = 1;

	UPROPERTY(EditAnywhere)
	//生成的最大值
	int32 MaxPerSpawnCount = 3;
};

USTRUCT(BlueprintType)
//这就是每一波的配置表。
struct FWarriorEnemyWaveSpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	//每一波敌人的敌人生成信息结构体
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	UPROPERTY(EditAnywhere)
	//一波敌人总共生成多少个敌人
	int32 TotalEnemyToSpawnThisWave = 1;
};

//动态委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameModeStateChangedDelegate, EWarriorSurvivalGameModeState, CurrentState);

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorSurvivalGameMode : public AWarriorBaseGameMode
{
	GENERATED_BODY()

protected:
	//初始化 TargetPoints、读取 DataTable、设置第一波
	virtual void BeginPlay() override;
	//状态机逻辑
	virtual void Tick(float DeltaTime) override;

private:
	//改变状态并触发事件广播
	void SetCurrentSurvivalGameModeState(EWarriorSurvivalGameModeState InState);
	//检查是否已完成全部波次
	bool HasFinishedAllWaves() const;
	//异步加载下一波敌人类（配合 TSoftClassPtr）
	void PreLoadNextWaveEnemies();
	//从 DataTable 读取当前波次数据
	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow() const;
	//实际生成敌人
	int32 TrySpawnWaveEnemies();
	//判断是否继续生成（比如剩余敌人没死完）
	bool ShouldKeepSpawnEnemies() const;

	UFUNCTION()
	//敌人销毁事件，当敌人死亡后计数器减少，判断波是否完成。
	void OnEnemyDestroyed(AActor* DestroyedActor);
	
	UPROPERTY()
	//存储当前生存游戏模式的状态
	EWarriorSurvivalGameModeState CurrentSurvivalGameModeState;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	//委托的事件，用于生存游戏状态被改变
	FOnSurvivalGameModeStateChangedDelegate OnSurvivalGameModeStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	//波次数据字段
	UDataTable* EnemyWaveSpawnerDataTable;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	//总波数
	int32 TotalWavesToSpawn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	//当前波数
	int32 CurrentWaveCount = 1;

	UPROPERTY()
	//已生成敌人数
	int32 CurrentSpawnedEnemiesCounter = 0;

	UPROPERTY()
	//当前波的总目标生成数
	int32 TotalSpawnEnemiesThisWAveCounter = 0;

	UPROPERTY()
	TArray<AActor*> TargetPointsArray;
	
	UPROPERTY()
	//从开始消耗的时间
	float TimePassedSinceStart = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	//生成新一波敌人等待的时间
	float SpawnNewWaveWaitTime = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	//生成敌人的延时时间
	float SpawnEnemiesDelayTime = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	//一波敌人完成时的等待时间
	float WaveCompletedWaitTime = 5.f;

	UPROPERTY()
	//防止重复加载的缓存表，用于节约异步加载资源时间
	TMap<TSoftClassPtr<AWarriorEnemyCharacter>,UClass*> PreLoadedEnemyClassMap;

public:
	UFUNCTION(BlueprintCallable)
	//外部可以手动登记生成的敌人，方便在蓝图里配合特殊生成逻辑
	void RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister);
};

