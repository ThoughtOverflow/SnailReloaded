// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/DefaultPlayerController.h"
#include "CombatPlayerController.generated.h"

struct FInputActionInstance;
class UInputMappingContext;
class UInputAction;
class UBuyMenu;
class UHudData;
class UPlayerHud;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ACombatPlayerController : public ADefaultPlayerController
{
	GENERATED_BODY()

public:

	ACombatPlayerController();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player UI")
	TSubclassOf<UPlayerHud> PlayerHudClass;
	UPROPERTY(BlueprintReadWrite)
	UPlayerHud* PlayerHud;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player UI")
	TSubclassOf<UBuyMenu> BuyMenuClass;
	UPROPERTY(BlueprintReadWrite)
	UBuyMenu* BuyMenu;
	UPROPERTY(BlueprintReadWrite)
	TArray<UUserWidget*> MenuWidgetsRef;

protected:

	virtual void OnPossess(APawn* InPawn) override;


public:

	UFUNCTION()
	void CloseCurrentlyOpenMenus(const FInputActionInstance& InputActionInstance);
	
	//Player HUD
	
	UFUNCTION()
	void CreatePlayerHud();
	UFUNCTION(Client, Reliable)
	void Client_CreatePlayerHud();
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHud(UHudData* HudData);
	UFUNCTION(BlueprintPure)
	UHudData* GetHudData();

	//Buy menu:

	UFUNCTION(BlueprintCallable)
	void ToggleBuyMenu(bool bOpen);

	UFUNCTION(BlueprintPure)
	bool IsAnyMenuOpen();

	
};
