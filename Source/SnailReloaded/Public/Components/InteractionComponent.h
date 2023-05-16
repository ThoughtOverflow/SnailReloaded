// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

class UInteractionComponent;
USTRUCT(BlueprintType)
struct FInteractionData
{
	GENERATED_BODY()

public:

	FInteractionData();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInteractionComponent* LastFocusedComponent;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, APawn*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, APawn*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, APawn*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, APawn*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, APawn*, Interactor);

/**
 * 
 */
UCLASS(ClassGroup = Interacion, meta = (BlueprintSpawnableComponent))
class SNAILRELOADED_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowMultipleInteractors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInteractionEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionTime;
	

	UFUNCTION(BlueprintCallable)
	void BeginFocus(APawn* Interactor);
	UFUNCTION(BlueprintCallable)
	void EndFocus(APawn* Interactor);
	UFUNCTION(BlueprintCallable)
	void BeginInteract(APawn* Interactor);
	UFUNCTION(BlueprintCallable)
	void Interact(APawn* Interactor);
	UFUNCTION(BlueprintCallable)
	void EndInteract(APawn* Interactor);

	UPROPERTY(BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;
	UPROPERTY(BlueprintAssignable)
	FOnEndFocus OnEndFocus;
	UPROPERTY(BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;
	UPROPERTY(BlueprintAssignable)
	FOnEndInteract OnEndInteract;
	
	
	
};
