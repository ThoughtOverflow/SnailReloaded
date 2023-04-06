// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ACombatPlayerController;
USTRUCT(BlueprintType)
struct FDamageRequest
{
	GENERATED_BODY()

public:
	
	FDamageRequest();

	static FDamageRequest Initialize(UHealthComponent* HealthComponent);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACombatPlayerController* SourcePlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaDamage;
	
	
};

USTRUCT(BlueprintType)
struct FDamageResponse
{
	GENERATED_BODY()

public:
	
	FDamageResponse();
	FDamageResponse(ACombatPlayerController* SrcPlayer, float DeltaHealth, float NewHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACombatPlayerController* SourcePlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NewHealth;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectHealthChanged, FDamageResponse, DamageResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectKilled, FDamageResponse, LatestDamage);

UCLASS( ClassGroup=(HealthSystem), meta=(BlueprintSpawnableComponent) )
class SNAILRELOADED_API UHealthComponent : public UActorComponent
{
public:
	

private:
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float DefaultObjectHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FDamageResponse LatestDamage;

	UPROPERTY(BlueprintAssignable)
	FObjectHealthChanged ObjectHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FObjectKilled ObjectKilled;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	UPROPERTY(Replicated)
	float ObjectHealth;
	UPROPERTY(Replicated)
	float ObjectMaxHealth;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	float GetObjectHealth() const;
	UFUNCTION(BlueprintCallable)
	FDamageResponse ChangeObjectHealth(FDamageRequest DamageRequest);
	FDamageResponse SetObjectHealth(FDamageRequest DamageRequest, float newHealth);
	UFUNCTION(BlueprintPure)
	float GetObjectMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	void SetObjectMaxHealth(float newHealth);
		
};
