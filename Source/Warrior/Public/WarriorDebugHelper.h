// Spirit Revenge All Rights Reserved
#pragma once

//用于项目DEBUG输出
namespace Debug
{
	static void Print(const FString& Msg,const FColor& Color = FColor::MakeRandomColor(),int32 InKey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey,7,Color,Msg);//将日志信息打印在屏幕上

			UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);//将格式化消息记录到日志文件中的宏
		}
	}
}
 