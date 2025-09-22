// Spirit Revenge All Rights Reserved


#include "Widgets/WarriorWidgetBase.h"
#include "Interfaces/PawnUIInterface.h"

void UWarriorWidgetBase::NativeOnInitialized()
{
	//调用了 Super::NativeOnInitialized()，保证引擎生命周期正常
	Super::NativeOnInitialized();

	//通过 GetOwningPlayerPawn() → Cast<IPawnUIInterface>() → GetHeroUIComponent() 来拿英雄 UI 组件
	//判断是否有PawnUIInterface
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		//判断HeroUIComponent是否存在
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			//调用了 BP_OnOwningHeroUIComponentInitialized，蓝图可以直接响应
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}

void UWarriorWidgetBase::InitEnemyCreateWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		//直接从 OwningEnemyActor Cast 到 IPawnUIInterface
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();

		//用 checkf 强制要求 EnemyUIComponent 不为空，防止后续蓝图接收到 nullptr 崩溃。
		checkf(EnemyUIComponent,TEXT("Failed to extrac an EnemyUIComponent from %s"), *OwningEnemyActor->GetActorNameOrLabel())

		//调用了 BP_OnOwningEnemyUIComponentInitialized，蓝图可以直接响应
		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}
