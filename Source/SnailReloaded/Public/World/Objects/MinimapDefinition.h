// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapDefinition.generated.h"

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector2D TranslateWorldLocation(FVector ObjectLocation);

};
