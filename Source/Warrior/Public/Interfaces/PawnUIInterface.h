// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnUIInterface.generated.h"

class UEnemyUIComponent;
class UPawnUIComponent;
class UHeroUIComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WARRIOR_API IPawnUIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//纯虚函数，必须由实现类（Pawn）重写。返回 通用 Pawn UI 组件。
	virtual UPawnUIComponent* GetPawnUIComponent() const = 0;

	//返回HeroUIComponent
	virtual UHeroUIComponent* GetHeroUIComponent() const;

	//返回EnemyUIComponent
	virtual UEnemyUIComponent* GetEnemyUIComponent() const;
};
