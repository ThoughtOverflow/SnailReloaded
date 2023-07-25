// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Menu/MenuPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "World/Objects/MenuCamera.h"

AMenuPlayerController::AMenuPlayerController()
{
	
}

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMenuCamera::StaticClass(), FoundActors);
	for(auto& actor : FoundActors)
	{
		AMenuCamera* camActor = Cast<AMenuCamera>(actor);
		if(camActor->CamType == EMenuCamType::SKINSELECT)
		{
			SkinCamera = camActor;
		}

		if(SkinCamera)
		{
			break;
		}
	}
	checkf(SkinCamera, TEXT("One or more menu camera definitions do not exist!"));

	MenuPlayer = Cast<AMenuPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuPlayer::StaticClass()));

	checkf(MenuPlayer, TEXT("No menu player!!"));

	Possess(MenuPlayer);
	// SetViewTargetWithBlend(MenuCamera, 0.3f, VTBlend_EaseInOut, 2.f);
	
	
}
