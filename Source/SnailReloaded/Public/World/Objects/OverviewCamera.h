// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "OverviewCamera.generated.h"

UCLASS()
class SNAILRELOADED_API AOverviewCamera : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOverviewCamera();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
