// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/Holostand.h"

// Sets default values
AHolostand::AHolostand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bPlayCaseOpening = false;
	HolopadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HolopadMesh"));
	SetRootComponent(HolopadMesh);

}

// Called when the game starts or when spawned
void AHolostand::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHolostand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHolostand::PlayCaseOpening()
{
	bPlayCaseOpening = true;
}

