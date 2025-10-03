// Spirit Revenge All Rights Reserved


#include "Components/UI/EnemyUIComponent.h"

#include "Widgets/WarriorWidgetBase.h"

void UEnemyUIComponent::RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	EnemyDrawWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawWidgetIfAny()
{
	if (EnemyDrawWidgets.IsEmpty())
	{
		return;
	}

	for (UWarriorWidgetBase* DrawWidget : EnemyDrawWidgets)
	{
		if (DrawWidget)
		{
			DrawWidget -> RemoveFromParent();
		}
	}

	EnemyDrawWidgets.Empty();
}
