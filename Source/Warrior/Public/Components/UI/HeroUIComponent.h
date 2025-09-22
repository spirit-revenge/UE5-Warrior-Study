// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

//声明了一个 动态多播委托，带一个参数
//TSoftObjectPtr<UTexture2D>（软引用的武器图标纹理）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>,SoftWeaponIcon);

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:
	//BlueprintAssignable 允许蓝图直接绑定这个事件
	UPROPERTY(BlueprintAssignable)
	//用于通知怒气值变化（0~1）
	FOnPercentChangedDelegate OnCurrentRageChanged;

	//BlueprintCallable 允许蓝图在实例上调用这个事件（通常用 Broadcast() 触发）
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	//参数类型是 TSoftObjectPtr<UTexture2D>，意味着武器图标是延迟加载的软引用资源
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;
};
