// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UBoxComponent;
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

	/*
	 * #if WITH_EDITOR 是 Unreal 的条件编译宏，意思是：
	 * 这段代码只会在 编辑器（Editor）版本 编译时包含，
	 * 不会被打包到最终的游戏（Cooked / Shipping）版本中。
	 * 当你在 Unreal Editor 中编辑蓝图或修改属性时，这个函数会被编译进编辑器模块。
	 * 当你打包成游戏运行时，编译器会忽略掉这一段
	 */
	#if WITH_EDITOR
	//~ Begin UObject Interface.
	//这是 UObject 的一个 虚函数，在编辑器中当某个属性在 Details 面板（或蓝图中）被修改时自动调用
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
	#endif
	
	//VisibleAnywhere, BlueprintReadOnly → 蓝图可查看，不可修改
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	//EnemyCombatComponent → 敌人的战斗逻辑组件。
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UBoxComponent* LeftHandCollisionBox; //Boss左手碰撞体

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName LeftHandCollisionBoxAttachBoneName; //Boss左手碰撞体绑定的mesh的关节点名

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UBoxComponent* RightHandCollisionBox; //Boss右手碰撞体

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName RightHandCollisionBoxAttachBoneName; //Boss右手碰撞体绑定的mesh的关节点名
	
	//VisibleAnywhere, BlueprintReadOnly → 蓝图可查看，不可修改
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	//EnemyUIComponent → 敌人的 UI 组件（血条、名字等）。
	UEnemyUIComponent* EnemyUIComponent;

	//VisibleAnywhere, BlueprintReadOnly → 蓝图可查看，不可修改
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	//EnemyHealthWidgetComponent → 显示血条的 WidgetComponent，可附加到骨骼或头顶位置。
	UWidgetComponent* EnemyHealthWidgetComponent;

	UFUNCTION()
	//当碰撞体重叠时触发的事件
	//用于绑定在碰撞盒上
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
private:
	//用于初始化敌人的初始属性，例如血量、攻击力、装备等
	void InitEnemyStartUpData();
	
public:
	//提供快速访问战斗组件的方法，内联函数，提高效率
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
	FORCEINLINE UBoxComponent* GetLeftHandCollisionBox() const {return LeftHandCollisionBox;}
	FORCEINLINE UBoxComponent* GetRightHandCollisionBox() const {return RightHandCollisionBox;}
};
