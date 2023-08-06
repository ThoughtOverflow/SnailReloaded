// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "ServerEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API UServerEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FName SessionName;
	UPROPERTY(BlueprintReadWrite)
	bool Auth;
	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentPlayers;
	UPROPERTY(BlueprintReadWrite)
	int32 Ping;
	
	FOnlineSessionSearchResult SearchResult;

	UFUNCTION(BlueprintCallable)
	void OnJoinRequest();
	
	
};
