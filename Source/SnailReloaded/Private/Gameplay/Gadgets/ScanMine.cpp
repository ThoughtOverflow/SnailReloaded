// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/Gadgets/ScanMine.h"

#include "Components/SphereComponent.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

AScanMine::AScanMine()
{
	GadgetHealthComponent->GameObjectType.ObjectType = EGameObjectTypes::ScanMine;
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBox"));
	SetRootComponent(RootBox);
	GadgetMesh->SetupAttachment(GetRootComponent());
	DetectionSphere->SetSphereRadius(200.f);
	DetectionSphere->SetupAttachment(GetRootComponent());
	GadgetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootBox->SetGenerateOverlapEvents(true);
	RootBox->SetCollisionResponseToAllChannels(ECR_Block);
	RootBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AScanMine::PlayerEntered);
	// DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AScanMine::PlayerExit);
}

void AScanMine::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<AActor*> Actors;
	DetectionSphere->GetOverlappingActors(Actors, ADefaultPlayerCharacter::StaticClass());
	for(auto& Actor : Actors)
	{
		//check for line of sight:
		ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(Actor);
		FHitResult HitResult;
		FVector TraceStartLoc = GetActorLocation();
		FVector TraceEndLoc = PlayerCharacter->GetActorLocation();
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(PlayerCharacter);
		// if(!OwningPlayerState->GetOwningController()) return;
		if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceEndLoc, TraceStartLoc, ECC_Visibility, Params))
		{
			if(HitResult.GetActor() == this && Cast<ACombatPlayerState>(PlayerCharacter->GetPlayerState())->GetTeam() != GetOwningTeam())
			{
				//We hit the player - it is in line of sight
				UE_LOG(LogTemp, Warning, TEXT("Revealed: %s"), *PlayerCharacter->GetName());
			}
		}
	}
	
}

