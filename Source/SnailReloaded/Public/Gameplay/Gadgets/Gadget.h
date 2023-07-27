// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "GameFramework/Actor.h"
#include "Gadget.generated.h"

class ADefaultPlayerCharacter;
UCLASS()
class SNAILRELOADED_API AGadget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGadget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHealthComponent* GadgetHealthComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	ACombatPlayerState* OwningPlayerState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* GadgetMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	ADefaultPlayerCharacter* TryGetOwningCharacter();
	UFUNCTION()
	EGameTeams GetOwningTeam();
	UFUNCTION()
	void OnGadgetDestroyed(const FDamageResponse& LatestDamage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
