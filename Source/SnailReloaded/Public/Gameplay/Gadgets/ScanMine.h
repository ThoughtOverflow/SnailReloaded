// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Gameplay/Gadgets/Gadget.h"
#include "ScanMine.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class SNAILRELOADED_API AScanMine : public AGadget
{
	GENERATED_BODY()

public:

	AScanMine();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mine properties")
	float BootupTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mine properties")
	float DetectionTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mine properties")
	USphereComponent* DetectionSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mine properties")
	UBoxComponent* RootBox;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void Initialize();

protected:

	UFUNCTION(BlueprintCallable)
	void PlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable)
	void OnBoxCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit);

	bool bAllowScanning;

	UFUNCTION()
	void ScanInitialize_Callback();

	UPROPERTY()
	FTimerHandle BootupTimer;
	
};
