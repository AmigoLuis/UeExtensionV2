#include "FBlueprintAssetRenameHandler.h"

#include "CheckAndLogAndReturn.h"
#include "CustomUtilities.h"
#include "EditorUtilityLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "../Settings/BPDefaultNameSettings.h"

bool FBlueprintAssetRenameHandler::IsBlueprintAsset(const FAssetData& AssetData)
{
    // 方法1：检查资产类名
    const FString AssetClassName = AssetData.AssetClassPath.ToString();
    
    // 蓝图的主要类名
    const static FString& BlueprintClassName = TEXT("Blueprint");
    
    if (AssetClassName.Contains(BlueprintClassName))
    {
        return true;
    }
    
    // 方法2：检查对象是否是蓝图 // 开销较大，暂时不用
    // UObject* AssetObject = AssetData.GetAsset();
    // if (AssetObject)
    // {
    //     if (Cast<UBlueprint>(AssetObject) || Cast<UBlueprintGeneratedClass>(AssetObject))
    //     {
    //         return true;
    //     }
    // }
    
    return false;
}

void FBlueprintAssetRenameHandler::AnalyzeParentClass(const FAssetData& AssetData, 
    UBlueprint* Blueprint, UClass* ParentClass)
{
    LOG_ENTER_FUNCTION();
    if (!IsValid(AssetData.GetAsset()))
    {
        PrintInLog(TEXT("Invalid asset: ") + AssetData.AssetName.ToString());
        return;
    }
    if (!IsAssetAbleToRename(AssetData))
    {
        PrintInLog(TEXT("Unable to rename asset."));
        return;
    }
    // 蓝图没有父类（这是ai写的代码，这可能吗。。）
    CHECK_NULL_RETURN(ParentClass);
    
    const FString ParentClassName = ParentClass->GetName();
    const FString ParentClassPath = ParentClass->GetPathName();
    
    PrintInLog(TEXT("父类名称: ") + ParentClassName, Display);
    PrintInLog(TEXT("父类路径: ") + ParentClassPath, Display);
    
    // 判断是C++类还是蓝图类
    if (IsBlueprintGeneratedClass(ParentClass))
    {
        PrintInLogDisplay(TEXT("父类为蓝图类。"));
        RenameAssetDerivedFromBlueprint(AssetData, ParentClassName);
        {// 这段代码没有啥用，后面可以删除，目前用来看父类的一些信息
#if 0
            // 获取蓝图生成的类
            if (const UBlueprintGeneratedClass* BlueprintParentClass = Cast<UBlueprintGeneratedClass>(ParentClass))
            {
                // 尝试获取源蓝图
                if (const UBlueprint* SourceBlueprint = Cast<UBlueprint>(BlueprintParentClass->ClassGeneratedBy))
                {
                    PrintInLogVerbose(TEXT("父类蓝图名称: ") + SourceBlueprint->GetName());
                    PrintInLogVerbose(TEXT("父类蓝图路径: ") + SourceBlueprint->GetPathName());
                }
            }
#endif
        }
    }
    else
    {
        PrintInLogDisplay(TEXT("父类不是蓝图类，假设为C++原生类。"));
        RenameAssetDerivedFromCPP(AssetData, ParentClassName);
        {
            // 这段代码没有啥用，后面可以删除，目前用来看父类的一些信息
            if (bool HasAnyClassNativeFlags = ParentClass->HasAnyClassFlags(CLASS_Native))
            {
                PrintInLogDisplay(TEXT("父类很可能是C++原生类，具有原生类标签: ") SYMBOL_NAME_TEXT(CLASS_Native));
            }
            else
            {
                PrintInLogDisplay(TEXT("父类不具有原生类标签: ") SYMBOL_NAME_TEXT(CLASS_Native));
            }
        }
    }
}

