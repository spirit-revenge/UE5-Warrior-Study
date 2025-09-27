// Spirit Revenge All Rights Reserved


#include "Controllers/WarriorAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	//使用 ObjectInitializer 将默认的 PathFollowingComponent 替换为 UCrowdFollowingComponent。
	//这样 AI 可以支持 人群避让（Crowd Avoidance），让多个 AI 在移动时不会互相穿透
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	//创建视觉感知组件 UAISenseConfig_Sight。
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	//只感知敌人，不感知友军或中立。
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	//视觉范围 5000 单位。
	AISenseConfig_Sight->SightRadius = 5000.f;
	//LoseSightRadius = 0 表示目标一旦离开视觉范围立即失去感知。
	AISenseConfig_Sight->LoseSightRadius = 0.f;
	//视野角度 360°，也就是全方位视野。
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

	//创建感知系统组件 UAIPerceptionComponent
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	//配置视觉感知为主感知
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	//绑定感知更新事件 OnTargetPerceptionUpdated，每当目标进入或离开感知范围时触发
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	//给 AI Controller 设置一个团队 ID。
	//团队 ID 在后面 GetTeamAttitudeTowards 中用于判断敌友关系。
	AAIController::SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type AWarriorAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	//获取检测用的Pawn
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	//获取Pawn的controller
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());

	//如果目标 Controller 的团队 ID 小于自己的团队 ID，则为 Hostile。
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}

	//否则视为友好
	return ETeamAttitude::Friendly;
}

void AWarriorAIController::BeginPlay()
{
	Super::BeginPlay();

	//获取 CrowdFollowingComponent。
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		//根据设置开启/关闭人群避让。
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState ::Disabled);

		//配置避让质量
		switch (DetourCrowdAvoidanceQuality)
		{
		case 1: CrowdComp->SetCrowdAvoidanceQuality( ECrowdAvoidanceQuality::Low); break;
		case 2: CrowdComp->SetCrowdAvoidanceQuality( ECrowdAvoidanceQuality::Medium); break;
		case 3: CrowdComp->SetCrowdAvoidanceQuality( ECrowdAvoidanceQuality::Good); break;
		case 4: CrowdComp->SetCrowdAvoidanceQuality( ECrowdAvoidanceQuality::High); break;
		default: break;	
		}

		//避让组
		CrowdComp->SetAvoidanceGroup(1);
		CrowdComp->SetGroupsToAvoid(1);
		//碰撞检测范围。
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	//获取黑板组件
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		//如果黑板的 TargetActor 为空
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
		{
			//如果目标被成功感知，则将其写入黑板
			if (Stimulus.WasSuccessfullySensed() && Actor)
			{
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
	}
	
}
