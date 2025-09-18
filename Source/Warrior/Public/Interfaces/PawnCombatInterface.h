// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnCombatInterface.generated.h"

class UPawnCombatComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IPawnCombatInterface 是一个纯接口（纯虚类），不能直接实例化。
 * 作用：只声明功能，不实现功能，让实现类自己提供具体逻辑。
 */
class WARRIOR_API IPawnCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//纯虚函数 = 0：
	//每个继承这个接口的类必须实现 GetPawnCombatComponent()。
	//返回类型是 UPawnCombatComponent*，即角色或敌人的战斗组件。
	virtual UPawnCombatComponent* GetPawnCombatComponent() const = 0;
};
