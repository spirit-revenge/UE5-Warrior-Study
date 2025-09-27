// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorHeroController.generated.h"

/**
 * 玩家控制器类，用于接收输入、驱动角色、管理 HUD 等
 */
UCLASS()
class WARRIOR_API AWarriorHeroController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()//自动生成反射和序列化代码

public:
	//构造函数
	AWarriorHeroController();

	//~ Begin IGenericTeamAgentInterface Interface.
	//返回玩家角色所属的团队 ID，方便 AI 判断敌友关系
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface Interface.
private:
	//用于存储玩家团队 ID。
	FGenericTeamId HeroTeamID;
};
