#pragma once

#include "CoreMinimal.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "MenuUserWidget.generated.h"


UCLASS()
class MULTIPLAYERSESSIONS_API UMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta=(BindWidget))
	UButton* HostButton;

	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections = 4;
	FString MatchType = UMultiplayerSessionsSubsystem::MatchTypeFreeForAll;
};
