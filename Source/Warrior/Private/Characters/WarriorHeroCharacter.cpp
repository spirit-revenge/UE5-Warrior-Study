// Spirit Revenge All Rights Reserved


#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WarriorInputComponent.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "DataAssets/StartUpData/DataAsset_HeroStartUpData.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "WarriorDebugHelper.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HAL/Platform.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	//设置胶囊体组件的尺寸，这是角色的碰撞体，决定了角色能否穿过门、与地面交互等
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);

	//禁止角色直接使用控制器的旋转输入
	//常见于第三人称角色设置，让摄像机自由旋转，而不是角色跟着转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//是一个 弹簧臂（Spring Arm），用于把摄像机拉远，形成第三人称视角
	//CreateDefaultSubobject创建一个组件或子对象，该对象将在该类的所有实例中被实例化。
	//USpringArmComponent此组件尝试保持其子对象与父对象之间的固定距离，但如果发生碰撞，则会收回子对象，并在没有碰撞时弹回。
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//SetupAttachment初始化所需的附加父级和SocketName，在组件注册时进行附加。通常应该在其拥有者Actor的构造函数中调用，并且应优先于在组件未注册时使用AttachToComponent。
	//GetRootComponent返回这个Actor的根组件。
	CameraBoom->SetupAttachment(GetRootComponent());
	//摄像机距离角色 200cm
	CameraBoom->TargetArmLength = 200.f;
	//让摄像机稍微偏右上，这样玩家不会被角色模型挡住视线
	CameraBoom->SocketOffset = FVector(0.f,55.f,65.f);
	//摄像机会跟随玩家控制器的旋转（比如鼠标移动）
	CameraBoom->bUsePawnControlRotation = true;

	//创建摄像机组件并附加到 CameraBoom（弹簧臂）的末端
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	//摄像机不会直接使用控制器旋转
	FollowCamera->bUsePawnControlRotation = false;

	//角色将朝着移动方向自动旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//设置旋转速率
	GetCharacterMovement()->RotationRate = FRotator(0.f,500.f,0.f);
	//设置角色的最大行走速度
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	//设置行走时的减速力，数值越大，角色停得越快
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if(UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
	
	/*
	if (WarriorAbilitySystemComponent && WarriorAttributeSet)
	{
		const FString ASCText = FString::Printf(TEXT("Owner Actor: %s, AvatarActor: %s"),
			*WarriorAbilitySystemComponent->GetOwnerActor()->GetActorLabel(),
			*WarriorAbilitySystemComponent->GetAvatarActor()->GetActorLabel());
		Debug::Print(TEXT("Ability system component valid.") + ASCText,FColor::Green);
		Debug::Print(TEXT("AttributeSet valid.") + ASCText,FColor::Green); 
	}
	*/
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"));
	
	ULocalPlayer* LocalPlayer= GetController<APlayerController>()->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);

	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTags_Move,ETriggerEvent::Triggered,this,&ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTags_Look,ETriggerEvent::Triggered,this,&ThisClass::Input_Look);
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotator(0.f,Controller->GetControlRotation().Yaw,0.f);

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotator.RotateVector(FVector::ForwardVector);
		UE_LOG(LogTemp,Warning,TEXT("XX"));
		AddMovementInput(ForwardDirection,MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotator.RotateVector(FVector::RightVector);
		
		AddMovementInput(RightDirection,MovementVector.X);
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector =  InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		//此处可设置为负值，即Y轴反转，更符合操作习惯
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

