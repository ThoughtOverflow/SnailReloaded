// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IHttpRequest.h"
#include "DefaultPlayerState.generated.h"

USTRUCT(BlueprintType)
struct FAPIAccountData
{
	GENERATED_BODY()

public:

	FAPIAccountData();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 PlayerLevel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 PlayerXP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 UnopenedCrates;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 XpPerLevel;
	
};

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ADefaultPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ADefaultPlayerState();

	UFUNCTION()
	void API_GetPlayerAccountData();
	UFUNCTION()
	void API_GetPlayerInventoryData();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FAPIAccountData AccountData;

	/**
	 * @brief The current xp of the player (Retrieved from the API)
	 */
	UPROPERTY()
	int32 PlayerXP = 0;

	/**
	 * @brief The current level of the player (Retrieved from the API)
	 */
	UPROPERTY()
	int32 PlayerLevel = 0;

	/**
		 * @brief The current crates of the player (Retrieved from the API)
		 */
	UPROPERTY()
	int32 UnopenedCrates = 0;
	
	/**
	* @brief The owned items of the player (Retrieved from the API)
	*/
	UPROPERTY()
	TArray<FString> PlayerOwnedItems;
	

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnLoginComplete();

	UFUNCTION()
	FString GetPlayerEpicID();
	
	void OnGetItemsRequestComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
	void OnGetAccountDataRequestComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
	
};
