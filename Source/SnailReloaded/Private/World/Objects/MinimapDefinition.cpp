// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/MinimapDefinition.h"

#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMinimapDefinition::AMinimapDefinition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	MinimapArea = CreateDefaultSubobject<UBoxComponent>(TEXT("MinimapArea"));
	MinimapArea->SetGenerateOverlapEvents(false);
	
}

// Called when the game starts or when spawned
void AMinimapDefinition::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMinimapDefinition::OnRep_MapMarkers()
{
	OnMapMarkersUpdated.Broadcast();
}

void AMinimapDefinition::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMinimapDefinition, bShowPlantedBomb);
	DOREPLIFETIME(AMinimapDefinition, bShowDroppedBomb);
	DOREPLIFETIME(AMinimapDefinition, BombLoc);
}

// Called every frame
void AMinimapDefinition::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector2D AMinimapDefinition::TranslateWorldLocation(FVector ObjectLocation)
{
	FVector P1Loc = (GetActorLocation() - FVector(MinimapArea->GetScaledBoxExtent())) * FVector(1.f, 1.f, 0.f);
	FVector P2Loc = (GetActorLocation() + FVector(MinimapArea->GetScaledBoxExtent())) * FVector(1.f, 1.f, 0.f);

	float ObjectXClamped = FMath::Clamp(ObjectLocation.X, P1Loc.X, P2Loc.X);
	float ObjectYClamped = FMath::Clamp(ObjectLocation.Y, P1Loc.Y, P2Loc.Y);

	FVector RelativeLocation = FVector(ObjectXClamped - P1Loc.X, ObjectYClamped - P1Loc.Y, 0.f);
	FVector MapArea2d = (P2Loc - P1Loc) * FVector(1.f, 1.f, 0.f);

	return FVector2D(RelativeLocation.X / MapArea2d.X, RelativeLocation.Y / MapArea2d.Y);
	
}

float AMinimapDefinition::TranslateWorldRotation(FVector ForwardVector)
{

	float DotX = ForwardVector.Dot(FVector(1.f, 0.f, 0.f));
	float DotY = ForwardVector.Dot(FVector(0.f, 1.f, 0.f));

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotX));
	if(DotY < 0.f)
	{
		Angle = 360.f - Angle;
	}
	return Angle;
}

void AMinimapDefinition::SetShowPlantMarker(bool bShow)
{
	if(HasAuthority())
	{
		bShowPlantedBomb = bShow;
		OnRep_MapMarkers();
	}
}

bool AMinimapDefinition::ShouldShowPlantMarker()
{
	return bShowPlantedBomb;
}

void AMinimapDefinition::SetShowDroppedBombMarker(bool bShow)
{
	if(HasAuthority())
	{
		bShowDroppedBomb = bShow;
		OnRep_MapMarkers();
	}
}

bool AMinimapDefinition::ShouldShowDroppedBombMarker()
{
	return bShowDroppedBomb;
}

void AMinimapDefinition::SetBombLocation(FVector NewLoc)
{
	if(HasAuthority())
	{
		BombLoc = NewLoc;
		OnRep_MapMarkers();
	}
}

FVector AMinimapDefinition::GetBombLocation()
{
	return BombLoc;
}


