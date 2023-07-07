// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Gameplay/UI/OutfitSelectionWidget.h"
#include "DefaultPlayerController.generated.h"

USTRUCT(BlueprintType)
struct FMenuWidgetData
{
	GENERATED_BODY()
public:

	FMenuWidgetData();

	UPROPERTY()
	UUserWidget* MenuWidget;
	UPROPERTY()
	bool bShowsMouseCursor;
	
};

class UPauseWidget;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ADefaultPlayerController();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI input settings")
	UInputMappingContext* UIDefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI input settings")
	UInputAction* EscInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI input settings")
	UInputAction* MouseScrollAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UPauseWidget> PauseWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UPauseWidget* PauseWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MainMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UUserWidget> GameInitWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* GameInitWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UOutfitSelectionWidget> OutfitSelectionWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UOutfitSelectionWidget* OutfitSelectionWidget;
	

	UPROPERTY(BlueprintReadWrite)
	TArray<FMenuWidgetData> MenuWidgetsRef;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateUIInputHandler(bool bActivate);

	UFUNCTION()
	void OnCloseCurrentlyOpenMenu(const FInputActionInstance& InputActionInstance);
	UFUNCTION()
	void RotateOutfitDummy(const FInputActionInstance& InputActionInstance);

protected:

	virtual void SetupInputComponent() override;

	virtual void CloseLastOpenMenu();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void TogglePauseMenu(bool bOpen);
	UFUNCTION(Client, Reliable)
	void Client_TogglePauseMenu(bool bOpen);
	UFUNCTION(BlueprintCallable)
	virtual void ToggleOutfitMenu(bool bOpen);
	UFUNCTION(Client, Reliable)
	void Client_ToggleOutfitMenu(bool bOpen);

	UPROPERTY()
	bool bNonMenuCursorState;

	UFUNCTION()
	virtual void ResetNonMenuInputMode();


public:
	
	UFUNCTION(BlueprintPure)
	bool IsAnyMenuOpen();

	UFUNCTION(BlueprintPure)
	FText FormatMatchTimer(float InSeconds);

	UFUNCTION(BlueprintCallable)
	void ToggleMenuWidget(UUserWidget* MenuWidget, bool bOn, bool bShowsCursor = true);

//Main menu calls:
	
	UFUNCTION(BlueprintCallable)
	void ToggleMainMenuWidget(bool bOn);
	UFUNCTION(BlueprintCallable)
	void ToggleGameInitWidget(bool bOn);

	UFUNCTION(Client, Reliable)
	void Client_ToggleMainMenuWidget(bool bOn);
	UFUNCTION(Client, Reliable)
	void Client_ToggleGameInitWidget(bool bOn);
	
};
