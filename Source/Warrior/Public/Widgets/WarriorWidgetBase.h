// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

class UEnemyUIComponent;
class UHeroUIComponent;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()
protected:
	//这是 UUserWidget 的虚函数，生命周期方法之一。
	//它会在 Widget 第一次初始化时调用（比 Construct 更早，只会调用一次）。
	//通常你会在这里做一次性绑定（比如委托、事件监听），或者查找 UI 组件。
	virtual void NativeOnInitialized() override;

	//BlueprintImplementableEvent：纯虚函数，没有 C++ 实现，留给蓝图实现。
	//DisplayName 让蓝图中显示更友好的名字。
	//HeroUI组件初始化时调用
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Owning Hero UI Component Initialized"))
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OwningHeroUIComponent);

	//BlueprintImplementableEvent：纯虚函数，没有 C++ 实现，留给蓝图实现。
	//DisplayName 让蓝图中显示更友好的名字。
	//EnemyUI组件初始化时调用
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On Owning Enemy UI Component Initialized"))
	void BP_OnOwningEnemyUIComponentInitialized(UEnemyUIComponent* OwningEnemyUIComponent);
public:
	//BlueprintCallable：允许蓝图直接调用这个函数。
	UFUNCTION(BlueprintCallable)
	//初始化Enemy的Widget UI
	void InitEnemyCreateWidget(AActor* OwningEnemyActor);
};
