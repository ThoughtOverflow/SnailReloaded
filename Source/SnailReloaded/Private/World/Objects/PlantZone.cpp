// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/PlantZone.h"

#include "Components/BoxComponent.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"


// Sets default values
APlantZone::APlantZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AssignedSite = EPlantSite::None;
	PlantZone = CreateDefaultSubobject<UBoxComponent>(TEXT("PlantZone"));
	PlantZone->OnComponentBeginOverlap.AddDynamic(this, &APlantZone::PlayerEntered);
	PlantZone->OnComponentEndOverlap.AddDynamic(this, &APlantZone::PlayerLeft);

}

// Called when the game starts or when spawned
void APlantZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlantZone::PlayerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->HasAuthority())
	{
		if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(OtherActor))
		{
			PlayerCharacter->SetIsInPlantZone(true);
		}
	}
}

void APlantZone::PlayerLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor->HasAuthority())
	{
		if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(OtherActor))
		{
			PlayerCharacter->SetIsInPlantZone(false);
		}
	}
}

// Called every frame
void APlantZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

