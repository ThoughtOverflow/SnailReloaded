// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "World/Objects/MinimapDefinition.h"

#include "Components/BoxComponent.h"

// Sets default values
AMinimapDefinition::AMinimapDefinition()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MinimapArea = CreateDefaultSubobject<UBoxComponent>(TEXT("MinimapArea"));
	
}

// Called when the game starts or when spawned
void AMinimapDefinition::BeginPlay()
{
	Super::BeginPlay();
	
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

float AMinimapDefinition::TranslateWorldRotation(FRotator ObjectRotation)
{
	FVector RotVector = ObjectRotation.Vector() * FVector(1.f, 1.f, 0.f);

	float DotX = RotVector.Dot(FVector(1.f, 0.f, 0.f));
	float DotY = RotVector.Dot(FVector(0.f, 1.f, 0.f));

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotX));
	if(DotY < 0.f)
	{
		Angle = 360.f - Angle;
	}
	return Angle;
}


