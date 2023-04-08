// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerController.h"

#include "Gameplay/Player/DefaultPlayerCharacter.h"

ADefaultPlayerController::ADefaultPlayerController()
{
}

void ADefaultPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//If player character is possessed, callback to initialize
	if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(InPawn))
	{
		PlayerCharacter->OnPlayerPossessed(this);
	}
}
