// Spirit Revenge All Rights Reserved


#include "WarriorTypes/WarriorCountDownAction.h"

void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	//一旦外部调用 CancelAction()，这里立即检测到并终止执行
	if (bNeedToCancel)
	{
		//输出状态明确为 Cancelled
		CountDownOutput = EWarriorCountDownActionOutput::Cancelled;

		//使用 FinishAndTriggerIf(true, ...) 会触发蓝图后续连接，引脚立即返回
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);

		//返回，结束 Latent
		return;
	}

	//如果时间累计到总时长
	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		//标记为 Completed 
		CountDownOutput = EWarriorCountDownActionOutput::Completed;

		//使用 FinishAndTriggerIf(true, ...) 会触发蓝图后续连接，引脚立即返回
		//第一个参数 true 表明触发（trigger）执行引脚。
		//ExecutionFunction：通常是蓝图里要被触发的函数名（FName），由 FLatentActionInfo 提供。
		//OutputLink：蓝图节点的链接索引（哪个输出执行引脚）。
		//CallbackTarget：触发时的目标对象（通常是拥有该 latent 节点的 UObject / Blueprint）。
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);

		//返回，结束 Latent
		return;
	}

	//比较当前间隔计时器 ElapsedInterval 与期望的更新间隔 UpdateInterval。
	if (ElapsedInterval < UpdateInterval)
	{
		//把当前帧的时间步（Response.ElapsedTime() — 通常相当于 DeltaTime）加入到 ElapsedInterval。
		ElapsedInterval += Response.ElapsedTime();
	}
	//当 ElapsedInterval >= UpdateInterval
	else
	{
		//把累计的总体运行时间 ElapsedTimeSinceStart 增加上一个值。这个表达式说明两点：
		//1.优先用 UpdateInterval 自增：如果 UpdateInterval 大于 0，
		//	表示我们把时间推进一个“完整的更新间隔”单位（而不是用精确的 ElapsedTime()）。这种做法可以使“更新时间点”更稳定（按预期的间隔跳步）。
		//2.否则退回到 Response.ElapsedTime()：若 UpdateInterval 非正（<= 0），就用当前帧实际流逝时间推进，避免加 0 导致不前进的死循环。
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();

		//计算剩余时间并写回外部引用 OutRemainingTime（这是构造函数中传入的外部引用变量）。
		//这样，蓝图或调用者可以立即读取到最新的剩余时间值用于显示 UI、文字等。
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;

		//将当前动作输出状态设为 Updated，表示这是一次“周期性更新”事件（与完成/取消区分开）
		CountDownOutput = EWarriorCountDownActionOutput::Updated;

		//触发蓝图节点对应的执行引脚，但不结束 latent action。
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);

		//重置间隔累积计时器，开始新的间隔累积。下一次调用 UpdateOperation 会再次累加 Response.ElapsedTime()，直到再次达到 UpdateInterval
		ElapsedInterval = 0.f;
	}
}

void FWarriorCountDownAction::CancelAction()
{
	//将内部标志 bNeedToCancel 设为 true。
	//这样下一次 UpdateOperation() 被引擎调用时（通常就是下一帧），
	//它会在开头检测到 bNeedToCancel 并执行取消分支（设置 CountDownOutput 为 Cancelled 并调用 FinishAndTriggerIf(...)），
	//从而安全地结束 latent action 并触发蓝图的取消输出。
	bNeedToCancel = true;
}
