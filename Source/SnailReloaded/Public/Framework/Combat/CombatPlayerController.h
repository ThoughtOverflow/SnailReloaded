// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/DefaultPlayerController.h"
#include "Gameplay/UI/TeamSelectionWidget.h"
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player UI")
	TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UTeamSelectionWidget* TeamSelectionWidget;

	

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	
	virtual void CloseLastOpenMenu() override;


public:

	virtual void ActivateUIInputHandler(bool bActivate) override;
	
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
	UFUNCTION(BlueprintCallable)
	void SelectOverviewCamera();

	//Respawn:

	UFUNCTION(BlueprintCallable)
	void SetRespawnRotation(FRotator NewRotation);
	UFUNCTION(Client, Reliable)
	void Client_SetRespawnRotation(FRotator NewRotation);

	UFUNCTION(BlueprintCallable)
	void ToggleTeamSelectionScreen(bool bOpen);
	UFUNCTION(Client, Reliable)
	void Client_ToggleTeamSelectionScreen(bool bOpen);

	//Game Start:

	UFUNCTION(BlueprintCallable)
	void TryStartGame();
	UFUNCTION(BlueprintCallable)
	void AssignPlayerToTeam(EGameTeams Team);
	UFUNCTION(Server, Reliable)
	void Server_AssignPlayerToTeam(EGameTeams Team);
	

	
};
