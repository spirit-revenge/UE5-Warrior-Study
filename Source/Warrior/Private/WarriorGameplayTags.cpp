// Spirit Revenge All Rights Reserved


#include "WarriorGameplayTags.h"

/*
 * 定义GameplayTags
 */
namespace WarriorGameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTags_Move,"InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTags_Look,"InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTags_EquipAxe,"InputTag.EquipAxe");
	UE_DEFINE_GAMEPLAY_TAG(InputTags_UnequipAxe,"InputTag.UnequipAxe");

	/** Player Tags **/
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Equip_Axe,"Player.Ability.Equip.Axe");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Unequip_Axe,"Player.Ability.Unequip.Axe");

	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Axe,"Player.Weapon.Axe")
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Equip_Axe,"Player.Event.Equip.Axe");
	UE_DEFINE_GAMEPLAY_TAG(Player_Event_Unequip_Axe,"Player.Event.Unequip.Axe");
}
