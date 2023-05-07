// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyManny.generated.h"

class UTextRenderComponent;
class UHealthComponent;
class UCapsuleComponent;
UCLASS()
class SNAILRELOADED_API ADummyManny : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADummyManny();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Manny")
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Manny")
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Manny")
	UHealthComponent* DummyHealthComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Manny")
	UTextRenderComponent* TextRenderComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnKilled(const FDamageResponse& LatestDamage);
	UFUNCTION()
	void OnHealthChanged(const FDamageResponse& LatestDamage);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
