// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

/**
 * 
 */
UCLASS(Config = ServerSettings)
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

	
};
