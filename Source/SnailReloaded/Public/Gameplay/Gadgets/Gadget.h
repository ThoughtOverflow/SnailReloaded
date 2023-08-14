// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Gadget.generated.h"

UENUM(BlueprintType)
enum class EGadgetType : uint8
{
	None = 0,
	ScanMine = 1,
	Turret = 2
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bUsePlacementMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gadget Properties", meta = (EditCondition = bUsePlacementMode, EditConditionHides = true))
	float GadgetPlacementDistance;
	/**
	* @brief A padding to assign when placing the actor close to a wall or an obstacle.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gadget Properties", meta = (EditCondition = bUsePlacementMode, EditConditionHides = true))
	float ActorRadialPadding;

	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> DefaultMaterials;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gadget Properties", meta = (EditCondition = bUsePlacementMode, EditConditionHides = true))
	UMaterialInterface* PlacementAllowedMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gadget Properties", meta = (EditCondition = bUsePlacementMode, EditConditionHides = true))
	UMaterialInterface* PlacementBlockedMaterial;
	
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
	
	UPROPERTY(ReplicatedUsing = OnRep_MaterialChange)
	bool bGadgetInitialized;
	UPROPERTY(ReplicatedUsing = OnRep_MaterialChange)
	bool bGadgetInPlacementMode;
	UPROPERTY(ReplicatedUsing = OnRep_MaterialChange)
	bool bGadgetPlacementBlocked;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated)
	ACombatPlayerState* OwningPlayerState;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_MaterialChange();

	UFUNCTION()
	void UpdatePlacementLocation();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void InitializeGadget(ACombatPlayerState* OwningState);
	UFUNCTION(BlueprintPure)
	bool IsGadgetInitialized();
	UFUNCTION(BlueprintPure)
	bool IsGadgetInPlacementMode();
	UFUNCTION(BlueprintPure)
	ACombatPlayerState* GetOwningPlayerState();
	UFUNCTION()
	virtual void EnteredPlacementMode(ACombatPlayerState* OwningState);
	UFUNCTION()
	virtual void CancelledPlacementMode(ACombatPlayerState* OwningState);


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

	UPROPERTY(BlueprintReadWrite)
	AGadget* SpawnedGadgetPtr;

	int32 GetRemainingGadgets();
	
};