bool FBlueprintAssetRenameHandler::IsBlueprintGeneratedClass(UClass* Class)
{
    CHECK_NULL_RETURN_VALUE(Class, false);
    
    // 方法1：检查是否是蓝图生成的类
    if (Class->IsChildOf(UBlueprintGeneratedClass::StaticClass()))
    {
        PrintInLog(TEXT("是蓝图生成的类"));
        return true;
    }
    
    // 方法2：检查类是否由蓝图生成
    if (Class->ClassGeneratedBy && Cast<UBlueprint>(Class->ClassGeneratedBy))
    {
        PrintInLog(TEXT("由蓝图生成"));
        return true;
    }
    
    // 方法3：检查类标志
    if (Class->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
    {
        PrintInLog(TEXT("类标志有蓝图编译类的标志"));
        return true;
    }
    
    return false;
}

static TWeakObjectPtr<UBPDefaultNameSettings> BPDefaultNameSettingsWeak;
TWeakObjectPtr<UBPDefaultNameSettings> FBlueprintAssetRenameHandler::GetOrNewBPDefaultNameSettingsWeakPtr()
{
    if (BPDefaultNameSettingsWeak == nullptr)
    {
        const UBPDefaultNameSettings* BPDefaultNameSettingsPtr = GetDefault<UBPDefaultNameSettings>();
        CHECK_NULL_RETURN_VALUE(BPDefaultNameSettingsPtr, BPDefaultNameSettingsWeak);
        BPDefaultNameSettingsWeak = NewObject<UBPDefaultNameSettings>();
        BPDefaultNameSettingsWeak->CopySettingsValues(BPDefaultNameSettingsPtr);
    }
    return BPDefaultNameSettingsWeak;
}

void FBlueprintAssetRenameHandler::RegisterBlueprintAssetRenameHandler()
{
}

void FBlueprintAssetRenameHandler::UnregisterBlueprintAssetRenameHandler()
{
    BPDefaultNameSettingsWeak.Reset();
}

// 如果是蓝图就处理，返回值表示是否处理过
bool FBlueprintAssetRenameHandler::ProcessAssetIfIsBlueprint(const FAssetData& AssetData)
{
    LOG_ENTER_FUNCTION();
    if (!GetOrNewBPDefaultNameSettingsWeakPtr()->bEnablePlugin)
    {
        PrintInLogDisplay(TEXT("Plugin not enabled, ") 
            TEXT(__FUNCTION__) TEXT(" ignored."));
        return false;
    }
    constexpr bool bAssetNotProcessed = false;
    constexpr bool bAssetProcessed = true;
    // 打印资产路径
	
    // 判断是否是蓝图，不是蓝图的话直接返回，是蓝图的话，打印蓝图父类信息：是cpp中的类还是蓝图中的类，打印父类名称
    // 资产类名
    const FString AssetClassName = AssetData.AssetClassPath.ToString();
    PrintInLog(TEXT("资产类型: ") + AssetClassName, Display);
    
    // 判断是否是蓝图
    if (!IsBlueprintAsset(AssetData))
    {
        PrintInLog(TEXT("不是蓝图资产，跳过处理"), Display);
        return bAssetNotProcessed;
    }
	
    PrintInLog(TEXT("这是蓝图资产，开始分析父类信息"), Display);
    // 打印资产路径
    const FString AssetPath = AssetData.GetObjectPathString();
    const FString AssetName = AssetData.AssetName.ToString();
    const FString PackagePath = AssetData.PackagePath.ToString();
    PrintInLog(TEXT("资产名称: ") + AssetName, Display);
    PrintInLog(TEXT("资产路径: ") + AssetPath, Display);
    PrintInLog(TEXT("包路径: ") + PackagePath, Display);
    
    // 获取蓝图资产对象
    UObject* AssetObject = AssetData.GetAsset();
    // "无法获取蓝图资产对象，可能正在加载中"
    CHECK_NULL_RETURN_VALUE(AssetObject, bAssetProcessed);
    
    // 可能不是有效的蓝图资产
    UBlueprint* Blueprint = Cast<UBlueprint>(AssetObject);
    CHECK_NULL_RETURN_VALUE(Blueprint, bAssetProcessed);
    
    // 获取父类
    UClass* ParentClass = Blueprint->ParentClass;
    // 蓝图没有父类（这是ai写的代码，这可能吗。。）
    CHECK_NULL_RETURN_VALUE(ParentClass, bAssetProcessed);
    
    // 分析父类信息
    AnalyzeParentClass(AssetData, Blueprint, ParentClass);
    return bAssetProcessed;
}

// TODO: 实现批量分析功能(目前只是ai生成的代码，未实际测试过)
void FBlueprintAssetRenameHandler::AnalyzeAllBlueprintsInPath(const FString& Path)
{

	const FAssetRegistryModule* AssetRegistryModulePtr = LoadModulePtrWithLog<FAssetRegistryModule>("AssetRegistry");
    CHECK_NULL_RETURN(AssetRegistryModulePtr);
	FARFilter Filter;
	Filter.PackagePaths.Add(*Path);
	Filter.bRecursivePaths = true;
    
	// 只查找蓝图资产
	Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UBlueprintGeneratedClass::StaticClass()->GetClassPathName());
    
	TArray<FAssetData> AssetDataList;
	AssetRegistryModulePtr->Get().GetAssets(Filter, AssetDataList);
    
	UE_LOG(LogTemp, Log, TEXT("在路径 %s 中找到 %d 个蓝图"), *Path, AssetDataList.Num());
    
	for (const FAssetData& AssetData : AssetDataList)
	{
		// OnAssetCreated(AssetData);
	}
}

