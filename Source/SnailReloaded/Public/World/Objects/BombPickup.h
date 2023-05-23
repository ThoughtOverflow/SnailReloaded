// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BombPickupInteraction.h"
#include "World/Objects/Pickup.h"
#include "BombPickup.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ABombPickup : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABombPickup();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInteractionComponent* InteractionComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* BoxCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float PickupGlobalScale;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION()
	void OnPickupInteract(APawn* Interactor);
	UFUNCTION()
	void OnBoxCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  FVector NormalImpulse, const FHitResult& Hit);
};
	
	

