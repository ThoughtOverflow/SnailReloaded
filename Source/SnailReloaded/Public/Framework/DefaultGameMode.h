// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

/**
 * 
 */

UENUM()
enum class EOutfitRarity : uint8
{
	Common = 0,
	Rare = 1,
	Epic = 2,
	Legendary = 3,
	Snailsane = 4
};

USTRUCT(BlueprintType)
struct FOutfitData
{
	GENERATED_BODY()

public:

	FOutfitData();

	FOutfitData(FString Name, EOutfitRarity Rarity, UTexture2D* Thumb);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString OutfitName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EOutfitRarity OutfitRarity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Thumbnail;
	
	
};

UCLASS(Config = "Game")
class SNAILRELOADED_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ADefaultGameMode();

	/**
	 * @brief The API token loaded from config file. Might not be null, if the server is not authenticated.
	 */
	UPROPERTY(Config)
	FString APIToken;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FString, FOutfitData> RegisteredOutfits;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FString, USkeletalMesh*> HeadgearAssets;


	
};
