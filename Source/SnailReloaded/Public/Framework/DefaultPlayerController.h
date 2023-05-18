// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game UI")
	TSubclassOf<UPauseWidget> PauseWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UPauseWidget* PauseWidget;
	

	UPROPERTY(BlueprintReadWrite)
	TArray<UUserWidget*> MenuWidgetsRef;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateUIInputHandler(bool bActivate);

	UFUNCTION()
	void OnCloseCurrentlyOpenMenu(const FInputActionInstance& InputActionInstance);

protected:

	virtual void SetupInputComponent() override;

	virtual void CloseLastOpenMenu();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void TogglePauseMenu(bool bOpen);
	UFUNCTION(Client, Reliable)
	void Client_TogglePauseMenu(bool bOpen);


public:
	
	UFUNCTION(BlueprintPure)
	bool IsAnyMenuOpen();

	UFUNCTION(BlueprintPure)
	FText FormatMatchTimer(float InSeconds);
	
};
