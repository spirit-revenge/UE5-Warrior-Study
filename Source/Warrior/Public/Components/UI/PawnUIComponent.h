// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

//声明一个 可在蓝图中绑定的多播委托。
//“OneParam” 表示这个委托有一个参数。
//“Dynamic” 表示它可以在蓝图中动态绑定/解绑，支持反射系统。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangedDelegate,float,NewPercent);

/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnUIComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	//BlueprintAssignable 允许蓝图直接绑定这个事件
	UPROPERTY(BlueprintAssignable)
	//血量百分比变化通知（0~1）
	FOnPercentChangedDelegate OnCurrentHealthChanged;
};
