// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUserWidget.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenuUserWidget::MenuSetup(const int32 NumberOfPublicConnections, FString TypeOfMatch)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	const UWorld* World = GetWorld();
	if (!World) {
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (PlayerController) {
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PlayerController->SetInputMode(InputModeData);
		PlayerController->SetShowMouseCursor(true);
	}

	const UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenuUserWidget::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenuUserWidget::OnCreateSession(const bool bWasSuccessful)
{
	UWorld* World = GetWorld();
	if (bWasSuccessful && World) {
		World->ServerTravel("/Game/Levels/Lobby?listen");
	}
}

void UMenuUserWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr) {
		return;
	}

	for (auto Result : SessionResults) {
		FString SettingsValue;
		Result.Session.SessionSettings.Get(UMultiplayerSessionsSubsystem::MatchTypeKey, SettingsValue);

		if (SettingsValue == MatchType) {
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
}

void UMenuUserWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) {
		return;
	}

	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) {
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, Address)) {
		return;
	}

	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (!PlayerController) {
		return;
	}

	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

void UMenuUserWidget::OnDestroySession(bool bWasSuccessful)
{
}

void UMenuUserWidget::OnStartSession(bool bWasSuccessful)
{
}

void UMenuUserWidget::HostButtonClicked()
{
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenuUserWidget::JoinButtonClicked()
{
	if (MultiplayerSessionsSubsystem) {
		MultiplayerSessionsSubsystem->FindSessions(10'000);
	}
}

void UMenuUserWidget::MenuTearDown()
{
	RemoveFromParent();

	const UWorld* World = GetWorld();
	if (!World) {
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) {
		return;
	}

	const FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->SetShowMouseCursor(false);
}

void UMenuUserWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
