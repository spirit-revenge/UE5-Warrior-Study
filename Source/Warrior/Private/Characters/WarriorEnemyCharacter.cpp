// Spirit Revenge All Rights Reserved


#include "Characters/WarriorEnemyCharacter.h"

#include "WarriorFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpDataBase.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"

AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	//AI 自动控制：AutoPossessAI = PlacedInWorldOrSpawned，确保敌人生成时由 AI Controller 控制
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//旋转设置：禁止 Pawn 跟随 Controller 旋转，使用自定义旋转逻辑
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//bOrientRotationToMovement = false → 禁止自动面向移动方向。
	//RotationRate、MaxWalkSpeed、BrakingDecelerationWalking 控制敌人移动行为
	GetCharacterMovement() -> bUseControllerDesiredRotation = false;
	GetCharacterMovement() -> bOrientRotationToMovement = false;
	GetCharacterMovement() -> RotationRate = FRotator(0.f,180.f,0.f);
	GetCharacterMovement() -> MaxWalkSpeed = 300.f;
	GetCharacterMovement() -> BrakingDecelerationWalking = 1000.f;

	//创建EnemyCombatComponent子组件
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");

	//创建EnemyUIComponent子组件
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");
	
	//创建EnemyHealthWidgetComponent子组件
	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	//附加到骨骼
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox -> SetupAttachment(GetMesh());
	LeftHandCollisionBox -> SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	LeftHandCollisionBox -> OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox -> SetupAttachment(GetMesh());
	RightHandCollisionBox -> SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	RightHandCollisionBox -> OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
}

UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	//返回EnemyCombatComponent战斗组件
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	//返回EnemyUIComponent UI组件
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	//返回EnemyUIComponent UI组件
	return EnemyUIComponent;
}

//初始化 Widget
void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//当游戏开始时，获取 EnemyHealthWidgetComponent 的 Widget 对象并初始化
	if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		//调用 InitEnemyCreateWidget(this)，会触发蓝图事件 BP_OnOwningEnemyUIComponentInitialized，绑定 UI 组件数据（血量、名字等）
		HealthWidget->InitEnemyCreateWidget(this);
	}
}

///当 AI 控制器接管 Pawn 时调用，触发 敌人初始数据加载
void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitEnemyStartUpData();
}

#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox -> AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox -> AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
}
#endif

void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent -> OnHitTargetActor(HitPawn);
		}
	}
}

//异步加载启动数据
void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	//软引用资源 CharacterStartUpData 异步加载，节省内存，避免一次性加载大量敌人数据
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if(UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					//加载完成后，将数据应用到 WarriorAbilitySystemComponent（属性/技能初始化）
					LoadedData -> GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);

					//Debug::Print(TEXT("Enemy Start Up Data Loaded"),FColor::Green);
				}
			}
		)
	);
}
