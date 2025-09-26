// Spirit Revenge All Rights Reserved


#include "WarriorFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	//保证传入的 Actor 不为空。
	check(InActor);

	//CastChecked 会在类型转换失败时报错，非常适合“必须存在”的情况
	//直接用 UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent，兼容任何实现了 IAbilitySystemInterface 的 Actor。
	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	//获取Actor伤的AbilitySystemComponent
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	//检查 Actor 是否已有该 Tag
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		//如果没有，就通过 ASC 添加一个 LooseGameplayTag
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	//判断是否有该Tag
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		//如果有，移除这个 Tag
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	//判断是否有该Tag
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	//利用 ExpandEnumAsExecs 直接给蓝图提供 Yes/No 执行流
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck)?EWarriorConfirmType::Yes:EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	//检查是否空指针
	check(InActor);

	//判断这个 Actor 有没有实现 IPawnCombatInterface 接口。
	//如果实现了，就通过接口调用 GetPawnCombatComponent() 返回组件
	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	//蓝图版本封装了C++版的函数的调用，并且根据返回值设置 OutValidType。
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;

	//返回值仍然是 CombatComponent，这样蓝图里可以直接拿到引用。
	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

