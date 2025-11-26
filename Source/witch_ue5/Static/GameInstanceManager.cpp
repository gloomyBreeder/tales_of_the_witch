#include "GameInstanceManager.h"
#include "Static/GameStateManager.h"
#include "GameFramework/GameUserSettings.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/UserWidget.h"

void UGameInstanceManager::Init()
{
	Super::Init();
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGameInstanceManager::OnMapLoaded);

	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);

	const int32 ScreenWidth = DisplayMetrics.PrimaryDisplayWidth;
	const int32 ScreenHeight = DisplayMetrics.PrimaryDisplayHeight;

	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	Settings->SetScreenResolution(FIntPoint(ScreenWidth, ScreenHeight));
	Settings->SetFullscreenMode(EWindowMode::Fullscreen);

	Settings->ApplySettings(false);
}

void UGameInstanceManager::OnMapLoaded(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
	{
		return;
	}
	const FString realName = UWorld::RemovePIEPrefix(LoadedWorld->GetMapName());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "OnMap loaded " + realName);
	if (realName == StartMap)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ShowMainMenu");
		ShowMainMenu();
		//HideLoading();
	}
}

void UGameInstanceManager::ShowMainMenu()
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
	if (Widget)
	{
		Widget->AddToViewport();
	}
}
