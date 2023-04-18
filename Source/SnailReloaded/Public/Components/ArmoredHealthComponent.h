// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/HealthComponent.h"
#include "ArmoredHealthComponent.generated.h"

enum class EItemIdentifier : uint8;
USTRUCT(BlueprintType)
struct FShieldProperties
{
	GENERATED_BODY()

public:

	FShieldProperties();
	FShieldProperties(float ShieldHealth, float ShieldDamageReduction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShieldHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShieldDamageReduction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemIdentifier ShieldIdentifier;
	
};

/**
 * 
 */
UCLASS( ClassGroup=(HealthSystem), meta=(BlueprintSpawnableComponent) )
class SNAILRELOADED_API UArmoredHealthComponent : public UHealthComponent
{
	GENERATED_BODY()

public:

	UArmoredHealthComponent();

	
protected:

	virtual FDamageResponse ChangeObjectHealth(FDamageRequest DamageRequest) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Properties", ReplicatedUsing = OnRep_ShieldHealth)
	float ShieldHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Properties", Replicated)
	float ShieldDamageReduction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shield Properties", ReplicatedUsing = OnRep_ShieldType)
	EItemIdentifier ShieldIdentifier;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_ShieldHealth();
	UFUNCTION()
	void OnRep_ShieldType();
	
public:
	
	UFUNCTION(BlueprintCallable)
	void SetShieldHealth(float NewHealth);
	UFUNCTION(BlueprintPure)
	float GetShieldHealth();
	UFUNCTION(BlueprintCallable)
	void SetShieldDamageReduction(float NewReduction);
	UFUNCTION(BlueprintPure)
	float GetShieldDamageReduction();
	UFUNCTION(BlueprintCallable)
	void SetShieldIdentifier(EItemIdentifier NewIdentifier);
	UFUNCTION(BlueprintPure)
	EItemIdentifier GetShieldIdentifier();
	
};
