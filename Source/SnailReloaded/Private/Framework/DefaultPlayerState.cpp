// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerState.h"

#include "HttpModule.h"
#include "Framework/DefaultGameMode.h"
#include "Framework/SnailGameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

FAPIAccountData::FAPIAccountData()
{
	PlayerLevel = 0;
	PlayerXP = 0;
	UnopenedCrates = 0;
	XpPerLevel = 1000;
}

ADefaultPlayerState::ADefaultPlayerState()
{
	
}

void ADefaultPlayerState::API_GetPlayerAccountData()
{
	if(!HasAuthority())
	{
		return;
	}
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FString RequestURL = FString::Printf(TEXT("http://oregtolgy-panzio.com:3000/queryaccount?uid=%s"), *GetPlayerEpicID());
	Request->SetVerb("GET");
	Request->SetURL(RequestURL);
	Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnGetAccountDataRequestComplete);
	Request->ProcessRequest();
}

void ADefaultPlayerState::API_GetPlayerInventoryData()
{
	if(!HasAuthority())
	{
		return;
	}
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FString RequestURL = FString::Printf(TEXT("http://oregtolgy-panzio.com:3000/getitems?uid=%s"), *GetPlayerEpicID());
	Request->SetVerb("GET");
	Request->SetURL(RequestURL);
	Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnGetItemsRequestComplete);
	Request->ProcessRequest();
}

void ADefaultPlayerState::API_ValidateToken()
{
	if(!HasAuthority())
	{
		return;
	}
	if(ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
		FString RequestURL = FString::Printf(TEXT("http://oregtolgy-panzio.com:3000/validatetoken"));
		Request->SetVerb("POST");
		Request->SetURL(RequestURL);
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		Request->SetContentAsString(FString::Printf(TEXT("token=%s"), *DefaultGameMode->APIToken));
		Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnGetTokenValidationComplete);
		Request->ProcessRequest();
	}
	
}

void ADefaultPlayerState::API_ValidateUser()
{
	if(!HasAuthority())
	{
		return;
	}
	if(ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
		FString RequestURL = FString::Printf(TEXT("http://oregtolgy-panzio.com:3000/createuser"));
		Request->SetVerb("POST");
		Request->SetURL(RequestURL);
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		Request->SetContentAsString(FString::Printf(TEXT("utoken=%s"), *GetPlayerAuthToken()));
		Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnAccountValidationComplete);
		Request->ProcessRequest();
	}
}


void ADefaultPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<USnailGameInstance>(GetGameInstance())->EpicLoginComplete.AddDynamic(this, &ADefaultPlayerState::OnLoginComplete);
}

void ADefaultPlayerState::OnLoginComplete()
{
	if(USnailGameInstance* SnailGameInstance = Cast<USnailGameInstance>(GetGameInstance()))
	{
		SnailGameInstance->MainStatusMessage = TEXT("Query API data");
		API_ValidateUser();
	}

}

FString ADefaultPlayerState::GetPlayerEpicID()
{
	return Cast<USnailGameInstance>(GetGameInstance())->GetPlayerEpicID();
}

FString ADefaultPlayerState::GetPlayerAuthToken()
{
	return Cast<USnailGameInstance>(GetGameInstance())->GetPlayerAuthToken();
}

void ADefaultPlayerState::OnGetItemsRequestComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
{
	if(bSuccess)
	{
		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());
		FJsonSerializer::Deserialize(Reader, ResponseJson);
		if(ResponseJson->GetStringField("status") != "ERROR")
		{
			PlayerOwnedItems.Empty();
			const TArray<TSharedPtr<FJsonValue>>* ItemsArray;
			ResponseJson->TryGetArrayField("msg", ItemsArray);
			for(const TSharedPtr<FJsonValue>& Item : *ItemsArray)
			{
				FAPIItemData ItemData;
				ItemData.ItemId = Item->AsObject()->GetStringField("id");
				ItemData.GroupId = Item->AsObject()->GetStringField("group");
				ItemData.bItemEquipped = Item->AsObject()->GetBoolField("equipped");
				PlayerOwnedItems.Add(ItemData);
			}
			
			
			if(USnailGameInstance* SnailGameInstance = Cast<USnailGameInstance>(GetGameInstance()))
			{
				SnailGameInstance->SetApiInventoryDataReady(true);
			}
		}
	}
}

void ADefaultPlayerState::OnGetAccountDataRequestComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
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

			if(USnailGameInstance* SnailGameInstance = Cast<USnailGameInstance>(GetGameInstance()))
			{
				SnailGameInstance->SetApiAccountDataReady(true);
			}
		}
	}
}

void ADefaultPlayerState::OnGetTokenValidationComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
{
	if(bSuccess)
	{
		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Res->GetContentAsString());
		FJsonSerializer::Deserialize(Reader, ResponseJson);
		if(ResponseJson->GetStringField("status") != "ERROR")
		{
			bool Valid = false;
			ResponseJson->TryGetBoolField("msg", Valid);

			if(USnailGameInstance* SnailGameInstance = Cast<USnailGameInstance>(GetGameInstance()))
			{
				SnailGameInstance->bIsAuthServer = Valid;
			}
		}
	}
}

void ADefaultPlayerState::OnAccountValidationComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
{
	API_ValidateToken();
	API_GetPlayerAccountData();
	API_GetPlayerInventoryData();
}

void ADefaultPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if(ADefaultPlayerState* DefaultPlayerState = Cast<ADefaultPlayerState>(PlayerState))
	{
		DefaultPlayerState->AccountData = AccountData;
	}
}
