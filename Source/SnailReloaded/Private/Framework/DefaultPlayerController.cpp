// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

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