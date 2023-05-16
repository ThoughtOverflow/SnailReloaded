// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapDefinition.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapMarkersUpdated);

class UBoxComponent;
UCLASS()
class SNAILRELOADED_API AMinimapDefinition : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinimapDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBoxComponent* MinimapArea;

	UPROPERTY(BlueprintAssignable)
	FOnMapMarkersUpdated OnMapMarkersUpdated;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_MapMarkers();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UI stuff:

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_MapMarkers)
	bool bShowPlantedBomb;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_MapMarkers)
	FVector BombLoc;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector2D TranslateWorldLocation(FVector ObjectLocation);
	UFUNCTION(BlueprintCallable)
	float TranslateWorldRotation(FVector ForwardVector);
	
	UFUNCTION(BlueprintCallable)
	void SetShowPlantMarker(bool bShow);
	UFUNCTION(BlueprintPure)
	bool ShouldShowPlantMarker();
	UFUNCTION(BlueprintCallable)
	void SetBombLocation(FVector NewLoc);
	UFUNCTION(BlueprintPure)
	FVector GetBombLocation();
	

};
