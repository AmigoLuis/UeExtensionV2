#pragma once
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace SuperManager
{
	enum ELogLevel : uint8
	{
		/** Always prints a fatal error to console (and log file) and crashes (even if logging is disabled) */
		Fatal = 1,

		/** 
		 * Prints an error to console (and log file). 
		 * Commandlets and the editor collect and report errors. Error messages result in commandlet failure.
		 */
		Error,

		/** 
		 * Prints a warning to console (and log file).
		 * Commandlets and the editor collect and report warnings. Warnings can be treated as an error.
		 */
		Warning,

		/** Prints a message to console (and log file) */
		Display,

		/** Prints a message to a log file (does not print to console) */
		Log,

		/** 
		 * Prints a verbose message to a log file (if Verbose logging is enabled for the given category, 
		 * usually used for detailed logging) 
		 */
		Verbose,

		/** 
		 * Prints a verbose message to a log file (if VeryVerbose logging is enabled, 
		 * usually used for detailed logging that would otherwise spam output) 
		 */
		VeryVerbose
	};
}

inline void PrintInLog(const FString& Message, SuperManager::ELogLevel LogLevelLocal = SuperManager::ELogLevel::Warning)
{
	switch (LogLevelLocal)
	{
	case SuperManager::ELogLevel::Fatal:
		UE_LOG(LogTemp, Fatal, TEXT("%s"), *Message);
		break;
	case SuperManager::ELogLevel::Error:
		UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
		break;
	case SuperManager::ELogLevel::Warning:
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		break;
	case SuperManager::ELogLevel::Display:
		UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
		break;
	case SuperManager::ELogLevel::Log:
		UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
		break;
	case SuperManager::ELogLevel::Verbose:
		UE_LOG(LogTemp, Verbose, TEXT("%s"), *Message);
		break;
	case SuperManager::ELogLevel::VeryVerbose:
		UE_LOG(LogTemp, Verbose, TEXT("%s"), *Message);
		break;
	}
}


inline void PrintDebugMessageOnScreen(const FString& Message,
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
		PrintInLog(FString::Printf(TEXT("GEngine is nullptr in Function:%s"), TEXT(__FUNCTION__)));
	}
}

inline EAppReturnType::Type ShowMessageDialog(const FString& Message, const EAppMsgType::Type MsgType,
                                              const bool bShowMessageAsWarning = true)
{
	if (bShowMessageAsWarning)
	{
		const FText MsgTitle = FText::FromString(TEXT("Warning about Duplicate Assets"));
		return FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

inline void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.bUseLargeFont = true;
	Info.FadeOutDuration = 5.0f;
	Info.FadeInDuration = 1.0f;
	FSlateNotificationManager::Get().AddNotification(Info);
}