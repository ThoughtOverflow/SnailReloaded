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
	bGadgetInitialized = true;
}

void AGadget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGadget, OwningPlayerState);
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
			OnInitialized();
		}
	}
	
}

bool AGadget::IsGadgetInitialized()
{
	return bGadgetInitialized;
}

ACombatPlayerState* AGadget::GetOwningPlayerState()
{
	return OwningPlayerState;
}

