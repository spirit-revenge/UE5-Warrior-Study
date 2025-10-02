// Spirit Revenge All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"

#include "EnhancedInputSubsystems.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/SizeBox.h"
#include "Controllers/WarriorHeroController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/WarriorWidgetBase.h"

//开始流程（寻找目标、改变移动速度、添加输入映射），最后调用 Super
void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	//尝试锁定目标
	TryLockOnTarget();
	//初始化目标锁定的移动
	InitTargetLockMovement();
	//初始化目标锁定的上下文
	InitTargetLockMappingContext();
	//调用父类
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

//恢复并清理资源（移除 widget、mapping、重置速度），然后 Super
void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	//重置目标锁定移动
	ResetTargetLockMovement();
	//重置目标锁定的上下文
	ResetTargetLockMappingContext();
	//清空能力
	CleanUp();
	//调用父类
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	//判断
	if (!CurrentLockedActor || //判断当前的锁定目标是否存在
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor, WarriorGameplayTags::Shared_Status_Dead) || //判断锁定的对象已死亡
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Dead) //hero已死亡
		)
	{
		CancelTargetLockAbility(); //取消能力
		return; //直接返回
	}

	SetTargetLockWidgetPosition(); //设置锁定目标的widget组件

	const bool bShouldOverrideRotation = //是否应该覆盖转向器，
		// hero没有死亡
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Rolling)
	&&
		//并且hero没有阻挡攻击
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Blocking);

	if (bShouldOverrideRotation) //如果可以覆盖转向器
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation( //找到转向的方向
	GetHeroCharacterFromActorInfo() -> GetActorLocation(), //hero的位置
	CurrentLockedActor -> GetActorLocation() //当前目标的位置
		);

		LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f); //减去相机偏移量，防止hero挡住镜头

		//得到当前的目标的
		const FRotator CurrentControlRot = GetHeroControllerFromActorInfo() -> GetControlRotation();
		//计算目标的旋转，使角色平滑面向目标
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime, TargetLockRotationInterpSpeed);
		//改变控制器旋转，本地玩家侧的操作
		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		//把角色真正的 actor yaw 对齐
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
		
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	//重新收集当前可锁定目标
	GetAvailableActorsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock;

	//得到左右两组候选
	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);

	//如果是朝左边旋转，新对象为左边
	if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else //如果是朝右边旋转，新对象为右边
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}

	if (NewTargetToLock) //如果有新对象，则设置当前锁定对象为新对象
	{
		CurrentLockedActor = NewTargetToLock;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	//得到所有可用对象
	GetAvailableActorsToLock();

	if (AvailableActorsToLock.IsEmpty()) //没有可锁定对象
	{
		CancelTargetLockAbility(); //取消能力
		return; //直接返回
	}

	CurrentLockedActor = GetNearestTargetFromAvailableActors(AvailableActorsToLock); //设置当前对象为最近目标

	if (CurrentLockedActor) //如果有当前锁定目标
	{
		//绘制锁定widget组件
		DrawTargetLockWidget(); 
		//设定widget的位置
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility(); //否则取消能力
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty(); //将可用的目标全部清空
	
	TArray<FHitResult> BoxTraceHits; //用于存储所有追着检测

	//检测多个目标
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(), //hero对象
		GetHeroCharacterFromActorInfo()->GetActorLocation(), //hero的位置
		// 最终目标位置
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetAvatarActorFromActorInfo() -> GetActorForwardVector() * BoxTraceDistance,
		TraceBoxSize / 2.f, //设置检测范围大小
		GetAvatarActorFromActorInfo() -> GetActorForwardVector().ToOrientationRotator(), // 盒子朝向和 Actor 一致
		BoxTraceChannel, //要检测的对象类型
		false, // 是否复杂碰撞
		TArray<AActor*>(), // 忽略的Actor
		bShowPersistentDebugShape ?  EDrawDebugTrace::Persistent : EDrawDebugTrace::None, //调试可视化
		BoxTraceHits, //返回的多个命中结果
		true // 忽略自身
	);

	for (const FHitResult& TraceHit : BoxTraceHits) //循环遍历所有最终的目标
	{
		if (AActor* HitActor = TraceHit.GetActor()) //如果追踪的目标存在
		{
			if (HitActor != GetHeroCharacterFromActorInfo()) //不等于hero
			{
				AvailableActorsToLock.AddUnique(HitActor); //将目标唯一存入所有可锁定的目标
			}
		}  
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f; //存储最近距离
	//返回发现的最近目标，（hero位置，所有可用目标，最近距离）
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo() -> GetActorLocation(), InAvailableActors, ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty()) //如果锁定目标不存在，或者为空
	{
		//取消能力并返回
		CancelTargetLockAbility(); 
		return;
	}

	//hero位置
	const FVector PlayerLocation = GetHeroCharacterFromActorInfo() -> GetActorLocation();
	//玩家到当前锁定目标的方向
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor -> GetActorLocation() - PlayerLocation).GetSafeNormal();

	//遍历
	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		//如果当前可用对象不存在或者为hero则结束本次循环
		if (!AvailableActor || AvailableActor == CurrentLockedActor) continue;

		//玩家到可用目标的方向
		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		//计算叉乘
		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		//如果Z轴大于零则加入右边，否则加入左边
		if (CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	//当前widget不存在
	if (!DrawnTargetLockWidget)
	{
		//检查是否在蓝图中赋值
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in blueprint"));
		//创建widget组件
		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);
		//检查是否创建成功
		check(DrawnTargetLockWidget);
		//添加到窗口中
		DrawnTargetLockWidget -> AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	//如果widget组件不存在或锁定目标不存在，则取消能力，返回
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}

	FVector2D ScreenPosition; //存储在屏幕的位置
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition( //将世界坐标转换为屏幕坐标
		GetHeroControllerFromActorInfo(), //hero controller
		CurrentLockedActor -> GetActorLocation(), //当前锁定目标的位置
		ScreenPosition, //屏幕位置
		true //相对于玩家视口子区域
	);

	// 如果widget大小为为ZeroVector
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		//遍历widget tree
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget)) //如果widget存在则转换为USizeBox
				{
					//设置widget的大小为找到的USizeBox大小
					TargetLockWidgetSize.X = FoundSizeBox -> GetWidthOverride(); 
					TargetLockWidgetSize.Y = FoundSizeBox -> GetHeightOverride();
				}
			}
		);
	}

	//减去锁定widget的半径
	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	//设置屏幕位置，已经计算了反向 DPI，将此设置为 false
	DrawnTargetLockWidget -> SetPositionInViewport(ScreenPosition,false);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	//获取默认的最大速度
	CachedDefaultMaxWalkSpeed = GetHeroCharacterFromActorInfo() -> GetCharacterMovement() -> MaxWalkSpeed;

	//修改最大速度
	GetHeroCharacterFromActorInfo() -> GetCharacterMovement() -> MaxWalkSpeed = TargetLockMaxWalkSpeed ;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	//得到本地玩家并转换为UEnhancedInputLocalPlayerSubsystem
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo() -> GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem*  Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	//检查是否存在
	check(Subsystem);

	//添加 TargetLockMappingContext
	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	//调用CancelAbility方法取消锁定能力
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	//设置所有锁定目标数组为空
	AvailableActorsToLock.Empty();

	//当前锁定目标为空
	CurrentLockedActor = nullptr;

	//如果有锁定的widget则移除
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget -> RemoveFromParent();
	}

	//设置锁定的widget为空
	DrawnTargetLockWidget = nullptr;
	//大小为空
	TargetLockWidgetSize = FVector2D::ZeroVector;
	//缓存的默认速度为0
	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	//如果缓存的默认速度大于0
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		//重置为默认速度
		GetHeroCharacterFromActorInfo() -> GetCharacterMovement() -> MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	//hero controller不存在则返回
	if (!GetHeroControllerFromActorInfo())
	{
		return;
	}

	//从输入增强系统中移除TargetLockMappingContext
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo() -> GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem*  Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem -> RemoveMappingContext(TargetLockMappingContext);
}
