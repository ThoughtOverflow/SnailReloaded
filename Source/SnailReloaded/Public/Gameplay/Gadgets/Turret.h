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

	virtual void OnInitialized() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
};


