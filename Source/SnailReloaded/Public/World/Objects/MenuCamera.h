// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "MenuCamera.generated.h"

/**
 * 
 */

UENUM()
enum class EMenuCamType : uint8
{
	NONE = 0,
	CASE_OPENING = 1,
	SKINSELECT = 2,
	ZOOMED_OPENING = 3,
};

UCLASS()
class SNAILRELOADED_API AMenuCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMenuCamType CamType;
	
};