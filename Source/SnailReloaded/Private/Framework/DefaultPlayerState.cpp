// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultPlayerState.h"

#include "HttpModule.h"
#include "Framework/DefaultGameMode.h"
#include "Framework/SnailGameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "World/Objects/SkinDisplayActor.h"

FAPIAccountData::FAPIAccountData()
{
	PlayerLevel = 0;
	PlayerXP = 0;
	UnopenedCrates = 0;
	XpPerLevel = 1000;
}

FAPIItemData::FAPIItemData()
{
	bItemEquipped = false;
	GroupId = "0";
	ItemId = "0";
	OutfitProperties = FOutfitData();
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
	FString RequestURL = GetActiveAPIAdress(FString::Printf(TEXT("queryaccount?uid=%s"), *GetPlayerEpicID()));
	Request->SetVerb("GET");
	Request->SetURL(RequestURL);
	Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnGetAccountDataRequestComplete);
	Request->ProcessRequest();
}

void ADefaultPlayerState::API_EquipOutfit(FString ItemID)
{
	if(!HasAuthority())
	{
		return;
	}
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FString RequestURL = GetActiveAPIAdress(TEXT("equipitem"));
	Request->SetVerb("POST");
	Request->SetURL(RequestURL);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetContentAsString(FString::Printf(TEXT("utoken=%s&item_id=%s"), *GetPlayerAuthToken(), *ItemID));
	Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnEquipmentComplete);
	Request->ProcessRequest();
}

void ADefaultPlayerState::API_UnequipOutfit(FString ItemID)
{
	if(!HasAuthority())
	{
		return;
	}
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FString RequestURL = GetActiveAPIAdress(TEXT("unequipitem"));
	Request->SetVerb("POST");
	Request->SetURL(RequestURL);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	Request->SetContentAsString(FString::Printf(TEXT("utoken=%s&item_id=%s"), *GetPlayerAuthToken(), *ItemID));
	Request->OnProcessRequestComplete().BindUObject(this, &ADefaultPlayerState::OnEquipmentComplete);
	Request->ProcessRequest();
}

void ADefaultPlayerState::OnEquipmentComplete(FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
{
	if(bSuccess)
	{
		API_GetPlayerInventoryData();
	}
}

void ADefaultPlayerState::API_GetPlayerInventoryData()
{
	if(!HasAuthority())
	{
		return;
	}
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FString RequestURL = GetActiveAPIAdress(FString::Printf(TEXT("getitems?uid=%s"), *GetPlayerEpicID()));
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
		FString RequestURL = GetActiveAPIAdress(TEXT("validatetoken"));
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
		FString RequestURL = GetActiveAPIAdress(TEXT("createuser"));
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

void ADefaultPlayerState::DisplayEquippedOutfits()
{
	if(HasAuthority())
	{
		if(ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			if(ASkinDisplayActor* FoundSkinModel = Cast<ASkinDisplayActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ASkinDisplayActor::StaticClass())))
			{
				//clear everyting:
				FoundSkinModel->HeadgearMesh->SetSkeletalMesh(nullptr);
				for(auto& Item : PlayerOwnedItems)
				{
					if(Item.bItemEquipped && Item.GroupId == "HEAD")
					{
						if(USkeletalMesh* AssignedMesh = *DefaultGameMode->HeadgearAssets.Find(Item.ItemId))
						{
							FoundSkinModel->EquipHeadgearToDummy(AssignedMesh);
						}
					}
				}
			}
		}
	}
}

void ADefaultPlayerState::OnRep_PlayerItems()
{
	OnPlayerOwnedItemsUpdated.Broadcast();
	DisplayEquippedOutfits();
}

FString ADefaultPlayerState::GetActiveAPIAdress(FString command)
{
	if(USnailGameInstance* SnailGameInstance = Cast<USnailGameInstance>(GetGameInstance()))
	{
		FString url = SnailGameInstance->bUseAlternateAPI ? SnailGameInstance->AlternateAPIWebAddress : SnailGameInstance->APIWebAddress;
		url.Append(FString::Printf(TEXT(":%d/%s"), SnailGameInstance->APIPort, *command));
		return url;
	}
	return "";
	
}

void ADefaultPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ADefaultPlayerState, PlayerOwnedItems, COND_OwnerOnly);
	DOREPLIFETIME(ADefaultPlayerState, AccountData);
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
				if(ADefaultGameMode* DefaultGameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					if(FOutfitData* FoundData = DefaultGameMode->RegisteredOutfits.Find(ItemData.ItemId))
					{
						ItemData.OutfitProperties = FOutfitData(FoundData->OutfitName, FoundData->OutfitRarity,FoundData->Thumbnail);
					}
				}
				PlayerOwnedItems.Add(ItemData);
			}
			OnRep_PlayerItems();
			
			
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
		DefaultPlayerState->PlayerOwnedItems = PlayerOwnedItems;
	}
}
