// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Gadgets/Gadget.h"

#include "Framework/Combat/CombatPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Net/UnrealNetwork.h"

FGadgetProperty::FGadgetProperty()
{
	GadgetType = EGadgetType::None;
	NumberOfGadgets = 1;
	PlacedGadgets = 0;
	SpawnedGadgetPtr = nullptr;
}

int32 FGadgetProperty::GetRemainingGadgets()
{
	return FMath::Max(0, NumberOfGadgets - PlacedGadgets);
}

// Sets default values
AGadget::AGadget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	bUsePlacementMode = false;
	bGadgetInPlacementMode = false;
	bGadgetInitialized = false;
	
	GadgetHealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("GadgetHeath"));
	GadgetHealthComponent->DefaultObjectHealth = 100.f;
	GadgetHealthComponent->SetObjectMaxHealth(100.f);
	GadgetHealthComponent->OnTeamQuery.BindDynamic(this, &AGadget::GetOwningTeam);
	GadgetHealthComponent->ObjectKilled.AddDynamic(this, &AGadget::OnGadgetDestroyed);
	GadgetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GadgetMesh"));
	SetRootComponent(GadgetMesh);

}

// Called when the game starts or when spawned
void AGadget::BeginPlay()
{
	Super::BeginPlay();
	
}

ADefaultPlayerCharacter* AGadget::TryGetOwningCharacter()
{
	if(OwningPlayerState && OwningPlayerState->GetPawn())
	{
		return Cast<ADefaultPlayerCharacter>(OwningPlayerState->GetPawn());	
	}
	return nullptr;
}

EGameTeams AGadget::GetOwningTeam()
{
	return OwningPlayerState->GetTeam();
}

void AGadget::OnGadgetDestroyed(const FDamageResponse& LatestDamage)
{
	this->Destroy();
}

void AGadget::OnInitialized()
{
	if(HasAuthority())
	{
		bGadgetInitialized = true;
		GadgetMesh->SetOnlyOwnerSee(false);
		if(bUsePlacementMode)
		{
			if(ADefaultPlayerCharacter* OwningPlayer = Cast<ADefaultPlayerCharacter>(OwningPlayerState->GetPawn()))
			{
				//Clear the spawned gadget reference, so a new gadget (with placement mode enabled) can be spawned.
				OwningPlayer->AssignedGadget.SpawnedGadgetPtr = nullptr;
			}
		}
	}
}

void AGadget::EnteredPlacementMode(ACombatPlayerState* OwningState)
{
	//Set the state early;
	if(HasAuthority())
	{
		OwningPlayerState = OwningState;
		bGadgetInPlacementMode = true;
		GadgetMesh->SetOnlyOwnerSee(true);
	}
}

void AGadget::CancelledPlacementMode(ACombatPlayerState* OwningState)
{
	//Destroy the actor;
	if(HasAuthority())
	{
		bGadgetInPlacementMode = false;
		Destroy();
		if(ADefaultPlayerCharacter* OwningPlayer = Cast<ADefaultPlayerCharacter>(OwningPlayerState->GetPawn()))
		{
			//Clear the spawned gadget reference, so a new gadget (with placement mode enabled) can be spawned.
			OwningPlayer->AssignedGadget.SpawnedGadgetPtr = nullptr;
		}
	}
}

void AGadget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGadget, OwningPlayerState);
	DOREPLIFETIME(AGadget, bGadgetInPlacementMode);
	DOREPLIFETIME(AGadget, bGadgetInitialized);
}

// Called every frame
void AGadget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGadget::InitializeGadget(ACombatPlayerState* OwningState)
{
	if(HasAuthority())
	{
		OwningPlayerState = OwningState;
		if(OwningPlayerState)
		{
			if(ADefaultPlayerCharacter* OwningCharacter = Cast<ADefaultPlayerCharacter>(OwningState->GetPlayerController()->GetPawn()))
			{
				OwningCharacter->AssignedGadget.PlacedGadgets++;
				OnInitialized();
			}			
		}
	}
	
}

bool AGadget::IsGadgetInitialized()
{
	return bGadgetInitialized;
}

bool AGadget::IsGadgetInPlacementMode()
{
	return bGadgetInPlacementMode;
}

ACombatPlayerState* AGadget::GetOwningPlayerState()
{
	return OwningPlayerState;
}

