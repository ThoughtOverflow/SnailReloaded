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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player UI")
	TSubclassOf<UUserWidget> DeathScreenClass;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* DeathScreen;

	

protected:

	virtual void OnPossess(APawn* InPawn) override;

	
	virtual void CloseLastOpenMenu() override;


public:

	virtual void ActivateUIInputHander(bool bActivate) override;
	
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
	UFUNCTION(Client, Reliable)
	void Client_ToggleBuyMenu(bool bOpen);

	//Death screen:

	UFUNCTION(BlueprintCallable)
	void ShowDeathScreen(bool bShow);
	UFUNCTION(Client, Reliable)
	void Client_ShowDeathScreen(bool bShow);

	//Respawn:

	UFUNCTION(BlueprintCallable)
	void SetRespawnRotation(FRotator NewRotation);
	UFUNCTION(Client, Reliable)
	void Client_SetRespawnRotation(FRotator NewRotation);

	

	
};
