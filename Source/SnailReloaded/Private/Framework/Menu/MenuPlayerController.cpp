// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Menu/MenuPlayerController.h"

#include "Framework/DefaultPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "World/Objects/ComputerCase.h"
#include "World/Objects/Holostand.h"
#include "World/Objects/ImageTablet.h"
#include "World/Objects/MenuCamera.h"
#include "World/Objects/SkinDisplayActor.h"

AMenuPlayerController::AMenuPlayerController()
{
	TransitionParams.BlendTime = 0.7f;
	TransitionParams.BlendFunction = VTBlend_EaseIn;
	TransitionParams.BlendExp = 0.8f;
	TransitionParams.bLockOutgoing = false;

	bNonMenuCursorState = true;
}

void AMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(InputComponent)
	{

		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		
		EnhancedInputComponent->BindAction(MouseScrollAction, ETriggerEvent::Triggered, this, &AMenuPlayerController::RotateOutfitDummy);
	}
}

void AMenuPlayerController::ShowSkinMenu()
{
	ToggleCaseVisibility(false);
	ToggleDisplayActorVisibility(true);
	SetViewTarget(SkinCamera, TransitionParams);
	ToggleMainMenuWidget(false);
	ToggleSkinOpenMenu(false);
	ToggleOutfitMenu(true);
	ResetOpeningScene();
}

void AMenuPlayerController::ShowOpeningMenu()
{
	ToggleDisplayActorVisibility(false);
	ToggleCaseVisibility(true);
	SetViewTarget(OpeningCamera, TransitionParams);
	ToggleOutfitMenu(false);
	ToggleSkinOpenMenu(true);
}

void AMenuPlayerController::ReturnToMenu()
{
	SetViewTarget(MenuPlayer, TransitionParams);
	ToggleOutfitMenu(false);
	ToggleSkinOpenMenu(false);
	ToggleMainMenuWidget(true);
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
		}else if(camActor->CamType == EMenuCamType::CASE_OPENING)
		{
			OpeningCamera = camActor;
		}
	}
	checkf(SkinCamera && OpeningCamera, TEXT("One or more menu camera definitions do not exist!"));

	MenuPlayer = Cast<AMenuPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuPlayer::StaticClass()));

	checkf(MenuPlayer, TEXT("No menu player!!"));

	//do the filling:

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AImageTablet::StaticClass(), Actors);
	for(auto& actor : Actors)
	{
		Cast<AImageTablet>(actor)->RandomizeDecal();
	}

	Possess(MenuPlayer);
	ToggleMainMenuWidget(true);
	SetInputMode(FInputModeGameAndUI());
	SetShowMouseCursor(true);
	
}


void AMenuPlayerController::ToggleOutfitMenu(bool bOpen)
{
	if(IsLocalController())
	{
		if(!OutfitSelectionWidget && OutfitSelectionWidgetClass) OutfitSelectionWidget = CreateWidget<UOutfitSelectionWidget>(this, OutfitSelectionWidgetClass);
		if(OutfitSelectionWidget)
		{
			if(bOpen)
			{
				if(!OutfitSelectionWidget->IsInViewport())
				{
					ToggleMenuWidget(OutfitSelectionWidget, true);
				}
			}else
			{
				if(OutfitSelectionWidget->IsInViewport())
				{
					ToggleMenuWidget(OutfitSelectionWidget, false);
				}
			}
		}
	}else
	{
		Client_ToggleOutfitMenu(bOpen);
	}
}

void AMenuPlayerController::ToggleSkinOpenMenu(bool bOpen)
{
	if(IsLocalController())
	{
		if(!OpeningWidget && OpeningWidgetClass) OpeningWidget = CreateWidget<USkinOpeningWidget>(this, OpeningWidgetClass);
		if(OpeningWidget)
		{
			if(bOpen)
			{
				if(!OpeningWidget->IsInViewport())
				{
					ToggleMenuWidget(OpeningWidget, true);
				}
			}else
			{
				if(OpeningWidget->IsInViewport())
				{
					ToggleMenuWidget(OpeningWidget, false);
				}
			}
		}
	}else
	{
		Client_ToggleSkinOpenMenu(bOpen);
	}
}

