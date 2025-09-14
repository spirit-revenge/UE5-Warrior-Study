// Spirit Revenge All Rights Reserved

#pragma once

#include "NativeGameplayTags.h"

/*
 * Warrior项目的标签
 */
namespace WarriorGameplayTags
{
	/** Input Tags **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTags_Move);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTags_Look);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTags_EquipAxe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTags_UnequipAxe);

	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTags_LightAttack_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTags_HeavyAttack_Axe);
	
	/** Player Tags **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
}
