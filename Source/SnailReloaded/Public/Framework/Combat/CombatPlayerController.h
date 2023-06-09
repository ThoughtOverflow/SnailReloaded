// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/DefaultPlayerController.h"
#include "Gameplay/UI/DamageIndicatorWidget.h"
#include "Gameplay/UI/GameNotification.h"
#include "Gameplay/UI/TeamSelectionWidget.h"
#include "CombatPlayerController.generated.h"

enum class ENotificationType : uint8;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player UI")
	TSubclassOf<UUserWidget> ScoreBoardClass;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* ScoreBoardWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player UI")
	TSubclassOf<UDamageIndicatorWidget> DamageIndicatorClass;
	UPROPERTY(BlueprintReadWrite)
	TArray<UDamageIndicatorWidget*> DamageIndicatorWidgets;
	

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInputMappingContext* CombatUIMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInputAction* ScoreboardToggleAction;

	UFUNCTION()
	void OnToggleScoreboardTriggered(const FInputActionInstance& InputActionInstance);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void TryBlockPlayerInputs(bool bBlock);

	virtual void ResetNonMenuInputMode() override;

	//Notification classes.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<ENotificationType, TSubclassOf<UGameNotification>> NotificationWidgetClasses;
	UPROPERTY()
	UGameNotification* CurrentNotification;
	UPROPERTY()
	FTimerHandle NotificationTimer;
	UFUNCTION()
	void NotificationTimer_Callback();
	UFUNCTION(Client, Reliable)
	void Client_TriggerGameNotification(ENotificationType Notification);
	


public:
	
	
	//Player HUD
	
	UFUNCTION()
	void CreatePlayerHud();
	UFUNCTION(Client, Reliable)
	void Client_CreatePlayerHud();
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHud(UHudData* HudData);
	UFUNCTION(BlueprintPure)
	UHudData* GetHudData();
	UFUNCTION()
	void AddDamageIndicator(AActor* Source);
	UFUNCTION(BlueprintCallable)
	void RemoveDamageIndicator(AActor* Source);
	UFUNCTION(Client, Reliable)
	void Client_AddDamageIndicator(AActor* Source);
	UFUNCTION(BlueprintPure)
	float GetUpdatedAngleForDamageIndicator(FVector InitialSourceloc);
	

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

	//Team Score display

	UFUNCTION(BlueprintPure)
	int32 GetAllyTeamScore();
	UFUNCTION(BlueprintPure)
	int32 GetEnemyTeamScore();

	//Scoreboard:

	UFUNCTION(BlueprintCallable)
	void ToggleScoreboard(bool bShow);

	//Pause menu when player exists
	virtual void TogglePauseMenu(bool bOpen) override;

	//Game Notifications:
	void TriggerGameNotification(ENotificationType Notification);

	
};
