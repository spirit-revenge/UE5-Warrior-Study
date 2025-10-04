// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"

class AWarriorEnemyCharacter;

//委托事件， 类型为Enemy的数组
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate, const TArray<AWarriorEnemyCharacter*>&, SpawnedEnemies);
/**
 * 
 */
UCLASS()
class WARRIOR_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	//Blueprint 内部节点
	//记录参数（EventTag、EnemyClass、数量、生成位置、随机范围）
	UFUNCTION(BlueprintCallable,
		Category = "Warrior|AbilityTasks",
		meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies",
			HidePin = "OwningAbility",
			DefaultToSelf = "OwningAbility",
			BlueprintInternalUseOnly = "true",
			NumToSpawn = "1",
			RandomSpawnRadius = "200"))
	//	返回任务实例
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag, //要监听的事件标签
		TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, //软类引用，允许蓝图打软引用而不马上加载资源
		int32 NumToSpawn, //生成参数
		const FVector& SpawnOrigin, //生成参数
		float RandomSpawnRadius); //生成参数

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;

	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

	//~ Begin UGameplayTask Interface
	//注册 AbilitySystemComponent->GenericGameplayEventCallbacks 监听指定 EventTag
	//等事件到来后调用 OnGameplayEventReceived
	virtual void Activate() override;
	//OnDestroy()
	//取消事件绑定（DelegateHandle）
	virtual void OnDestroy(bool bInOwnerFinished) override;
	//~ End UGameplayTask Interface
	
private:
	/* 缓存的参数 */
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AWarriorEnemyCharacter> CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	
	FDelegateHandle DelegateHandle; //用于在 OnDestroy() 从 AbilitySystemComponent 中移除绑定

	//检查 EnemyClass 是否有效
	//如果是软引用（TSoftClassPtr），则用 StreamableManager 异步加载
	//加载完成后调用 OnEnemyClassLoaded()
	void OnGameplayEventReceived(const FGameplayEventData* InPayload);
	//根据数量和随机位置生成敌人
	void OnEnemyClassLoaded();
};
