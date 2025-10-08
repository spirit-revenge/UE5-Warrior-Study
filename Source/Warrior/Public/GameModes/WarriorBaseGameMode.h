// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorBaseGameMode : public AGameModeBase
{
	GENERATED_BODY() //自动生成反射和序列化代码

public:
	//构造函数
	AWarriorBaseGameMode();

protected:
	//EditDefaultsOnly 允许在蓝图类默认值（Class Defaults）中编辑，而运行时或实例中无法修改。
	//BlueprintReadOnly 可以在蓝图中读取此值，但不能修改。
	//（配合上面的 EditDefaultsOnly，形成“只能在编辑器设置、不能在运行时修改”的控制模式）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	//这个属性可以在你的 GameMode 蓝图里设置默认难度
	EWarriorGameDifficulty CurrentGameDifficulty;
	
public:
	//FORCEINLINE：提示编译器将函数内联，以提升性能。
	//const：保证此函数不会修改成员变量。
	//简洁的访问器函数，用于 C++ 端访问蓝图设定的游戏难度。
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
};
