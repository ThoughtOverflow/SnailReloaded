// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Holostand.generated.h"

UCLASS()
class SNAILRELOADED_API AHolostand : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* HolopadMesh;
	
public:	
	// Sets default values for this actor's properties
	AHolostand();

	UPROPERTY(BlueprintReadWrite)
	bool bPlayCaseOpening;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlayCaseOpening();

};
