// Spirit Revenge All Rights Reserved


#include "Items/WarriorProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AWarriorProjectileBase::AWarriorProjectileBase()
{
	//不需要每帧 Tick → 节省性能
	PrimaryActorTick.bCanEverTick = false;
	//碰撞盒作为根组件，用于检测击中或重叠事件
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	//初始碰撞类型为 QueryOnly（只查询，不参与物理模拟）。
	//默认阻挡 Pawn 和世界物体
	ProjectileCollisionBox -> SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	//注册 Hit 和 BeginOverlap 回调事件
	ProjectileCollisionBox -> OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox -> OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);

	//粒子效果附着在根组件上，用于可视化投射物
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent -> SetupAttachment(GetRootComponent());

	//投射物运动组件：控制初始速度、最大速度和方向。
	//ProjectileGravityScale = 0 → 不受重力影响
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp -> InitialSpeed = 700.f;
	ProjectileMovementComp -> MaxSpeed = 900.f;
	ProjectileMovementComp -> Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp -> ProjectileGravityScale = 0.f;

	//投射物 4 秒后自动销毁，避免无限存在
	InitialLifeSpan = 4.f;
}

void AWarriorProjectileBase::BeginPlay()
{
	//调用父类
	Super::BeginPlay();

	//如果伤害策略是 OnBeginOverlap，把碰撞响应改为 Overlap，触发 OnProjectileBeginOverlap
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox -> SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//播放蓝图实现的击中特效
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);
	
	APawn* HitPawn = Cast<APawn>(OtherActor);

	//判断是否为敌对pawn
	if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{
		//只对 敌对 Pawn 生效，否则直接销毁投射物
		Destroy();
		return;
	}

	bool bIsValidBlock = false;
	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn, WarriorGameplayTags::Player_Status_Blocking);

	//检查目标是否处于 阻挡状态
	if (bIsPlayerBlocking)
	{
		//如果是，调用自定义逻辑判断是否有效格挡
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this, HitPawn);
	}

	//创建 GameplayEventData，用于 GAS 事件传递。
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;
	
	if (bIsValidBlock)
	{
		//有效格挡 → 发送 SuccessfulBlock 事件
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else
	{
		//普通击中 → 调用 HandleApplyProjectileDamage 处理伤害
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	//投射物命中后销毁，不再存在世界中
	Destroy();
}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//OverlappedActors 是一个 TArray<AActor*>，用于记录已经被这个弹体命中过的 Actor。
	//有些投射物可能因为碰撞体大或残留帧延迟，导致多次重叠触发。
	//这行代码确保每个目标只处理 一次命中事件
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	//把该 OtherActor 加入命中列表。
	//AddUnique 会自动防止重复添加（内部会先检查 Contains）
	OverlappedActors.AddUnique(OtherActor);

	//尝试把命中的 Actor 转型为 APawn。
	//如果转换成功，说明命中的对象是一个角色
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		//构建一个 FGameplayEventData，用于传递 Gameplay 事件上下文。
		//Instigator：谁发射的这个弹体（通常是角色）。
		//Target：被击中的 Pawn。
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;

		//调用静态函数 IsTargetPawnHostile 判断是否敌对
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			//命中后处理逻辑
			HandleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	//确保 GameplayEffectSpecHandle 有效，如果没有赋值会报错
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forgot to assign a valid spec handle to the projectile: %s"), *GetActorNameOrLabel());

	//调用函数库，将 GAS GameplayEffect 应用到命中的 Pawn
	const bool bWasApply = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	//如果伤害应用成功，发送 HitReact 事件，触发角色受击动画或其他逻辑
	if (bWasApply)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WarriorGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}

