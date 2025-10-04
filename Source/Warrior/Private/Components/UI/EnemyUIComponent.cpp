// Spirit Revenge All Rights Reserved


#include "Components/UI/EnemyUIComponent.h"

#include "Widgets/WarriorWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	//把传入的 Widget（通常是血条、锁定图标等）注册到 EnemyDrawWidgets 数组中保存
	EnemyDrawWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawWidgetIfAny()
{
	//如果数组是空的，就什么都不做（避免不必要的循环）
	if (EnemyDrawWidgets.IsEmpty())
	{
		return;
	}

	//循环访问每一个注册过的敌人 UI 元素。
	for (UWarriorWidgetBase* DrawWidget : EnemyDrawWidgets)
	{
		if (DrawWidget)
		{
			//RemoveFromParent() 是 UMG 的标准函数，
			//会把该 Widget 从屏幕上或世界空间中移除。
			//注意：它不会立即销毁对象（Destroy），而是从 UI 层解除绑定
			DrawWidget -> RemoveFromParent();
		}
	}

	//把内部记录的 Widget 数组清空，避免保留悬空指针。
	EnemyDrawWidgets.Empty();
}
