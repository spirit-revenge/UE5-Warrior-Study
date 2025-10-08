// Spirit Revenge All Rights Reserved


#include "GameModes/WarriorBaseGameMode.h"

AWarriorBaseGameMode::AWarriorBaseGameMode()
{
	//PrimaryActorTick.bCanEverTick = true;
	//表示该类（继承自 AActor）可以执行 Tick()。
	//只有当这个标志为 true 时，Tick(float DeltaTime) 才会被调用。
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;
	//表示游戏开始时 Tick 自动启用。
	//如果不设为 true，即使类支持 Tick，也需要手动调用 SetActorTickEnabled(true) 才会执行
	PrimaryActorTick.bStartWithTickEnabled = true;
}
