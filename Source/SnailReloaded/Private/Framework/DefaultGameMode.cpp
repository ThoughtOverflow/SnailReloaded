// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultGameMode.h"

FOutfitData::FOutfitData(): OutfitRarity(EOutfitRarity::Common), Thumbnail(nullptr)
{
}

FOutfitData::FOutfitData(FString Name, EOutfitRarity Rarity, UTexture2D* Thumb)
{
	OutfitName = Name;
	OutfitRarity = Rarity;
	Thumbnail = Thumb;
}

ADefaultGameMode::ADefaultGameMode()
{
	bUseSeamlessTravel = true;
}
