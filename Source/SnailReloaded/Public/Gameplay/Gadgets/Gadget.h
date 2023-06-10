// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gadget.generated.h"

UCLASS()
class SNAILRELOADED_API AGadget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGadget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
