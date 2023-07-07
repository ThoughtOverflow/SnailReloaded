// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkinDisplayActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
UCLASS()
class SNAILRELOADED_API ASkinDisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkinDisplayActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* BaseSkeleton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* HeadgearMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UNiagaraComponent* DeathEffectParticles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void EquipHeadgearToDummy(USkeletalMesh* Mesh);

};
