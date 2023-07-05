// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IHttpRequest.h"
#include "DefaultPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerOwnedItemsUpdated);

struct FOutfitData;
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

USTRUCT(BlueprintType)
struct FAPIItemData
{

	FAPIItemData();
	
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ItemId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString GroupId;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bItemEquipped;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FOutfitData OutfitProperties;
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
	void API_EquipOutfit(FString ItemID);
	UFUNCTION()
	void API_GetPlayerInventoryData();
	UFUNCTION()
	void API_ValidateToken();
	UFUNCTION()
	void API_ValidateUser();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
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
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerItems)
	TArray<FAPIItemData> PlayerOwnedItems;

	UFUNCTION()
	FString GetPlayerEpicID();
	UFUNCTION()
	FString GetPlayerAuthToken();

	UFUNCTION()
	void OnRep_PlayerItems();

	UFUNCTION(BlueprintCallable)
	FString GetActiveAPIAdress(FString command);
	
	
protected:


	UPROPERTY(BlueprintAssignable)
	FOnPlayerOwnedItemsUpdated OnPlayerOwnedItemsUpdated;
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnLoginComplete();
	
	void OnGetItemsRequestComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
	void OnGetAccountDataRequestComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
	void OnGetTokenValidationComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
	void OnAccountValidationComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);
	void OnEquipmentComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess);

	virtual void CopyProperties(APlayerState* PlayerState) override;
	
};
