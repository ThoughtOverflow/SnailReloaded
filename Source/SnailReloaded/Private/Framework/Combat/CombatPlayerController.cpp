// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Components/ArmoredHealthComponent.h"
#include "Framework/Combat/CombatGameState.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Gameplay/UI/BuyMenu.h"
#include "Gameplay/UI/HudData.h"
#include "Gameplay/UI/PlayerHud.h"
#include "Kismet/GameplayStatics.h"


ACombatPlayerController::ACombatPlayerController()
{
	
}

void ACombatPlayerController::OnToggleScoreboardTriggered(const FInputActionInstance& InputActionInstance)
{
	if(InputActionInstance.GetValue().Get<bool>())
	{
		ToggleScoreboard(true);
	}else
	{
		ToggleScoreboard(false);
	}
	
}

void ACombatPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//If player character is possessed, callback to initialize
	if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(InPawn))
	{
		ToggleTeamSelectionScreen(false);
		CreatePlayerHud();
		PlayerCharacter->ReloadPlayerBanner();
		PlayerCharacter->OnPlayerPossessed(this);
		
	}
}

void ACombatPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if(HasAuthority())
		{
			Possess(CombatGameState->GetLevelOverviewCamera());
		}
	}
	ToggleTeamSelectionScreen(true);
	
}

void ACombatPlayerController::CloseLastOpenMenu()
{
	//Last menu with cursor enabled
	if(MenuWidgetsRef.Num() == 1)
	{
		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());
	}
	Super::CloseLastOpenMenu();
}

void ACombatPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(InputComponent)
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		EnhancedInputSubsystem->AddMappingContext(CombatUIMappingContext, 0);
		
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(ScoreboardToggleAction, ETriggerEvent::Triggered, this, &ACombatPlayerController::OnToggleScoreboardTriggered);
		EnhancedInputComponent->BindAction(ScoreboardToggleAction, ETriggerEvent::Completed, this, &ACombatPlayerController::OnToggleScoreboardTriggered);
		
	}
}

void ACombatPlayerController::TryBlockPlayerInputs(bool bBlock)
{
	if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(GetPawn()))
	{
		DefaultPlayerCharacter->BlockPlayerInputs(bBlock);
	}
}


void ACombatPlayerController::CreatePlayerHud()
{
	if(IsLocalController())
	{
		//Always recreate to avoid net sync issues:
		
		if(PlayerHudClass) PlayerHud = CreateWidget<UPlayerHud>(this, PlayerHudClass);
		if(PlayerHud && !PlayerHud->IsInViewport()) PlayerHud->AddToViewport();
		if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			SetViewTargetWithBlend(GetPawn(), 1.f, VTBlend_EaseInOut, 3.f);
		}
	}else
	{
		Client_CreatePlayerHud();
	}
}

void ACombatPlayerController::Client_CreatePlayerHud_Implementation()
{
	CreatePlayerHud();
}

void ACombatPlayerController::UpdatePlayerHud(UHudData* HudData)
{
	if(IsLocalController() && PlayerHud)
	{
		PlayerHud->PlayerHealthPercentage = HudData->GetPlayerHealthPercentage();
		PlayerHud->CurrentClipAmmo = HudData->GetCurrentClipAmmo();
		PlayerHud->CurrentTotalAmmo = HudData->GetCurrentTotalAmmo();
		PlayerHud->CurrentWeaponName = HudData->GetCurrentWeaponName();
		PlayerHud->PlayerShieldHealth = HudData->GetPlayerShieldHealth();
		PlayerHud->bIsReloading = HudData->GetIsReloading();
		PlayerHud->bShowPlantHint = HudData->GetShowPlantHint();
		PlayerHud->bShowDefuseHint = HudData->GetShowDefuseHint();
	}
}

UHudData* ACombatPlayerController::GetHudData()
{
	UHudData* HudData = UHudData::CreatePlayerHudWrapper(this);
	if(IsLocalController() && PlayerHud)
	{
		HudData->SetCurrentClipAmmo(PlayerHud->CurrentClipAmmo)->
		SetCurrentTotalAmmo(PlayerHud->CurrentTotalAmmo)->
		SetCurrentWeaponName(PlayerHud->CurrentWeaponName)->
		SetPlayerHealthPercentage(PlayerHud->PlayerHealthPercentage)->
		SetPlayerShieldHealth(PlayerHud->PlayerShieldHealth)->
		SetReloading(PlayerHud->bIsReloading)->
		SetShowPlantHint(PlayerHud->bShowPlantHint)->
		SetShowDefuseHint(PlayerHud->bShowDefuseHint);
	}
	return HudData;
}

void ACombatPlayerController::ToggleBuyMenu(bool bOpen)
{
	if(IsLocalController())
	{
		if(!BuyMenu && BuyMenuClass)
		{
			BuyMenu = CreateWidget<UBuyMenu>(this, BuyMenuClass);
		}
		if(BuyMenu)
		{
			if(bOpen)
			{
				BuyMenu->AddToViewport();
				SetShowMouseCursor(true);
				SetInputMode(FInputModeGameAndUI());
				TryBlockPlayerInputs(true);
				MenuWidgetsRef.Add(BuyMenu);
				
			}else
			{
				if(BuyMenu->IsInViewport())
				{
					BuyMenu->RemoveFromParent();
					SetShowMouseCursor(false);
					SetInputMode(FInputModeGameOnly());
					TryBlockPlayerInputs(false);
					if(MenuWidgetsRef.Contains(BuyMenu)) MenuWidgetsRef.Remove(BuyMenu);
				}
			}
		}
	}else
	{
		Client_ToggleBuyMenu(bOpen);
	}
}

