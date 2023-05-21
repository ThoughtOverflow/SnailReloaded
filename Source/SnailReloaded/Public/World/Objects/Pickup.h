// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Weapons/WeaponBase.h"
#include "Pickup.generated.h"

class UBoxComponent;
UCLASS()
class SNAILRELOADED_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInteractionComponent* InteractionComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* BoxCollision;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AWeaponBase> WeaponClass;
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentWeaponClipAmmo;
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentWeaponTotalAmmo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPickupInteract(APawn* Interactor);

};
