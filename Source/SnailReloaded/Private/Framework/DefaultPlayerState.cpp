// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerState.h"

#include "HttpModule.h"
#include "Framework/SnailGameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

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
		API_GetPlayerAccountData();
		API_GetPlayerInventoryData();
	}

}

FString ADefaultPlayerState::GetPlayerEpicID()
{
	return Cast<USnailGameInstance>(GetGameInstance())->GetPlayerEpicID();
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
			ResponseJson->TryGetStringArrayField("msg", PlayerOwnedItems);
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