void ACombatPlayerController::ShowDeathScreen(bool bShow)
{
	if(IsLocalController())
	{
		if(!DeathScreen && DeathScreenClass)
		{
			DeathScreen = CreateWidget<UUserWidget>(this, DeathScreenClass);
		}
		if(DeathScreen)
		{
			if(bShow)
			{
				if(PlayerHud)
				{
					PlayerHud->RemoveFromParent();
				}
				if(!DeathScreen->IsInViewport()) DeathScreen->AddToViewport();
			}else
			{
				if(DeathScreen->IsInViewport()) DeathScreen->RemoveFromParent();
			}
			
		}
	}else
	{
		Client_ShowDeathScreen(bShow);
	}
}

void ACombatPlayerController::SelectOverviewCamera()
{
	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		//TODO: FCK UNREAL'S ASS.
	}
	
}

void ACombatPlayerController::SetRespawnRotation(FRotator NewRotation)
{
	if(IsLocalController())
	{
		SetControlRotation(NewRotation);
	}else
	{
		Client_SetRespawnRotation(NewRotation);
	}
}

void ACombatPlayerController::ToggleTeamSelectionScreen(bool bOpen)
{
	if(IsLocalController())
	{
		if(TeamSelectionWidgetClass && !TeamSelectionWidget) TeamSelectionWidget = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass);
		if(bOpen)
		{
			if(TeamSelectionWidget && !TeamSelectionWidget->IsInViewport()) TeamSelectionWidget->AddToViewport();
			SetShowMouseCursor(true);
		}else
		{
			if(TeamSelectionWidget && TeamSelectionWidget->IsInViewport()) TeamSelectionWidget->RemoveFromParent();
			SetShowMouseCursor(false);
		}
	}else
	{
		Client_ToggleTeamSelectionScreen(bOpen);
	}
}

void ACombatPlayerController::Client_ToggleTeamSelectionScreen_Implementation(bool bOpen)
{
	ToggleTeamSelectionScreen(bOpen);
}

void ACombatPlayerController::TryStartGame()
{
	if(HasAuthority())
	{
		if(ACombatGameMode* CombatGameMode = Cast<ACombatGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatGameMode->InitializeCurrentGame();
		}
	}
}

void ACombatPlayerController::AssignPlayerToTeam(EGameTeams Team)
{
	if(HasAuthority())
	{
		GetPlayerState<ACombatPlayerState>()->SetTeam(Team);
	}else
	{
		Server_AssignPlayerToTeam(Team);
	}
}

int32 ACombatPlayerController::GetAllyTeamScore()
{
	if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if(ACombatPlayerState* CombatPlayerState = GetPlayerState<ACombatPlayerState>())
		{
			return StandardCombatGameState->GetScoreForTeam(CombatPlayerState->GetTeam());
		}
	}
	return 0;
}

int32 ACombatPlayerController::GetEnemyTeamScore()
{
	if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		if(ACombatPlayerState* CombatPlayerState = GetPlayerState<ACombatPlayerState>())
		{
			switch (CombatPlayerState->GetTeam()) {
			case EGameTeams::None: return 0;
			case EGameTeams::TeamA: return StandardCombatGameState->GetScoreForTeam(EGameTeams::TeamB);
			case EGameTeams::TeamB: return StandardCombatGameState->GetScoreForTeam(EGameTeams::TeamA);
			default: return 0;
			}
		}
	}
	return 0;
}

void ACombatPlayerController::ToggleScoreboard(bool bShow)
{
	if(IsLocalController())
	{
		if(ScoreBoardClass && !ScoreBoardWidget) ScoreBoardWidget = CreateWidget<UUserWidget>(this, ScoreBoardClass);

		if(ScoreBoardWidget)
		{
			if(bShow)
			{
				if(!ScoreBoardWidget->IsInViewport()) ScoreBoardWidget->AddToViewport();
			}else
			{
				if(ScoreBoardWidget->IsInViewport()) ScoreBoardWidget->RemoveFromParent();
			}
		}
	}
}

void ACombatPlayerController::TogglePauseMenu(bool bOpen)
{
	TryBlockPlayerInputs(bOpen);
	Super::TogglePauseMenu(bOpen);
}


void ACombatPlayerController::Server_AssignPlayerToTeam_Implementation(EGameTeams Team)
{
	AssignPlayerToTeam(Team);
}

void ACombatPlayerController::Client_SetRespawnRotation_Implementation(FRotator NewRotation)
{
	SetRespawnRotation(NewRotation);
}

void ACombatPlayerController::Client_ShowDeathScreen_Implementation(bool bShow)
{
	ShowDeathScreen(bShow);
}

void ACombatPlayerController::Client_ToggleBuyMenu_Implementation(bool bOpen)
{
	ToggleBuyMenu(bOpen);
}



