// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Components/InteractionComponent.h"

FInteractionData::FInteractionData()
{
	LastFocusedComponent = nullptr;
}

UInteractionComponent::UInteractionComponent()
{
	SetActive(true);
	SetHiddenInGame(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	InteractionDistance = 300.f;
	InteractionTime = 1.f;
	bAllowMultipleInteractors = false;
	bInteractionEnabled = true;
	
}

void UInteractionComponent::BeginFocus(APawn* Interactor)
{
	if(Interactor->IsLocallyControlled())
	{
		SetHiddenInGame(false);		
	}
	OnBeginFocus.Broadcast(Interactor);
}

void UInteractionComponent::EndFocus(APawn* Interactor)
{
	if(Interactor->IsLocallyControlled())
	{
		SetHiddenInGame(true);
	}
	OnEndFocus.Broadcast(Interactor);
}

void UInteractionComponent::BeginInteract(APawn* Interactor)
{
	OnBeginInteract.Broadcast(Interactor);
}

void UInteractionComponent::Interact(APawn* Interactor)
{
	OnInteract.Broadcast(Interactor);
}

void UInteractionComponent::EndInteract(APawn* Interactor)
{
	OnEndInteract.Broadcast(Interactor);
}
