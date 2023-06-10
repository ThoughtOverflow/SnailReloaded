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

protected:


	
};
