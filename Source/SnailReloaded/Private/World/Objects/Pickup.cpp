// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/Pickup.h"

#include "Components/BoxComponent.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("PickupInteraction"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));

	SetRootComponent(BoxCollision);
	InteractionComponent->SetupAttachment(BoxCollision);
	SkeletalMesh->SetupAttachment(GetRootComponent());
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetEnableGravity(true);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);


}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

