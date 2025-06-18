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
	//��A ��Ŭ��
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* teamAShotClockText = nullptr;

	//��B ��Ŭ��
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* teamBShotClockText = nullptr;
};
