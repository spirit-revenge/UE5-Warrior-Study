// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WarriorEnumTypes.h"

/*
 * 这段 FWarriorCountDownAction 是一个非常典型的 UE Latent Action（延迟异步动作） 的自定义实现，
 * 用来在蓝图或 C++ 中实现异步倒计时（CountDown） 的逻辑
 * 在 Unreal Engine 中，FPendingLatentAction 是实现异步蓝图节点（带执行引脚延迟）的核心机制。
 * 它用于 UKismetSystemLibrary::Delay()、MoveTo()、Async Load 等类似节点的底层实现。
 */
class FWarriorCountDownAction : public FPendingLatentAction
{
public:
	//构造方法
	//InTotalCountDownTime		总倒计时时间
	//InUpdateInterval		每隔多久更新一次（如每 0.1 秒触发一次 Tick）
	//InOutRemainingTime		剩余时间的引用变量（外部共享，供蓝图或逻辑层访问）
	//InCountDownOutput		输出状态
	//LatentInfo	蓝图延迟系统必需信息，包含回调函数、目标对象等
	FWarriorCountDownAction(float InTotalCountDownTime, float InUpdateInterval, float& InOutRemainingTime, EWarriorCountDownActionOutput& InCountDownOutput, const FLatentActionInfo& LatentInfo)
	: bNeedToCancel(false)
	, TotalCountDownTime(InTotalCountDownTime)
	, UpdateInterval(InUpdateInterval)
	, OutRemainingTime(InOutRemainingTime)
	, CountDownOutput(InCountDownOutput)
	, ExecutionFunction(LatentInfo.ExecutionFunction)
	, OutputLink(LatentInfo.Linkage)
	, CallbackTarget(LatentInfo.CallbackTarget)
	, ElapsedInterval(0.f)
	, ElapsedTimeSinceStart(0.f)
	{
	}

	//这是 Latent Action 的心脏，每帧都会被调用
	virtual void UpdateOperation(FLatentResponse& Response) override;

	//当外部（C++ 或蓝图）调用 CancelAction() 时，下次 Tick 就会检测到并提前结束。
	void CancelAction();
	
private:
	bool bNeedToCancel;               // 控制是否提前取消倒计时
	float TotalCountDownTime;         // 总倒计时长度
	float UpdateInterval;             // 每次更新间隔
	float& OutRemainingTime;          // 外部共享的剩余时间变量
	EWarriorCountDownActionOutput& CountDownOutput; // 输出枚举（表示完成/取消）
	FName ExecutionFunction;          // 结束后要执行的蓝图函数名
	int32 OutputLink;                 // 蓝图连线索引
	FWeakObjectPtr CallbackTarget;    // 蓝图延迟回调目标
	float ElapsedInterval;            // 当前间隔内经过的时间
	float ElapsedTimeSinceStart;      // 自开始以来的总时间
};
