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

	//FloatValueToPrint：要打印的浮点数值。
	//InKey：唯一 ID，如果不等于 -1，同一个 ID 的消息会被覆盖，而不是重复显示。
	static void Print(const FString& FloatTitle, float FloatValueToPrint, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			//FString::SanitizeFloat() 会把浮点数转成字符串，并去掉非法字符，保证安全。
			const FString FloatMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);

			GEngine->AddOnScreenDebugMessage(InKey,7.f,Color,FloatMsg);

			UE_LOG(LogTemp,Warning,TEXT("%s"),*FloatMsg)
		}
	}
}
 