// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Framework/DefaultGameMode.h"

FOutfitData::FOutfitData(): OutfitRarity(EOutfitRarity::Common), ThumbnailURL(nullptr)
{
}

FOutfitData::FOutfitData(FString Name, EOutfitRarity Rarity, UTexture2D* URL)
{
	OutfitName = Name;
	OutfitRarity = Rarity;
	ThumbnailURL = URL;
}

ADefaultGameMode::ADefaultGameMode()
{
	bUseSeamlessTravel = true;
}