void AMenuPlayerController::OpenCase()
{
	ResetOpeningScene();
	if(OpeningWidget)
	{
		OpeningWidget->bOpeningInProgress = true;
	}
	if(ADefaultPlayerState* DefaultPlayerState = GetPlayerState<ADefaultPlayerState>())
	{
		DefaultPlayerState->API_OpenCrate();
	}
}

void AMenuPlayerController::CrateDataReceived(const TSharedPtr<FJsonObject>* Data)
{
	if(AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AComputerCase::StaticClass()))
	{
		AComputerCase* ComputerCase = Cast<AComputerCase>(Actor);
		if(ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			const TSharedPtr<FJsonObject>* ItemObject;
			Data->Get()->TryGetObjectField("data", ItemObject);
			FOutfitData OutfitData = *DefaultGameMode->RegisteredOutfits.Find(ItemObject->Get()->GetStringField("id"));
			UTexture2D* OpenedItemTexture = OutfitData.Thumbnail;
			ComputerCase->SetOpenedOutfitTexture(OpenedItemTexture);
			if(Data->Get()->GetStringField("result") == "itemgranted")
			{
				if(OpeningWidget)
				{
					OpeningWidget->DisplayedItemName = OutfitData.OutfitName;
				}
			}
			else if(Data->Get()->GetStringField("result") == "duplicate")
			{
				if(OpeningWidget)
				{
					OpeningWidget->DisplayedItemName = FString::Printf(TEXT("%s - duplicate"), *OutfitData.OutfitName);
				}
			}
			if(AHolostand* Holostand = Cast<AHolostand>(UGameplayStatics::GetActorOfClass(GetWorld(), AHolostand::StaticClass())))
			{
				Holostand->PlayCaseOpening();
			}
		}
	}
}

void AMenuPlayerController::OnCaseOpeningAnimationFinished()
{
	if(OpeningWidget)
	{
		OpeningWidget->PlayTextAnimation();
		OpeningWidget->bOpeningInProgress = false;
	}
}

void AMenuPlayerController::Client_ToggleSkinOpenMenu_Implementation(bool bOpen)
{
	ToggleSkinOpenMenu(bOpen);
}

void AMenuPlayerController::ToggleMainMenuWidget(bool bOn)
{
	if(IsLocalController())
	{
		if(MainMenuWidgetClass && !MainMenuWidget) MainMenuWidget = CreateWidget(this, MainMenuWidgetClass);
		if(MainMenuWidget)
		{
			if(bOn)
			{
				MainMenuWidget->AddToViewport();
			}else
			{
				MainMenuWidget->RemoveFromParent();
			}
		}
	}else
	{
		Client_ToggleMainMenuWidget(bOn);
	}
}

void AMenuPlayerController::Client_ToggleMainMenuWidget_Implementation(bool bOn)
{
	ToggleMainMenuWidget(bOn);
}

void AMenuPlayerController::Client_ToggleOutfitMenu_Implementation(bool bOpen)
{
	ToggleOutfitMenu(bOpen);
}

void AMenuPlayerController::RotateOutfitDummy(const FInputActionInstance& InputActionInstance)
{
	float rotation = InputActionInstance.GetValue().Get<float>();

	if(AActor* Dummy = UGameplayStatics::GetActorOfClass(GetWorld(), ASkinDisplayActor::StaticClass()))
	{
		Dummy->AddActorLocalRotation(FRotator(0.f, rotation, 0.f));
	}	
}

void AMenuPlayerController::ToggleDisplayActorVisibility(bool bVisible)
{
	if(AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ASkinDisplayActor::StaticClass()))
	{
		Actor->SetActorHiddenInGame(!bVisible);
	}
}

void AMenuPlayerController::ToggleCaseVisibility(bool bVisible)
{
	if(AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), AComputerCase::StaticClass()))
	{
		Actor->SetActorHiddenInGame(!bVisible);
	}
}

void AMenuPlayerController::ResetOpeningScene()
{
	if(AComputerCase* ComputerCase = Cast<AComputerCase>(UGameplayStatics::GetActorOfClass(GetWorld(), AComputerCase::StaticClass())))
	{
		ComputerCase->bOpenCase = false;
		ComputerCase->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ComputerCase->SetActorLocation(ComputerCase->OriginalLocation);
	}
	if(AHolostand* Holostand = Cast<AHolostand>(UGameplayStatics::GetActorOfClass(GetWorld(), AHolostand::StaticClass())))
	{
		Holostand->bPlayCaseOpening = false;
	}
	
}
