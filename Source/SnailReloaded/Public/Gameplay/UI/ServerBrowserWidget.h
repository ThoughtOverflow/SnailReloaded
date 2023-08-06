// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "ServerBrowserWidget.generated.h"

class UServerEntryWidget;
USTRUCT(BlueprintType)
struct FServerEntry
{
	GENERATED_BODY()

public:

	FServerEntry();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ServerName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxPlayers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 CurrentPlayers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bAuthenticatedSession;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ServerPing;
	FOnlineSessionSearchResult OnlineSessionSearchResult;
	
};

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UServerBrowserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UServerEntryWidget> ServerEntryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FServerEntry> Servers;

	UFUNCTION()
	void RefreshBrowserWidget();
	
};
