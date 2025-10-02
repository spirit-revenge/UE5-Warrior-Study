// Spirit Revenge All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "WarriorProjectileBase.generated.h"

UENUM(BlueprintType) //允许在蓝图里使用这个枚举
enum class EProjectileDamagePolicy : uint8
{
	OnHit, //当投射物 击中某个物体或角色 时触发伤害
	OnBeginOverlap //当投射物 开始重叠（碰撞体交集）时触发伤害
};

class UProjectileMovementComponent;
class UBoxComponent;
class UNiagaraComponent;
struct FGameplayEventData;

UCLASS()
class WARRIOR_API AWarriorProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	//构造方法
	AWarriorProjectileBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UBoxComponent* ProjectileCollisionBox; //投射物的碰撞体，用于检测击中或重叠事件

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent; //粒子特效组件，用于投射物视觉效果（火焰、光束等）

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComp; //控制投射物运动（速度、重力、旋转等）

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	EProjectileDamagePolicy ProjectileDamagePolicy = EProjectileDamagePolicy::OnHit; //指定投射物什么时候触发伤害

	UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (ExposeOnSpawn = "true"))
	//GAS 的伤害效果（GameplayEffect）实例，可以在 Spawn 时指定，用于不同武器/技能的伤害
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle; 
	
	UFUNCTION()
	//当投射物撞击物体（Hit）时调用
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	//当投射物进入碰撞体（Overlap）时调用
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//在蓝图里实现投射物击中时的视觉或音效效果。
	//参数：HitLocation，击中位置。
	//蓝图实现无需 C++ 覆盖，设计师可以直接拖粒子/音效
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Spawn Projectile Hit FX"))
	void BP_OnSpawnProjectileHitFX(const FVector& HitLocation);
	
private:
	//核心伤害逻辑封装在这里：
	//输入击中的 Pawn 和伤害数据。
	//统一处理 GAS 的 GameplayEvent 或 ApplyGameplayEffectSpecToTarget。
	//封装好逻辑，避免在 Hit/Overlap 里重复调用。
	void HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload);
};
