// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/BombPickup.h"

#include "Components/BoxComponent.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

ABombPickup::ABombPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupGlobalScale = 1.f;

	bReplicates = true;
	SetReplicateMovement(true);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	InteractionComponent = CreateDefaultSubobject<UBombPickupInteraction>(TEXT("PickupInteraction"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));

	SetRootComponent(BoxCollision);
	InteractionComponent->SetupAttachment(SkeletalMesh);
	SkeletalMesh->SetupAttachment(GetRootComponent());
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetEnableGravity(true);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	BoxCollision->SetNotifyRigidBodyCollision(true);
	BoxCollision->OnComponentHit.AddDynamic(this, &ABombPickup::OnBoxCollide);
	
	InteractionComponent->OnInteract.AddDynamic(this, &ABombPickup::OnPickupInteract);
	
}

void ABombPickup::BeginPlay()
{
	Super::BeginPlay();
}


void ABombPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABombPickup::OnPickupInteract(APawn* Interactor)
{
	if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(Interactor))
	{
		if(DefaultPlayerCharacter->HasAuthority())
		{
			DefaultPlayerCharacter->SetHasBomb(true);
			if(HasAuthority())
			{
				if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
				{
					StandardCombatGameState->GetMinimapDefinition()->SetShowDroppedBombMarker(false);
				}
			}
			Destroy();
		}
	}
}

void ABombPickup::OnBoxCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	NormalImpulse.Normalize();
	float degZ = FMath::RadiansToDegrees(FMath::Acos(NormalImpulse.Dot(FVector(0.f, 0.f, 1.f))));
	if(degZ <= 30.f)
	{
		BoxCollision->SetSimulatePhysics(false);
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		if(HasAuthority())
		{
			if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
			{
				StandardCombatGameState->GetMinimapDefinition()->SetShowDroppedBombMarker(true);
			}
		}
	}
}



