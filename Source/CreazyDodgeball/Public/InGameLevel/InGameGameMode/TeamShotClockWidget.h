#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameLevel/Characters/CharacterEnums.h"
#include "TeamShotClockWidget.generated.h"

class UTextBlock;

UCLASS()
class CREAZYDODGEBALL_API UTeamShotClockWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//ÆÀA ¼¦Å¬¶ô
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* teamAShotClockText = nullptr;

	//ÆÀB ¼¦Å¬¶ô
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* teamBShotClockText = nullptr;
};
