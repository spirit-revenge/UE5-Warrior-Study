// Spirit Revenge All Rights Reserved


#include "Characters/WarriorBaseCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //表示这个 Actor 永远不会启用 Tick
	PrimaryActorTick.bStartWithTickEnabled = false;//表示当游戏一开始时，这个 Actor 的 Tick 是 禁用的

	GetMesh()->bReceivesDecals = false;//禁止角色的 Mesh 接收贴花效果（比如血迹、弹孔）

	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));

	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));
}

UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}

void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (WarriorAbilitySystemComponent)
	{
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this,this);

		//以下两种都可选择
		//ensure(!CharacterStartUpData.IsNull());
		//if (ensure(!CharacterStartUpData.IsNull())){}
		ensureMsgf(!CharacterStartUpData.IsNull(),TEXT("Forgot to assign start up data to %s"),*GetName());

		
	}
}


