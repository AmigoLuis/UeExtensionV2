#pragma once
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/ThreadManager.h"
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
// 获取任意符号的字符串（无法用于模板）
#define SYMBOL_NAME_STR(x) #x

// 定义日志类别
DECLARE_LOG_CATEGORY_EXTERN(LogGamePlugin, Log, All);
#define M_CUSTOM_LOG_CATEGORY LogGamePlugin

#pragma region WarningAndLearning 
// 被deepseek忽悠了半天，写这个日志打印线程号、时间的功能，因为在ue控制台没看到，结果后面发现在Saved/Logs下面的日志文件，打开后里面每一条日志都有时间、线程号，等于白写，
// 留下这个注释以做警示，后续搞一个功能之前，一定一定要查一下有没有已经存在的功能！！
// 留下这个注释以做警示，后续搞一个功能之前，一定一定要查一下有没有已经存在的功能！！
#define M_LOCAL_LOG_PARAMS_NO_USE TEXT("[%04d-%02d-%02d %02d:%02d:%02d.%03d][%u][%s]%s"), \
	Now.GetYear(), Now.GetMonth(), Now.GetDay(),\
	Now.GetHour(), Now.GetMinute(), Now.GetSecond(),\
	Now.GetMillisecond(), ThreadID, *ThreadName, *Message
inline void PrintInLogNoUse(const FString& Message, 
	const SuperManager::ELogLevel LogLevelLocal = SuperManager::ELogLevel::Warning)
{
	const FDateTime Now = FDateTime::UtcNow(); // 使用UTC时间避免时区问题
	// 获取当前线程id
	const auto ThreadID = FPlatformTLS::GetCurrentThreadId();
	// 获取当前线程名
	const FString ThreadName = FThreadManager::Get().GetThreadName(ThreadID);
	switch (LogLevelLocal)
	{
	case SuperManager::ELogLevel::Fatal:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Fatal, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	case SuperManager::ELogLevel::Error:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Error, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	case SuperManager::ELogLevel::Warning:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Warning, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	case SuperManager::ELogLevel::Display:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Display, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	case SuperManager::ELogLevel::Log:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Log, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	case SuperManager::ELogLevel::Verbose:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Verbose, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	case SuperManager::ELogLevel::VeryVerbose:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, VeryVerbose, M_LOCAL_LOG_PARAMS_NO_USE);
		break;
	}
}
#pragma endregion WarningAndLearning

// 留下这个注释以做警示，后续搞一个功能之前，一定一定要查一下有没有已经存在的功能！！
#define M_LOCAL_LOG_PARAMS TEXT("%s"), *Message
inline void PrintInLog(const FString& Message, 
	const SuperManager::ELogLevel LogLevelLocal = SuperManager::ELogLevel::Warning)
{
	switch (LogLevelLocal)
	{
	case SuperManager::ELogLevel::Fatal:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Fatal, M_LOCAL_LOG_PARAMS);
		break;
	case SuperManager::ELogLevel::Error:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Error, M_LOCAL_LOG_PARAMS);
		break;
	case SuperManager::ELogLevel::Warning:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Warning, M_LOCAL_LOG_PARAMS);
		break;
	case SuperManager::ELogLevel::Display:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Display, M_LOCAL_LOG_PARAMS);
		break;
	case SuperManager::ELogLevel::Log:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Log, M_LOCAL_LOG_PARAMS);
		break;
	case SuperManager::ELogLevel::Verbose:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, Verbose, M_LOCAL_LOG_PARAMS);
		break;
	case SuperManager::ELogLevel::VeryVerbose:
		UE_LOG(M_CUSTOM_LOG_CATEGORY, VeryVerbose, M_LOCAL_LOG_PARAMS);
		break;
	}
}

#define LOG_ENTER_FUNCTION() \
PrintInLog(TEXT("Entered Function: ") TEXT(__FUNCTION__) TEXT("."), SuperManager::ELogLevel::Display);

#define LOG_NULL_PTR(PTR) \
PrintInLog(TEXT(SYMBOL_NAME_STR(PTR)) TEXT(" is nullptr in Function:") TEXT(__FUNCTION__) TEXT("."), SuperManager::ELogLevel::Error);

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
		LOG_NULL_PTR(GEngine);
	}
	PrintInLog(Message, SuperManager::Display);
}

inline EAppReturnType::Type ShowMessageDialog(const FString& Message, const bool bShowMessageAsWarning = true,
                                              const EAppMsgType::Type MsgType = EAppMsgType::Ok)
{
	if (bShowMessageAsWarning)
	{
		const FText MsgTitle = FText::FromString(TEXT("Warning about Duplicate Assets"));
		PrintInLog(Message);
		return FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
	}
	else
	{
		PrintInLog(Message, SuperManager::Display);
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

inline void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.bUseLargeFont = true;
	Info.FadeOutDuration = 5.0f;
	Info.FadeInDuration = 1.0f;
	PrintInLog(Message, SuperManager::Display);
	FSlateNotificationManager::Get().AddNotification(Info);
}
