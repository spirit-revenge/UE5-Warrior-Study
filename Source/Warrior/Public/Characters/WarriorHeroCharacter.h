// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:
	AWarriorHeroCharacter();//构造函数
	
protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;//在关卡被加载的时候执行且仅运行时被调用
	
	
private:
#pragma region Components
	//UPROPERTY宏定义属性元数据和变量说明符
	//设为任何地方可见，蓝图只读，种类为相机
	//AllowPrivateAccess表示标记为 BlueprintReadOnly 或 BlueprintReadWrite 的私有成员应该可以从蓝图中访问
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera",meta=(AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;//弹簧组件，

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera",meta=(AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;//角色跟随相机
#pragma endregion

#pragma region Inputs
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
#pragma endregion
	
};
