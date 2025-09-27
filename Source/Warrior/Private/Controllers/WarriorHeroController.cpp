// Spirit Revenge All Rights Reserved


#include "Controllers/WarriorHeroController.h"

AWarriorHeroController::AWarriorHeroController()
{
	//给玩家控制器指定默认团队 ID。
	HeroTeamID = FGenericTeamId(0);
}

FGenericTeamId AWarriorHeroController::GetGenericTeamId() const
{
	//给玩家控制器指定默认团队 ID。
	return HeroTeamID;
}
