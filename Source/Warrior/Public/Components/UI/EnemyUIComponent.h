// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"

class UWarriorWidgetBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	//把一个 UI Widget 注册到当前敌人的 UI 列表中。
	//这通常在 敌人生成时 或 UI 创建时 调用
	void RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister);

	UFUNCTION(BlueprintCallable)
	//清理或销毁所有已注册的敌人 UI Widget
	void RemoveEnemyDrawWidgetIfAny();
private:
	//这是一个数组，用来保存所有已经注册到这个敌人身上的 UI Widget。
	//每个 UWarriorWidgetBase 实例通常代表一个 世界空间中的 UI 元素（World Space Widget）
	TArray<UWarriorWidgetBase*> EnemyDrawWidgets;
};
