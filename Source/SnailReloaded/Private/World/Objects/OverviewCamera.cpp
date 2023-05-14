// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/OverviewCamera.h"

// Sets default values
AOverviewCamera::AOverviewCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

}

// Called when the game starts or when spawned
void AOverviewCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOverviewCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

