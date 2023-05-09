// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
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

void ADefaultPlayerController::ActivateUIInputHander(bool bActivate)
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
	if(InputActionInstance.GetValue().Get<bool>() && MenuWidgetsRef.Num() > 0)
	{
		CloseLastOpenMenu();
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
