// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"

/**
 * 所有 Pawn 组件（比如战斗组件）的基类
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARRIOR_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	//返回当前组件所属的 Pawn，并强制转换为指定类型。
	template<class T>
	T* GetOwningPawn() const
	{
		//编译期检查，确保你传入的 T 一定是 APawn 或其子类，否则编译直接报错。
		static_assert(TPointerIsConvertibleFromTo<T,APawn>::Value,"'T' template Parameter to GetPawn must be derived from APawn");
		//运行时安全转换，如果转换失败会触发 check()（游戏直接断言），能避免拿到错误类型的对象。
		return CastChecked<T>(GetOwner());
	}

	//不指定模板类型，直接返回基类 APawn*
	//当你只需要知道它是个 Pawn，不需要特定子类时，可以用这个版本
	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	//获取当前 Pawn 的控制器，并强制转换成指定类型。
	template<class T>
	T* GetOwningController() const
	{
		//编译期检查，确保你传入的 T 一定是 AController 或其子类，否则编译直接报错
		static_assert(TPointerIsConvertibleFromTo<T,AController>::Value,"'T' template Parameter to GetController must be derived from AController");

		//GetController<T>() 是 APawn 的模板函数，可以直接得到指定类型的控制器
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
