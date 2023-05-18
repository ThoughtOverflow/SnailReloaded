// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/BombPickup.h"

ABombPickup::ABombPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionComponent =CreateDefaultSubobject<UBombPickupInteraction>(TEXT("BombPickup"));
	SkeletalMesh =CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BombSkeletalMesh"));

	SetRootComponent(SkeletalMesh);
	InteractionComponent->SetupAttachment(SkeletalMesh);
	
}

void ABombPickup::BeginPlay()
{
	Super::BeginPlay();
}


void ABombPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



