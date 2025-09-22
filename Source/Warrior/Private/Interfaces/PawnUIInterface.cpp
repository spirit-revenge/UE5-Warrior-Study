// Spirit Revenge All Rights Reserved


#include "Interfaces/PawnUIInterface.h"

// Add default functionality here for any IPawnUIInterface functions that are not pure virtual.
UHeroUIComponent* IPawnUIInterface::GetHeroUIComponent() const
{
	//具体实现由实现类重写
	return nullptr;
}

UEnemyUIComponent* IPawnUIInterface::GetEnemyUIComponent() const
{
	//具体实现由实现类重写
	return nullptr;
}
