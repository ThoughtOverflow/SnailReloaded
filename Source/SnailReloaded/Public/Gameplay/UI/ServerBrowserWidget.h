// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerBrowserWidget.generated.h"

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
	
};

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UServerBrowserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FServerEntry> Servers;

	UFUNCTION(BlueprintImplementableEvent)
	void RefreshBrowserWidget();
	
};
