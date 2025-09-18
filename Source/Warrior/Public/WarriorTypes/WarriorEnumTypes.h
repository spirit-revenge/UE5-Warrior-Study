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