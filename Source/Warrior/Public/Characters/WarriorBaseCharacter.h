// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "Interfaces/PawnUIInterface.h"

#include "WarriorBaseCharacter.generated.h"

class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
class UDataAsset_StartUpDataBase;
class UMotionWarpingComponent;
/**
 * 基础角色类
 * ACharacter：
 *		UE 提供的角色类，带有胶囊体、移动组件、Mesh 等基础功能。
 *		适合做可控制的角色（玩家或 AI）
 * IAbilitySystemInterface：
 *		这是 UE GAS (Gameplay Ability System) 的关键接口。
 *		实现后，GAS 系统可以通过这个角色获取它的 AbilitySystemComponent
 */
UCLASS()
class WARRIOR_API AWarriorBaseCharacter : public ACharacter,public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWarriorBaseCharacter();//构造函数

	//~ Begin IAbilitySystemInterface Interface.
	//实现 IAbilitySystemInterface 的纯虚函数。 GAS 系统调用这个函数，获取角色的 AbilitySystemComponent
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface

	//~ Begin IPawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//~ End IPawnUIInterface Interface
protected:
	//~ Begin APawn Interface.
	/*
	 * 当 Pawn 被一个 Controller（玩家或 AI）接管时调用。
	 * 常用来：
	 *		初始化 Ability System（给角色应用初始能力）。
	 *		应用 Startup Data（初始属性、技能）
	 */
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	//VisibleAnywhere 说明在编辑器可以看到，但不能改引用
	//你的角色专属的 Ability System Component
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	//属性集合 存储角色的生命值、魔法值、攻击力、防御力等数值。GAS 通过这个类读取和修改属性。
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;
	
	//软引用的数据资产，保存角色的“初始数据”。可以在蓝图里给不同角色指定不同的 DataAsset，方便复用逻辑
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;
	
public:
	//内联函数，提供快速访问。蓝图和 C++ 都能直接调用，性能比普通函数更高
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const {return WarriorAbilitySystemComponent;}

	//内联函数
	//UWarriorAttributeSet 是 GAS 的 AttributeSet，用来存储角色的数值属性
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const {return WarriorAttributeSet;}
};
