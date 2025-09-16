// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
class UHeroCombatComponent;

/**
 * hero的角色类
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	AWarriorHeroCharacter();//构造函数

	//~ Begin IPawnCombatInterface Interface.
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface
	
protected:
	//~ Begin APawn Interface.
	//当一个 Controller 接管这个 Pawn 时调用
	/*
	 * 这里通常要做：
	 *		初始化 AbilitySystemComponent（父类已经做了）
	 *		应用角色专属的初始技能或属性
	 *		可能进行一些相机或 UI 初始化
	 */
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	//用于绑定输入，比如移动、攻击、释放技能
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//在关卡被加载的时候执行且仅运行时被调用
	virtual void BeginPlay() override;
	
	
private:
#pragma region Components
	//UPROPERTY宏定义属性元数据和变量说明符
	//设为任何地方可见，蓝图只读，种类为相机
	//AllowPrivateAccess表示标记为 BlueprintReadOnly 或 BlueprintReadWrite 的私有成员应该可以从蓝图中访问
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera",meta=(AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;//弹簧组件，

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera",meta=(AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;//角色跟随相机

	//英雄的战斗组件 ，管理攻击、连招、武器等战斗逻辑
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat",meta=(AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;
#pragma endregion

#pragma region Inputs
	//存放输入配置的 DataAsset，可以在蓝图里配置哪些按键对应哪些动作
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;

	//实际执行输入的函数
	//移动输入
	void Input_Move(const FInputActionValue& InputActionValue);
	//镜头移动输入
	void Input_Look(const FInputActionValue& InputActionValue);
	//按键按下输入
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	//按键释放输入
	void Input_AbilityInputReleased(FGameplayTag InInputTag);
	
#pragma endregion

public:
	//函数内联，方便其他类获取 HeroCombatComponent
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};
