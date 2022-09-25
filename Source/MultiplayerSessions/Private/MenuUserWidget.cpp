// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuUserWidget.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenuUserWidget::MenuSetup(const int32 NumberOfPublicConnections, FString TypeOfMatch, const FString LobbyPath)
{
	if (TypeOfMatch.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("TypeOfMatch is empty"));
		return;
	}

	if (LobbyPath.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("LobbyPath is empty"));
		return;
	}
	
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	const UWorld* World = GetWorld();
	if (!World) {
		UE_LOG(LogTemp, Error, TEXT("World is empty"));
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
	} else {
		UE_LOG(LogTemp, Error, TEXT("GameInstance is empty"));
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
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize"));
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("HostButton is empty"));
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("JoinButton is empty"));
	}

	return true;
}

void UMenuUserWidget::OnCreateSession(const bool bWasSuccessful)
{
	UWorld* World = GetWorld();
	if (!World) {
		UE_LOG(LogTemp, Error, TEXT("World is empty"));
		return;
	}

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
		return;
	}

	World->ServerTravel(PathToLobby);
}

void UMenuUserWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("MultiplayerSessionsSubsystem is empty"));
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
		UE_LOG(LogTemp, Error, TEXT("Subsystem is empty"));
		return;
	}

	const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("SessionInterface is invalid"));
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, Address)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve platform specific connection information for joining the match"));
		return;
	}

	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	if (!PlayerController) {
		UE_LOG(LogTemp, Error, TEXT("FirstLocalPlayerController is empty"));
		return;
	}

	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

void UMenuUserWidget::OnDestroySession(const bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem) {
		UE_LOG(LogTemp, Error, TEXT("MultiplayerSessionsSubsystem is empty"));
		return;
	}

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session"));
		return;
	}

	MultiplayerSessionsSubsystem->DestroySession();
}

void UMenuUserWidget::OnStartSession(const bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem) {
		UE_LOG(LogTemp, Error, TEXT("MultiplayerSessionsSubsystem is empty"));
		return;
	}

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Error, TEXT("Failed to start session"));
		return;
	}

	MultiplayerSessionsSubsystem->StartSession();
}

void UMenuUserWidget::HostButtonClicked()
{
	if (!MultiplayerSessionsSubsystem) {
		UE_LOG(LogTemp, Error, TEXT("MultiplayerSessionsSubsystem is empty"));
		return;
	}

	MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

void UMenuUserWidget::JoinButtonClicked()
{
	if (!MultiplayerSessionsSubsystem) {
		UE_LOG(LogTemp, Error, TEXT("MultiplayerSessionsSubsystem is empty"));
		return;
	}
	
	MultiplayerSessionsSubsystem->FindSessions(10'000);
}

void UMenuUserWidget::MenuTearDown()
{
	RemoveFromParent();

	const UWorld* World = GetWorld();
	if (!World) {
		UE_LOG(LogTemp, Error, TEXT("World is empty"));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) {
		UE_LOG(LogTemp, Error, TEXT("FirstPlayerController is empty"));
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
