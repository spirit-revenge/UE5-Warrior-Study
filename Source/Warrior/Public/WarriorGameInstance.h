// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "WarriorGameInstance.generated.h"

USTRUCT(BlueprintType)
//游戏关卡配置单元
struct FWarriorGameLevelSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = ( Categories = "GameData.Level"))
	//GameplayTag 类型的字段；
	//用来表示关卡的逻辑标识
	FGameplayTag LevelTag;

	UPROPERTY(EditDefaultsOnly)
	//这是一个 软引用（TSoftObjectPtr）到 UWorld 对象；
	//UWorld 代表一个 地图文件
	TSoftObjectPtr<UWorld> Level;

	//检查此关卡配置是否完整有效；
	//只有当：
	//	LevelTag 有值（即有效 Tag）；
	//	Level 不为空；
	//	才返回 true。
	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//重写 UGameInstance::Init()；
	//游戏启动时（而不是关卡开始时）调用；
	//用来做全局初始化工作
	virtual void Init() override;
	
protected:
	//当 一个新地图开始加载 前会被调用；
	//常用于：
	//	显示加载界面；
	//	清除旧的缓存数据；
	//	准备新地图资源。
	virtual void OnPreLoadMap(const FString& MapName);
	//当目标世界（地图）加载完成 时触发；
	//可在这里执行：
	//	初始化该关卡的逻辑；
	//	传递游戏数据；
	//	关闭加载界面。
	virtual void OnDestinationWorldLoaded(UWorld* LoadedWorld);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//这是在编辑器中配置的关卡列表；
	//每一项代表一个 FWarriorGameLevelSet
	TArray<FWarriorGameLevelSet> GameLevelSets;

public:
	UFUNCTION(BlueprintPure, meta = (GameplayTagFilter = "GameData.Level"))
	//通过输入一个 GameplayTag，找到对应的地图资源（软引用）。
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const;
};
