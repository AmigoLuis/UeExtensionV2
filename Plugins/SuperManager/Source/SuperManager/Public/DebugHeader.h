#pragma once
#include "LogMacros.h"
#include "PrintInLog.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/ThreadManager.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"

FORCEINLINE void PrintDebugMessageOnScreen(const FString& Message,
                                      const FColor& Color = FColor::Green,
                                      const float TimeToDisplay = 8.0f,
                                      const int32 Key = -1)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(Key, TimeToDisplay, Color, Message);
	}
	else
	{
		LOG_NULL_PTR(GEngine);
	}
	PrintInLog(Message, Display);
}

FORCEINLINE EAppReturnType::Type ShowMessageDialog(const FString& Message, const bool bShowMessageAsWarning = true,
                                              const EAppMsgType::Type MsgType = EAppMsgType::Ok)
{
	EAppReturnType::Type AppReturnType;
	if (bShowMessageAsWarning)
	{
		const FText MsgTitle = FText::FromString(TEXT("Warning"));
		AppReturnType = FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
		PrintInLog(Message + TEXT(" User selected:") + LexToString(AppReturnType));
	}
	else
	{
		AppReturnType = FMessageDialog::Open(MsgType, FText::FromString(Message));
		PrintInLog(Message + TEXT(" User selected:") + LexToString(AppReturnType), Display);
	}
	return AppReturnType;
}

FORCEINLINE void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.bUseLargeFont = true;
	Info.FadeOutDuration = 5.0f;
	Info.FadeInDuration = 1.0f;
	PrintInLog(Message, Display);
	FSlateNotificationManager::Get().AddNotification(Info);
}
