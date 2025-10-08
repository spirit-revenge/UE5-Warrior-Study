#pragma once

//它被用作 蓝图的 Exec 输出引脚（通过 ExpandEnumAsExecs），这样可以在蓝图中直接用类似 Branch 的分支执行流，而不是返回一个 bool。
UENUM()
enum class EWarriorConfirmType : uint8
{
	Yes,
	No
};

//它被用作 蓝图的 Exec 输出引脚（通过 ExpandEnumAsExecs），这样可以在蓝图中直接用类似 Branch 的分支执行流，而不是返回一个 bool。
UENUM()
enum class EWarriorValidType : uint8
{
	Valid,
	Invalid
};

//它被用作 蓝图的 Exec 输出引脚（通过 ExpandEnumAsExecs），这样可以在蓝图中直接用类似 Branch 的分支执行流，而不是返回一个 bool。
UENUM()
enum class EWarriorSuccessType : uint8
{
	Successful,
	Failed
};

UENUM()
enum class EWarriorCountDownActionInput : uint8
{
	Start, //表示“启动”倒计时动作的输入信号。
	Cancel //表示“取消”倒计时动作的输入信号。
};

UENUM()
enum class EWarriorCountDownActionOutput : uint8
{
	Updated,   //表示当前倒计时仍在进行中，且刚更新过一次。
	Completed, //表示倒计时正常结束。
	Cancelled  //表示倒计时被提前终止（用户或系统取消）。
};

//游戏难度等级
UENUM(BlueprintType)
enum class EWarriorGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard
};
