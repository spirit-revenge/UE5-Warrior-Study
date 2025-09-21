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
#include "Components/UI/HeroUIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/UI/HeroUIComponent.h"
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

	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));

}

UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* AWarriorHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;	
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	//调用父类
	Super::PossessedBy(NewController);

	//如果角色的初始数据不为空
	if (!CharacterStartUpData.IsNull())
	{
		//软引用，运行时同步加载
		if(UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			//GiveToAbilitySystemComponent() 会把该角色需要的初始技能、被动 Buff、属性应用到 AbilitySystemComponent 中。
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

//游戏开始时调用
void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//游戏开始时调用
void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//检测存放输入配置的 DataAsset是否存在
	checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"));

	// 获取本地玩家和 EnhancedInput 系统
	ULocalPlayer* LocalPlayer= GetController<APlayerController>()->GetLocalPlayer();

	//Enhanced Input 插件的子系统，负责管理输入映射
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	//保证 Enhanced Input 系统存在，否则游戏直接崩溃报错
	check(Subsystem);

	//注册默认的输入映射表（Mapping Context）
	//优先级设置为 0，意味着这是基础映射，可以叠加其他输入配置（比如 UI 交互模式）
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);

	//把引擎默认的 UInputComponent 转换为你自定义的 UWarriorInputComponent
	//CastChecked 如果转换失败直接崩溃，保证组件一定是我们期望的类型
	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	//绑定输入动作
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTags_Move,ETriggerEvent::Triggered,this,&ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTags_Look,ETriggerEvent::Triggered,this,&ThisClass::Input_Look);
	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset,this,&ThisClass::Input_AbilityInputPressed,&ThisClass::Input_AbilityInputReleased);
}

//
void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue) //InputActionValue 是 Enhanced Input 系统传来的值，包含了玩家输入的强度
{
	// 从输入事件中取出二维向量
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	// 获取控制器当前朝向，只取 Yaw（水平旋转），忽略 Pitch/Roll
	const FRotator MovementRotator(0.f,Controller->GetControlRotation().Yaw,0.f);

	// 如果Y轴方向移动不为0
	if (MovementVector.Y != 0.f)
	{
		// 计算角色前方的世界方向
		//FVector::ForwardVector 是 (X=1,Y=0,Z=0)，表示世界坐标系的前方。
		//RotateVector 根据相机朝向旋转，得到相机前方。
		const FVector ForwardDirection = MovementRotator.RotateVector(FVector::ForwardVector);
		//UE_LOG(LogTemp,Warning,TEXT("XX"));
		//AddMovementInput 通知 CharacterMovementComponent 进行加速度更新
		AddMovementInput(ForwardDirection,MovementVector.Y);
	}

	// 如果X轴方向移动不为0
	if (MovementVector.X != 0.f)
	{
		//FVector::RightVector 是 (X=0,Y=1,Z=0)，表示世界坐标系的右方。
		//同理，通过旋转得到相机右方，再添加移动输入
		const FVector RightDirection = MovementRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection,MovementVector.X);
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	// 获取鼠标/摇杆的视角变化量
	const FVector2D LookAxisVector =  InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		// 控制水平旋转（左右看）
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		//// 控制垂直旋转（抬头/低头）
		//此处可设置为负值，即Y轴反转，更符合操作习惯
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	//触发输入
	WarriorAbilitySystemComponent->OnAbilityInputPressed( InInputTag );
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased( InInputTag );
}

