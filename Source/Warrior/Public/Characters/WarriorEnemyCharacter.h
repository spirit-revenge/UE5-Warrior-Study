// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UWidgetComponent;
class UEnemyCombatComponent;
class UEnemyUIComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	//构造方法
	AWarriorEnemyCharacter();

	//~ Begin IPawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;
	//~ End IPawnUIInterface Interface
	
protected:
virtual void BeginPlay() override;
	
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	//VisibleAnywhere, BlueprintReadOnly → 蓝图可查看，不可修改
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	//EnemyCombatComponent → 敌人的战斗逻辑组件。
	UEnemyCombatComponent* EnemyCombatComponent;

	//VisibleAnywhere, BlueprintReadOnly → 蓝图可查看，不可修改
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	//EnemyUIComponent → 敌人的 UI 组件（血条、名字等）。
	UEnemyUIComponent* EnemyUIComponent;

	//VisibleAnywhere, BlueprintReadOnly → 蓝图可查看，不可修改
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	//EnemyHealthWidgetComponent → 显示血条的 WidgetComponent，可附加到骨骼或头顶位置。
	UWidgetComponent* EnemyHealthWidgetComponent;
private:
	//用于初始化敌人的初始属性，例如血量、攻击力、装备等
	void InitEnemyStartUpData();
	
public:
	//提供快速访问战斗组件的方法，内联函数，提高效率
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};
