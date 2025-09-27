// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WarriorAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()

public:
	//构造函数
	AWarriorAIController(const FObjectInitializer& ObjectInitializer);

	//~ Begin IGenericTeamAgentInterface Interface.
	//团队关系判断函数（来自 IGenericTeamAgentInterface 接口）：
	//Friendly：友方
	//Neutral：中立
	//Hostile：敌方
	//游戏中 AI 会通过这个函数判断目标是否需要攻击。
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//~ End IGenericTeamAgentInterface Interface.
protected:
	//在 AI Controller 被生成并控制 Pawn 时调用。
	virtual void BeginPlay() override;

	//EnemyPerceptionComponent：AI 的感知系统组件，可以添加视觉、听觉等感知。
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;

	//	AISenseConfig_Sight：专门配置视觉感知参数（视野角度、感知范围、失去视野延迟等）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	//当感知系统发现或失去目标时，会触发这个回调。
	//Actor：被感知的目标
	//FAIStimulus：感知触发信息（距离、是否被阻挡、感知类型等）
	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus  Stimulus);

private:
	/*Detour Crowd Avoidance 是 UE5 中导航系统的一部分，用于处理多个 AI 同时移动时的碰撞和避让。*/
	//bEnableDetourCrowdAvoidance：是否启用
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance = true;

	//DetourCrowdAvoidanceQuality：避让精度（越高越平滑，但性能消耗大）
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config",
		meta=(EditCondition = "bEnableDetourCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;

	//CollisionQueryRange：检测周围障碍物或其他 AI 的范围
	UPROPERTY(EditDefaultsOnly, Category="Detour Crowd Avoidance Config", meta=(EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
