// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToFaceTarget.generated.h"

struct FRotateToFaceTargetTaskMemory
{
	//OwningPawn
	//存储执行旋转任务的 Pawn（通常是 AI 控制的角色）的弱指针。
	//使用 TWeakObjectPtr 是为了防止对象被销毁后仍然持有引用，避免悬空指针。
	TWeakObjectPtr<APawn> OwningPawn;
	//TargetActor
	//存储要朝向的目标 Actor 的弱指针（比如玩家或者其他 AI）。
	//同样使用弱指针保证安全。
	TWeakObjectPtr<AActor> TargetActor;

	//TargetActor
	//存储要朝向的目标 Actor 的弱指针（比如玩家或者其他 AI）。
	//同样使用弱指针保证安全。
	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}

	//用于清空内部状态，释放指针引用。
	//在任务完成或失败时调用，保证下一次任务执行时不会误用上一次的残留数据。
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UBTTask_RotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

	//构造函数
	UBTTask_RotateToFaceTarget();
	
	//~ Begin UBTNode Interface
	//InitializeFromAsset → 解析黑板 Key
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//GetInstanceMemorySize → 返回任务内存大小
	virtual uint16 GetInstanceMemorySize() const override;
	//GetInstanceMemorySize → 返回任务内存大小（如果你要自定义 FBTTaskMemory 存状态用），不需要可以返回 0
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

	//ExecuteTask
	//在 BT 节点刚开始执行时调用。
	//如果旋转可以瞬间完成，就直接 return Succeeded;。
	// Tick 慢慢旋转，返回 EBTNodeResult::InProgress。
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//TickTask
	//只有在 ExecuteTask 返回 InProgress 并且 bNotifyTick = true 时才会被调用。
	//每帧做朝向插值，检测是否达到了角度精度（HasReachedAnglePercision），达到了就 FinishLatentTask(Succeeded)。
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//用来判断 Pawn 的朝向是否已经对准目标
	bool HasReachedAnglePercision(APawn* QueryPawn, AActor* TargetActor) const;
	
	UPROPERTY(EditAnywhere, Category = "Face Target")
	float AnglePrecision; // 允许的误差角度

	UPROPERTY(EditAnywhere, Category = "Face Target")
	float RotationInterpSpeed; // 插值速度

	UPROPERTY(EditAnywhere, Category = "Face Target")
	FBlackboardKeySelector InTargetToFaceKey; // 黑板里的目标 Actor
};
