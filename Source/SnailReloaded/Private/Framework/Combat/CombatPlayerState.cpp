// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/Combat/CombatPlayerState.h"

#include "HttpModule.h"
#include "Framework/Combat/CombatGameState.h"
#include "Framework/Combat/CombatPlayerController.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACombatPlayerState::ACombatPlayerState()
{
	CurrentTeam = EGameTeams::None;
	IsDeadPreviousRound = false;
}

void ACombatPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		SetPlayerMoney(CombatGameState->InitialPlayerMoney);
	}
}

void ACombatPlayerState::OnRep_GameTeam()
{
	//If team changes, notify everything through the GameState.
	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		CombatGameState->OnRep_GamePlayers();
	}
}

void ACombatPlayerState::OnRep_DiedPreviousRound()
{
	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		CombatGameState->NotifyPlayerDeath(this);
	}
}

void ACombatPlayerState::OnRep_PlayerMoney()
{
	
}

void ACombatPlayerState::OnRep_ScoreUpdate()
{
	
}


void ACombatPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACombatPlayerState, PlayerMoney);
	DOREPLIFETIME(ACombatPlayerState, CurrentTeam);
	DOREPLIFETIME(ACombatPlayerState, PlayerAssistCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerDeathCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerKillCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerScore);
	DOREPLIFETIME(ACombatPlayerState, PlayerPlantCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerDefuseCount);
	DOREPLIFETIME(ACombatPlayerState, PlayerColor);
	DOREPLIFETIME(ACombatPlayerState, SelectedGadget);
	DOREPLIFETIME(ACombatPlayerState, IsDeadPreviousRound);
}

void ACombatPlayerState::OnRep_PlayerColor()
{
	//If color changes, notify everything through the GameState.
	if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		CombatGameState->OnRep_GamePlayers();
	}
}

void ACombatPlayerState::SetPlayerMoney(int32 NewMoney)
{
	if(HasAuthority())
	{
		PlayerMoney = NewMoney;
		OnRep_PlayerMoney();
	}
}

void ACombatPlayerState::ChangePlayerMoney(int32 DeltaMoney)
{
	if(HasAuthority())
	{
		SetPlayerMoney(GetPlayerMoney() + DeltaMoney);
	}
}

int32 ACombatPlayerState::GetPlayerMoney()
{
	return PlayerMoney;
}

int32 ACombatPlayerState::AddKill()
{
	if(HasAuthority())
	{
		PlayerKillCount++;
		CalculateScore();
		OnRep_ScoreUpdate();
	}
	
	return PlayerKillCount;
		
}

int32 ACombatPlayerState::AddDeath()
{
	if(HasAuthority())
	{
		PlayerDeathCount++;
		CalculateScore();
		OnRep_ScoreUpdate();
	}
	return PlayerDeathCount;
}

int32 ACombatPlayerState::AddAssist()
{
	if(HasAuthority())
	{
		PlayerAssistCount++;
		CalculateScore();
		OnRep_ScoreUpdate();
	}
	return PlayerAssistCount;
}

int32 ACombatPlayerState::AddPlant()
{
	if(HasAuthority())
	{
		PlayerPlantCount++;
		CalculateScore();
		OnRep_ScoreUpdate();
	}
	return PlayerPlantCount;
}

int32 ACombatPlayerState::AddDefuse()
{
	if(HasAuthority())
	{
		PlayerDefuseCount++;
		CalculateScore();
		OnRep_ScoreUpdate();
	}
	return PlayerDefuseCount;
}

int32 ACombatPlayerState::GetKills()
{
	return PlayerKillCount;
}

int32 ACombatPlayerState::GetDeaths()
{
	return PlayerDeathCount;
}

int32 ACombatPlayerState::GetAssists()
{
	return PlayerAssistCount;
}

int32 ACombatPlayerState::GetPlants()
{
	return PlayerPlantCount;
}

int32 ACombatPlayerState::GetDefuses()
{
	return PlayerDefuseCount;
}

int32 ACombatPlayerState::GetScores()
{
	return PlayerScore;
}


void ACombatPlayerState::CalculateScore()
{
	if(ACombatGameState* GameState = Cast<ACombatGameState>(GetWorld()->GetGameState()))
	{
		PlayerScore = GameState->GetBaseScore()-PlayerDeathCount*GameState->GetDeathValue()+PlayerKillCount*GameState->GetKillValue()+PlayerAssistCount*GameState->GetAssistValue()+(PlayerPlantCount+PlayerDefuseCount)*GameState->GetBombValue();
	}
}

