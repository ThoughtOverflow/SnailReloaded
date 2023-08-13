// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Gadgets/Gadget.h"
#include "Components/SphereComponent.h"
#include "Turret.generated.h"

/**
 * 
 */
UCLASS()
class SNAILRELOADED_API ATurret : public AGadget
{
	GENERATED_BODY()

	ATurret();
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* SightRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	AActor* CurrentTarget;
	UPROPERTY(BlueprintReadWrite)
	FTimerHandle ShootingTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Properties")
	float TurretPlacementDistance;
	
	/**
	 * @brief A padding to assign when placing the actor close to a wall or an obstacle.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Properties")
	float ActorRadialPadding;


	UFUNCTION(BlueprintCallable)
	void FireTurret();
	UFUNCTION(BlueprintCallable)
	void InitiateShooting();

	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable)
	void PlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable)
	void PlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION(BlueprintCallable)
	void CheckTarget();

	UFUNCTION()
	void UpdatePlacementLocation();

	virtual void OnInitialized() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CancelledPlacementMode(ACombatPlayerState* OwningState) override;
	virtual void EnteredPlacementMode(ACombatPlayerState* OwningState) override;
	
	
};

