// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

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
	

	UPROPERTY(BlueprintReadWrite)
	TArray<UUserWidget*> MenuWidgetsRef;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateUIInputHander(bool bActivate);

	UFUNCTION()
	void OnCloseCurrentlyOpenMenu(const FInputActionInstance& InputActionInstance);

protected:

	virtual void SetupInputComponent() override;

	virtual void CloseLastOpenMenu();


public:
	
	UFUNCTION(BlueprintPure)
	bool IsAnyMenuOpen();

	UFUNCTION(BlueprintPure)
	FText FormatMatchTimer(float InSeconds);
	
};
