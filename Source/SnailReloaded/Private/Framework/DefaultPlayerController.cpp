// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/UI/PauseWidget.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetTextLibrary.h"

ADefaultPlayerController::ADefaultPlayerController()
{
}


void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(InputComponent)
	{

		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(EscInputAction, ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnCloseCurrentlyOpenMenu);
		
	}
}

void ADefaultPlayerController::CloseLastOpenMenu()
{
	UUserWidget* LastWidget = MenuWidgetsRef[MenuWidgetsRef.Num() - 1];
	LastWidget->RemoveFromParent();
	MenuWidgetsRef.Remove(LastWidget);
}

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//Activate the esc handler by default.
	ActivateUIInputHandler(true);
}

void ADefaultPlayerController::TogglePauseMenu(bool bOpen)
{
	if(IsLocalController())
	{

		if(!PauseWidget && PauseWidgetClass)
		{
			PauseWidget = CreateWidget<UPauseWidget>(this, PauseWidgetClass);
		}
		if(PauseWidget)
		{
			if(bOpen)
			{
				if(!PauseWidget->IsInViewport())
				{
					PauseWidget->AddToViewport();
					SetShowMouseCursor(true);
					SetInputMode(FInputModeGameAndUI());
					MenuWidgetsRef.Add(PauseWidget);
				}
			}else
			{
				if(PauseWidget->IsInViewport())
				{
					PauseWidget->RemoveFromParent();
					SetShowMouseCursor(false);
					SetInputMode(FInputModeGameOnly());
					MenuWidgetsRef.Remove(PauseWidget);
				}
			}
		}
		
	}else
	{
		Client_TogglePauseMenu(bOpen);
	}
}

void ADefaultPlayerController::Client_TogglePauseMenu_Implementation(bool bOpen)
{
	TogglePauseMenu(bOpen);
}

void ADefaultPlayerController::ActivateUIInputHandler(bool bActivate)
{
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	if(bActivate)
	{
		EnhancedInputSubsystem->AddMappingContext(UIDefaultMappingContext, 0);
	}else
	{
		EnhancedInputSubsystem->RemoveMappingContext(UIDefaultMappingContext);
	}
}


void ADefaultPlayerController::OnCloseCurrentlyOpenMenu(const FInputActionInstance& InputActionInstance)
{
	if(InputActionInstance.GetValue().Get<bool>())
	{
		if(MenuWidgetsRef.Num() > 0)
		{
			CloseLastOpenMenu();	
		}else
		{
			//Open pause menu.
			TogglePauseMenu(true);
		}
	}
}

bool ADefaultPlayerController::IsAnyMenuOpen()
{
	for(auto& Widget : MenuWidgetsRef)
	{
		if(Widget->IsInViewport())
		{
			return true;
		}
	}
	return false;
}

FText ADefaultPlayerController::FormatMatchTimer(float InSeconds)
{

	FTimespan Timespan = FTimespan::FromSeconds(FMath::Floor(InSeconds));
	FText SecondsText = UKismetTextLibrary::Conv_IntToText(Timespan.GetSeconds(), false, false, 2);
	return  FText::FromString(FString::Printf(TEXT("%d:%s"), Timespan.GetMinutes(), *SecondsText.ToString()));
}
