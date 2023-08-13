

#include "Gameplay/Gadgets/ScanMine.h"

#include <SPIRV-Reflect/SPIRV-Reflect/include/spirv/unified1/spirv.h>

#include "Components/SphereComponent.h"
#include "Framework/Combat/CombatPlayerController.h"
#include "Framework/Combat/Standard/StandardCombatGameState.h"
#include "Gameplay/Player/DefaultPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Serialization/PropertyLocalizationDataGathering.h"

AScanMine::AScanMine()
{
	GadgetHealthComponent->GameObjectType.ObjectType = EGameObjectTypes::ScanMine;
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	RootBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RootBox"));
	SetRootComponent(RootBox);
	GadgetMesh->SetupAttachment(GetRootComponent());
	DetectionSphere->SetSphereRadius(200.f);
	DetectionSphere->SetupAttachment(GetRootComponent());
	GadgetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootBox->SetGenerateOverlapEvents(true);
	RootBox->SetCollisionResponseToAllChannels(ECR_Block);
	RootBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AScanMine::PlayerEntered);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AScanMine::PlayerExit);
	RootBox->OnComponentHit.AddDynamic(this, &AScanMine::OnBoxCollide);
	bAllowScanning = false;
	RootBox->SetNotifyRigidBodyCollision(true);
	GadgetHealthComponent->DefaultObjectHealth = 20.f;
	GadgetHealthComponent->SetObjectMaxHealth(20.f);
}

void AScanMine::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(HasAuthority() && bAllowScanning)
	{
		TArray<AActor*> Actors;
		DetectionSphere->GetOverlappingActors(Actors, ADefaultPlayerCharacter::StaticClass());
		for(auto& Actor : Actors)
		{
			//check for line of sight:
			ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(Actor);
			FHitResult HitResult;
			FVector TraceStartLoc = GetActorLocation();
			FVector TraceEndLoc = PlayerCharacter->GetActorLocation();
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(PlayerCharacter);
			// if(!OwningPlayerState->GetOwningController()) return;
			if(GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceEndLoc, TraceStartLoc, ECC_Visibility, Params))
			{
				if(HitResult.GetActor() == this && Cast<ACombatPlayerState>(PlayerCharacter->GetPlayerState())->GetTeam() != GetOwningTeam())
				{
					if(!DetectedPlayers.Contains(Actor))
					{
						//We hit the player - it is in line of sight
						UE_LOG(LogTemp, Warning, TEXT("Revealed: %s"), *PlayerCharacter->GetName());
						// PlayerCharacter->SetRevealedByMine(true);
						//Player Notification:
						Cast<ACombatPlayerController>(PlayerCharacter->GetController())->TriggerPlayerNotification( FText::FromString(TEXT("detected")), FLinearColor(0.7f, 0.5f, 0.f, 0.75f), 3.f);
						Cast<ACombatPlayerController>(OwningPlayerState->GetOwningController())->TriggerPlayerNotification( FText::FromString(TEXT("mine triggered")), FLinearColor(0.16f, 0.64f, 0.7f, 0.75f), 3.f);
						//---
						if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
						{
							for(auto& State : StandardCombatGameState->GetAllPlayersOfTeam(GetOwningTeam()))
							{
								if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(State->GetPawn()))
								{
									DefaultPlayerCharacter->Client_SetRevealPlayer(PlayerCharacter, true);
								}
							}
						}
						DetectedPlayers.Add(PlayerCharacter);
					}
				}else
				{
					if(DetectedPlayers.Contains(Actor))
					{
						//Set reveal to false;
						// PlayerCharacter->SetRevealedByMine(false);
						if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
						{
							for(auto& State : StandardCombatGameState->GetAllPlayersOfTeam(GetOwningTeam()))
							{
								if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(State->GetPawn()))
								{
									DefaultPlayerCharacter->Client_SetRevealPlayer(PlayerCharacter, false);
								}
							}
						}
						DetectedPlayers.Remove(PlayerCharacter);
					}
				}
			}
		}
	}

	
}

void AScanMine::OnInitialized()
{
	Super::OnInitialized();

	RootBox->SetSimulatePhysics(true);
	RootBox->AddImpulse(OwningPlayerState->GetPawn()->GetControlRotation().Vector() * 1200.f, NAME_None, true);
	if(HasAuthority())
	{
		if(FMath::IsNearlyZero(BootupTime))
		{
			bAllowScanning = true;
		}else
		{
			GetWorldTimerManager().SetTimer(BootupTimer, this, &AScanMine::ScanInitialize_Callback, BootupTime);
		}
	}
}

void AScanMine::PlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex)
{
	if(HasAuthority() && bAllowScanning)
	{
		if(ADefaultPlayerCharacter* PlayerCharacter = Cast<ADefaultPlayerCharacter>(OtherActor))
		{
			if(DetectedPlayers.Contains(PlayerCharacter))
			{
				//Set reveal to false;
				// PlayerCharacter->SetRevealedByMine(false);
				if(AStandardCombatGameState* StandardCombatGameState = Cast<AStandardCombatGameState>(GetWorld()->GetGameState()))
				{
					for(auto& State : StandardCombatGameState->GetAllPlayersOfTeam(GetOwningTeam()))
					{
						if(ADefaultPlayerCharacter* DefaultPlayerCharacter = Cast<ADefaultPlayerCharacter>(State->GetPawn()))
						{
							DefaultPlayerCharacter->Client_SetRevealPlayer(PlayerCharacter, false);
						}
					}
				}
				DetectedPlayers.Remove(PlayerCharacter);
			}
		}
	}
}

void AScanMine::OnBoxCollide(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//Check for the barrier - ignore:
	if(OtherActor->GetClass()->IsChildOf(ABarrier::StaticClass()))
	{
		return;
	}
	RootBox->SetNotifyRigidBodyCollision(false);
	RootBox->SetSimulatePhysics(false);
	FVector StartV = GetActorUpVector();
	FVector EndV = Hit.ImpactNormal;
	FQuat RotationQ = FQuat::FindBetweenNormals(StartV, EndV);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *RotationQ.Rotator().ToString());
	SetActorLocation(Hit.ImpactPoint + Hit.ImpactNormal * RootBox->GetScaledBoxExtent().Z);
	SetActorRotation(RotationQ.Rotator());
}

void AScanMine::ScanInitialize_Callback()
{
	if(HasAuthority())
	{
		bAllowScanning = true;
	}
}