bool FBlueprintAssetRenameHandler::IsAssetAbleToRename(const FAssetData& AssetData)
{
    // 如果蓝图资产是以默认的蓝图名称开头，则可重命名资产
    const FString AssetName = AssetData.AssetName.ToString();
    return AssetName.StartsWith(GetOrNewBPDefaultNameSettingsWeakPtr()->DefaultBlueprintName);
}

bool FBlueprintAssetRenameHandler::RenameAssetDerivedFromBlueprint(const FAssetData& AssetData, const FString& ParentClassName)
{
    return RenameAssetDerivedFromCPP(AssetData, ParentClassName);
}

bool FBlueprintAssetRenameHandler::RenameAssetDerivedFromCPP(const FAssetData& AssetData, const FString& ParentClassName)
{
    const auto bUseBlueprintPrefix = GetOrNewBPDefaultNameSettingsWeakPtr()->bUseBlueprintPrefix;
    const auto& BlueprintPrefix = 
        bUseBlueprintPrefix ? GetOrNewBPDefaultNameSettingsWeakPtr()->BlueprintPrefix : FString();
    const auto bShowRenameNotification = GetOrNewBPDefaultNameSettingsWeakPtr()->bShowRenameNotification;
    const auto& NamingPattern = GetOrNewBPDefaultNameSettingsWeakPtr()->NamingPattern;
    const auto UMaxIntSuffixValue = GetOrNewBPDefaultNameSettingsWeakPtr()->UMaxIntSuffixValue;
    
    for (uint32 SameNameAssetCount = 0; SameNameAssetCount < UMaxIntSuffixValue; ++SameNameAssetCount)
    {
        const FString& NewAssetName = FString::Format(*NamingPattern, 
            {{TEXT("Prefix"), BlueprintPrefix},
                {TEXT("ParentClassName"),ParentClassName},
                {TEXT("IntSuffix"),SameNameAssetCount}});
        
        const FString& NewAssetFullName = AssetData.PackagePath.ToString() + TEXT("/")
            + NewAssetName + TEXT(".") + NewAssetName;
        
        if (UEditorAssetLibrary::DoesAssetExist(NewAssetFullName)) continue;
        
        PrintInLog(FString::Format(TEXT("Try to rename asset :{0} to new name :{1}, "
                                        "asset new path {2} doesn't exist."), 
            {AssetData.AssetName.ToString(), NewAssetName, NewAssetFullName}));
        CHECK_NULL_RETURN_VALUE(AssetData.GetAsset(), false);
        UEditorUtilityLibrary::RenameAsset(AssetData.GetAsset(), NewAssetName);
        FixUpRedirectors();
        if (bShowRenameNotification)
        {
            ShowNotifyInfo(TEXT("Blueprint asset renamed, new asset name: ") + NewAssetName);
        }
        return true;
    }
    if (bShowRenameNotification)
    {
        ShowMessageDialog(FString::Format(
            TEXT("Rename blueprint asset failed, too many same-name assets exit, reached ") 
            SYMBOL_NAME_TEXT(UMaxIntSuffixValue) TEXT(":{0}"), {UMaxIntSuffixValue}));
    }
    return false;
}
