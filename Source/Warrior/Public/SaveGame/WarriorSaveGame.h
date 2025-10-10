// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WarriorTypes/WarriorEnumTypes.h"
#include "WarriorSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	//用来存储当前的游戏难度
	EWarriorGameDifficulty SavedCurrentGameDifficulty;
};
