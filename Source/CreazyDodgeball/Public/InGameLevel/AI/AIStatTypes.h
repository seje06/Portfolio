#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AIStatTypes.generated.h"

UENUM(BlueprintType)
enum class EAIStatCategory : uint8
{
	None,
	Dodger,
	Catcher,
	Sniper
};

USTRUCT(BlueprintType)
struct FStatDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIStatCategory statCategory = EAIStatCategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float baseEvasionStat = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float baseCatchStat = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float baseAccuracyStat = 0.0f;
};