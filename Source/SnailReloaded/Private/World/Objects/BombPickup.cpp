// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/BombPickup.h"

#include "Components/BoxComponent.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"

ABombPickup::ABombPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupGlobalScale = 1.f;

	bReplicates = true;
	SetReplicateMovement(true);
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("PickupInteraction"));
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
			Destroy();
		}
	}
}



