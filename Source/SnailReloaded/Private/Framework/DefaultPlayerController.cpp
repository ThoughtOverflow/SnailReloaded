// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/UI/PauseWidget.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetTextLibrary.h"

FMenuWidgetData::FMenuWidgetData()
{
	MenuWidget = nullptr;
	bShowsMouseCursor = true;
}

ADefaultPlayerController::ADefaultPlayerController()
{
	bNonMenuCursorState = false;
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
	UUserWidget* LastWidget = MenuWidgetsRef[MenuWidgetsRef.Num() - 1].MenuWidget;
	LastWidget->RemoveFromParent();
	if(MenuWidgetsRef.Num() > 1)
	{
		SetShowMouseCursor(MenuWidgetsRef[MenuWidgetsRef.Num() - 2].bShowsMouseCursor);
		SetInputMode(FInputModeGameAndUI());
	}else
	{
		SetShowMouseCursor(bNonMenuCursorState);
		ResetNonMenuInputMode();
	}
	MenuWidgetsRef.RemoveAt(MenuWidgetsRef.Num() - 1);
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
					ToggleMenuWidget(PauseWidget, true);
				}
			}else
			{
				if(PauseWidget->IsInViewport())
				{
					ToggleMenuWidget(PauseWidget, false);
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
	if(IsLocalController())
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
	
}


void ADefaultPlayerController::OnCloseCurrentlyOpenMenu(const FInputActionInstance& InputActionInstance)
{
	if(InputActionInstance.GetValue().Get<bool>())
	{
		if(MenuWidgetsRef.Num() >= 1)
		{
			CloseLastOpenMenu();	
		}else
		{
			//Open pause menu.
			TogglePauseMenu(true);
		}
	}
}

void ADefaultPlayerController::ResetNonMenuInputMode()
{
	SetInputMode(FInputModeUIOnly());
}

bool ADefaultPlayerController::IsAnyMenuOpen()
{
	for(auto& WidgetData : MenuWidgetsRef)
	{
		if(WidgetData.MenuWidget->IsInViewport())
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

void ADefaultPlayerController::ToggleMenuWidget(UUserWidget* MenuWidget, bool bOn, bool bShowsCursor)
{

	if(MenuWidgetsRef.Num() == 0)
	{
		if(bOn)
		{
			//Store prev cursor state:
			bNonMenuCursorState = bShowMouseCursor;
		}
	}
	
	SetShowMouseCursor(bOn);
	FMenuWidgetData MenuWidgetData;
	MenuWidgetData.MenuWidget = MenuWidget;
	MenuWidgetData.bShowsMouseCursor = bShowsCursor;
	if(bOn )
	{
		MenuWidget->AddToViewport();
		SetInputMode(FInputModeGameAndUI());
		MenuWidgetsRef.Add(MenuWidgetData);
	}else
	{
		MenuWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
		int32 indexToRemove = 0;
		for(int i=0; i<MenuWidgetsRef.Num(); i++)
		{
			if(MenuWidgetsRef[i].MenuWidget == MenuWidget)
			{
				indexToRemove = i;
				break;
			}
		}
		if(MenuWidgetsRef.Num() == 1)
		{
			//Last widget - Call the close operation, to reset the properties;
			CloseLastOpenMenu();
		}else
		{
			MenuWidgetsRef.RemoveAt(indexToRemove);
		}
	}
	
}

void ADefaultPlayerController::ToggleMainMenuWidget(bool bOn)
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

void ADefaultPlayerController::ToggleGameInitWidget(bool bOn)
{
	if(IsLocalController())
	{
		if(GameInitWidgetClass && !GameInitWidget) GameInitWidget = CreateWidget(this, GameInitWidgetClass);
		if(GameInitWidget)
		{
			if(bOn)
			{
				GameInitWidget->AddToViewport();
			}else
			{
				GameInitWidget->RemoveFromParent();
			}
		}
	}else
	{
		Client_ToggleGameInitWidget(bOn);
	}
}

void ADefaultPlayerController::Client_ToggleMainMenuWidget_Implementation(bool bOn)
{
	ToggleMainMenuWidget(bOn);
}

void ADefaultPlayerController::Client_ToggleGameInitWidget_Implementation(bool bOn)
{
	ToggleGameInitWidget(bOn);
}
