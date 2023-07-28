// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "Framework/Combat/CombatPlayerState.h"
#include "GameFramework/Actor.h"
#include "Gadget.generated.h"

UENUM(BlueprintType)
enum class EGadgetType : uint8
{
	None = 0,
	ScanMine = 1,
	Turret = 2
};

USTRUCT(BlueprintType)
struct FGadgetProperty
{
	GENERATED_BODY()

public:

	FGadgetProperty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EGadgetType GadgetType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 NumberOfGadgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 PlacedGadgets;

	int32 GetRemainingGadgets();
	
};

class ADefaultPlayerCharacter;
class ACombatPlayerState;

UCLASS()
class SNAILRELOADED_API AGadget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGadget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UHealthComponent* GadgetHealthComponent;

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
	UFUNCTION()
	virtual void OnInitialized();
	
	UPROPERTY()
	bool bGadgetInitialized;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	ACombatPlayerState* OwningPlayerState;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void InitializeGadget(ACombatPlayerState* OwningState);
	UFUNCTION(BlueprintPure)
	bool IsGadgetInitialized();
	UFUNCTION(BlueprintPure)
	ACombatPlayerState* GetOwningPlayerState();


};
