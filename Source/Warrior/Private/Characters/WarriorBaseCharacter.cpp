// Spirit Revenge All Rights Reserved


#include "Characters/WarriorBaseCharacter.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "MotionWarpingComponent.h"

// Sets default values
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //表示这个 Actor 永远不会启用 Tick
	PrimaryActorTick.bStartWithTickEnabled = false;//表示当游戏一开始时，这个 Actor 的 Tick 是 禁用的

	GetMesh()->bReceivesDecals = false;//禁止角色的 Mesh 接收贴花效果（比如血迹、弹孔）

	//为角色创建一个 UWarriorAbilitySystemComponent 子组件
	//CreateDefaultSubobject 在构造函数中创建子对象/子组件
	//TEXT("WarriorAbilitySystemComponent") → 子组件名字，可在编辑器或蓝图中看到
	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));

	//为角色创建一个 UMotionWarpingComponent 子组件
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
	//为角色创建属性集（AttributeSet）子组件
	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));
}

//GAS 系统需要通过角色拿到它的 AbilitySystemComponent，就会调用这个函数。
//实际返回的是我们自定义的 UWarriorAbilitySystemComponent
UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}

UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* AWarriorBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
	//先执行 ACharacter 的默认行为
	Super::PossessedBy(NewController);

	//防止空指针
	if (WarriorAbilitySystemComponent)
	{
		//非常关键的一步，必须调用，否则 GAS 不知道这个组件的拥有者和 Avatar
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this,this);

		//以下两种都可选择
		//ensure(!CharacterStartUpData.IsNull());
		//if (ensure(!CharacterStartUpData.IsNull())){}

		
		//如果没有给角色分配 CharacterStartUpData，会在日志里输出警告。
		//方便开发者发现忘记配置 Startup Data 的问题
		ensureMsgf(!CharacterStartUpData.IsNull(),TEXT("Forgot to assign start up data to %s"),*GetName());

		
	}
}


