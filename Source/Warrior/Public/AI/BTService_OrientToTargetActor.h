// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientToTargetActor.generated.h"

/**
 * 该类是用于enemy攻击时进行朝向的
 * 这个 UBTService_OrientToTargetActor 是一个 行为树服务 (BTService)，
 * 作用是让 AI Pawn 在行为树运行时，持续朝向黑板里记录的目标 Actor（比如玩家）
 */
UCLASS()
class WARRIOR_API UBTService_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()

	//构造方法
	UBTService_OrientToTargetActor();

	//~ Begin UBTNode Interface
	//从 BehaviorTree 初始化服务
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//返回编辑器里显示的说明文本，便于调试和文档化
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//行为树 Tick 时会调用这里。
	//这里的逻辑就是获取黑板里的目标 Actor，然后让 Pawn 转向目标（用插值避免瞬间转身）
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//黑板键选择器。
	//允许你在行为树里选择一个 Actor 类型的黑板值（通常是玩家）
	UPROPERTY(EditAnywhere, Category = "Target")
	FBlackboardKeySelector InTargetActorKey;

	//插值速度，控制旋转朝向的平滑度。
	UPROPERTY(EditAnywhere, Category = "Target")
	float RotationInterpSpeed;
};