void ACombatPlayerState::YouDied()
{
	if(HasAuthority() && !IsDeadPreviousRound)
	{
		IsDeadPreviousRound = true;
		OnRep_DiedPreviousRound();
	}
}

void ACombatPlayerState::ResetDeathFlag()
{
	if(HasAuthority() && IsDeadPreviousRound)
	{
		IsDeadPreviousRound = false;
		OnRep_DiedPreviousRound();
	}
	
}

bool ACombatPlayerState::GetDeathState()
{
	return IsDeadPreviousRound;
}

EGameTeams ACombatPlayerState::GetTeam()
{
	return CurrentTeam;
}

void ACombatPlayerState::SetTeam(EGameTeams NewTeam)
{
	if(HasAuthority())
	{
		CurrentTeam = NewTeam;
		PlayerColor = EPlayerColor::None;
		OnRep_PlayerColor();
		OnRep_GameTeam();
	}
}

void ACombatPlayerState::SetPlayerColor(EPlayerColor Color)
{
	if(HasAuthority())
	{
		if(ACombatGameState* CombatGameState = Cast<ACombatGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			if(GetTeam() == EGameTeams::None) return;
			for(auto& PlayerState : CombatGameState->GetAllPlayersOfTeam(GetTeam()))
			{
				if(PlayerState->GetPlayerColor() == Color)
				{
					return;
				}
			}
			PlayerColor = Color;
			OnRep_PlayerColor();
		}
	}else
	{
		Server_SetPlayerColor(Color);
	}
}

void ACombatPlayerState::Server_SetPlayerColor_Implementation(EPlayerColor Color)
{
	SetPlayerColor(Color);
}

EPlayerColor ACombatPlayerState::GetPlayerColor()
{
	return PlayerColor;
}



FLinearColor ACombatPlayerState::GetColorByEnum(EPlayerColor Color)
{
	if(Color == EPlayerColor::None) return FLinearColor::White;
	return *ColorMap.Find(Color);
}

void ACombatPlayerState::AssignGadget(EGadgetType Gadget, int32 Amount)
{
	if(HasAuthority())
	{
		SelectedGadget = FGadgetProperty();
		SelectedGadget.GadgetType = Gadget;
		SelectedGadget.NumberOfGadgets = Amount;
		OnRep_PlayerColor();
	}else
	{
		Server_AssignGadget(Gadget, Amount);
	}
}

void ACombatPlayerState::Server_AssignGadget_Implementation(EGadgetType Gadget, int32 Amount)
{
	AssignGadget(Gadget, Amount);
}


FGadgetProperty ACombatPlayerState::GetAssignedGadget()
{
	return SelectedGadget;
}

void ACombatPlayerState::OnRegisterScoreChangeComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
{
	if(bSuccess)
	{
		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());
		FJsonSerializer::Deserialize(Reader, ResponseJson);
		if(ResponseJson->GetStringField("status") != "ERROR")
		{
			const TSharedPtr<FJsonObject>* OutObj;
			ResponseJson->TryGetObjectField("msg", OutObj);
			AccountData.PlayerLevel = OutObj->Get()->GetIntegerField("level");
			AccountData.PlayerXP = OutObj->Get()->GetIntegerField("xp");
			AccountData.UnopenedCrates = OutObj->Get()->GetIntegerField("crates");

			API_GetPlayerAccountData();
		}
	}
}

void ACombatPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
}

void ACombatPlayerState::API_RegisterScoreChange()
{
	if(!HasAuthority())
	{
		return;
	}
	if(ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
		FString RequestURL = FString::Printf(TEXT("http://oregtolgy-panzio.com:3000/registerscore"));
		Request->SetVerb("POST");
		Request->SetURL(RequestURL);
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		Request->SetContentAsString(FString::Printf(TEXT("token=%s&utoken=%s&uid=%s&val=%d"), *DefaultGameMode->APIToken, *GetPlayerAuthToken(), *GetPlayerEpicID(), GetScores()));
		Request->OnProcessRequestComplete().BindUObject(this, &ACombatPlayerState::OnRegisterScoreChangeComplete);
		Request->ProcessRequest();
	}
}
