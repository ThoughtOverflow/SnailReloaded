// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DefaultPlayerController.h"
#include "Gameplay/Player/MenuPlayer.h"
#include "Gameplay/UI/SkinOpeningWidget.h"
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

	UPROPERTY()
	AMenuPlayer* MenuPlayer;
	
	UPROPERTY(BlueprintReadWrite)
	AMenuCamera* SkinCamera;
	UPROPERTY(BlueprintReadWrite)
	AMenuCamera* OpeningCamera;

	AMenuPlayerController();

	UFUNCTION(BlueprintCallable)
	void ShowSkinMenu();
	UFUNCTION(BlueprintCallable)
	void ShowOpeningMenu();
	UFUNCTION(BlueprintCallable)
	void ReturnToMenu();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MainMenuWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UOutfitSelectionWidget> OutfitSelectionWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UOutfitSelectionWidget* OutfitSelectionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<USkinOpeningWidget> OpeningWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	USkinOpeningWidget* OpeningWidget;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI input settings")
	UInputAction* MouseScrollAction;

protected:

	virtual void BeginPlay() override;

	FViewTargetTransitionParams TransitionParams;

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void RotateOutfitDummy(const FInputActionInstance& InputActionInstance);

	UFUNCTION(BlueprintCallable)
	void ToggleDisplayActorVisibility(bool bVisible);

public:

	UFUNCTION(Client, Reliable)
	void Client_ToggleMainMenuWidget(bool bOn);

	UFUNCTION(BlueprintCallable)
	void ToggleMainMenuWidget(bool bOn);

	UFUNCTION(BlueprintCallable)
	virtual void ToggleOutfitMenu(bool bOpen);
	UFUNCTION(BlueprintCallable)
	virtual void ToggleSkinOpenMenu(bool bOpen);
	UFUNCTION(Client, Reliable)
	void Client_ToggleOutfitMenu(bool bOpen);
	UFUNCTION(Client, Reliable)
	void Client_ToggleSkinOpenMenu(bool bOpen);
	
};
