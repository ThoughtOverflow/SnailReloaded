// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/ComputerCase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AComputerCase::AComputerCase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	bOpenCase = false;

}

// Called when the game starts or when spawned
void AComputerCase::BeginPlay()
{
	Super::BeginPlay();
	OriginalLocation = GetActorLocation();
}

// Called every frame
void AComputerCase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


