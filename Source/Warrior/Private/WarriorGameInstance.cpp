// Spirit Revenge All Rights Reserved


#include "WarriorGameInstance.h"
#include "MoviePlayer.h"

void UWarriorGameInstance::Init()
{
	//调用父类 UGameInstance::Init()；
	//引擎在游戏启动时调用一次；
	//保证基础初始化（比如 OnlineSubsystem、EngineSubsystem 初始化）被正确执行
	Super::Init();

	//绑定一个 全局委托（delegate），当 任何地图开始加载之前 被调用
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
	//当地图加载完成并创建出 UWorld* 对象后触发；
	//用于执行“加载完成”后的逻辑，比如关闭加载界面
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);
}

void UWarriorGameInstance::OnPreLoadMap(const FString& MapName)
{
	//创建一个加载界面的配置结构体；
	//由 MoviePlayer 模块使用；
	//用于定义加载界面的显示方式、持续时间、UI内容等。
	FLoadingScreenAttributes LoadingScreenAttributes;
	//意思是：当地图加载完成后，加载界面会自动消失；
	//如果设为 false，则必须手动调用 StopMovie() 来关闭。
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	//表示加载界面最少显示 2 秒；
	//即使地图很快加载完成，也不会立即关闭；
	//防止“闪一下”影响体验。
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.f;
	//设置加载界面的显示内容；
	//NewTestLoadingScreenWidget() 是引擎内置的一个测试用默认UI（白色背景+“Loading...”）；
	//之后可以自定义自己的 UMG Widget 来替换。
	LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	//使用引擎的 MoviePlayer 系统来注册并启动加载界面；
	//MoviePlayer 是 Unreal 的底层系统之一，用于在加载过程中播放过场动画或显示静态 UI；
	//注意：这个系统只在“非 PIE（Play In Editor）模式”下有效；
	//在编辑器里调试时，通常不会看到 Loading Screen。
	GetMoviePlayer() -> SetupLoadingScreen(LoadingScreenAttributes);
}

void UWarriorGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	//当新地图加载完成后调用；
	//停止当前的 MoviePlayer，即关闭加载界面；
	//参数 LoadedWorld 就是刚加载好的 UWorld（可用来执行一些初始化操作）。
	GetMoviePlayer() -> StopMovie();
}

TSoftObjectPtr<UWorld> UWarriorGameInstance::GetGameLevelByTag(FGameplayTag InTag) const
{
	//遍历 GameInstance 中配置的所有地图项。
	for (const FWarriorGameLevelSet& GameLevelSet : GameLevelSets)
	{
		//跳过未配置完整的地图（即无 Tag 或 Level 的项）；
		//防止访问空指针或加载错误。
		if (!GameLevelSet.IsValid()) continue;

		//找到与传入 GameplayTag 匹配的项；
		//== 运算符可直接比较 FGameplayTag。
		if (GameLevelSet.LevelTag == InTag)
		{
			//返回对应地图的软引用（TSoftObjectPtr<UWorld>）；
			//可以用 UGameplayStatics::OpenLevelBySoftObjectPtr() 打开该地图。
			return GameLevelSet.Level;
		}
	}

	//没找到时返回空引用。
	return TSoftObjectPtr<UWorld>();
}
