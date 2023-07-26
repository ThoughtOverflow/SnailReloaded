// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ComputerCase.generated.h"

UCLASS()
class SNAILRELOADED_API AComputerCase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComputerCase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimationAsset* OpenAnim;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AnimOpenCase();

};
