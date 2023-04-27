// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlantZone.generated.h"

class UBoxComponent;
UENUM(BlueprintType)
enum class EPlantSite : uint8
{
	None = 0,
	Site_A = 1,
	Site_B = 2,
	Site_C = 3,
	Site_D = 4,
	Site_E = 5,
};

UCLASS()
class SNAILRELOADED_API APlantZone : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APlantZone();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone settings")
	EPlantSite AssignedSite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* PlantZone;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void PlayerLeft(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
