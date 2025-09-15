// Spirit Revenge All Rights Reserved


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	//保证传入的 Actor 不为空。
	check(InActor);

	//CastChecked 会在类型转换失败时报错，非常适合“必须存在”的情况
	//直接用 UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent，兼容任何实现了 IAbilitySystemInterface 的 Actor。
	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNane(AActor* InActor, FGameplayTag TagToAdd)
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
