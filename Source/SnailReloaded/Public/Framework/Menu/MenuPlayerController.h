// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DefaultPlayerController.h"
#include "Gameplay/Player/MenuPlayer.h"
#include "MenuPlayerController.generated.h"

class AMenuCamera;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API AMenuPlayerController : public ADefaultPlayerController
{
	GENERATED_BODY()

public:

	AMenuPlayer* MenuPlayer;
	
	UPROPERTY(BlueprintReadWrite)
	AMenuCamera* SkinCamera;

	AMenuPlayerController();

protected:

	virtual void BeginPlay() override;
	
};
