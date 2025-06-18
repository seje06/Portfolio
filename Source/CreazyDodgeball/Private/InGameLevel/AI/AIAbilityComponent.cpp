// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/AI/AIAbilityComponent.h"
#include "InGameLevel/AI/AIStatTypes.h"
#include "Engine/DataTable.h"


UAIAbilityComponent::UAIAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	aiStatCategory = (EAIStatCategory)FMath::RandRange(1, 2);

	switch (aiStatCategory)
	{
	case EAIStatCategory::Catcher:
		ActiveEvasionStat = 0.3f;
		ActiveCatchStat = 0.31f;
		break;
	case EAIStatCategory::Dodger:
		ActiveEvasionStat = 0.31f;
		ActiveCatchStat = 0.3f;
		break;
	}
}

void UAIAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadStatsFromTable();
}

void UAIAbilityComponent::LoadStatsFromTable()
{
	if (!statDataTable) return;

	//context는 디버그용 텍스트
	const FString contextString(TEXT("Stat Load"));

	//데이터테이블 전체 row 순회
	TArray<FName> rowNames = statDataTable->GetRowNames();

	for (const FName& rowName : rowNames)
	{
		const FStatDataRow* row = statDataTable->FindRow<FStatDataRow>(rowName, contextString);
		if (row && row->statCategory == aiStatCategory)
		{
			ActiveEvasionStat = row->baseEvasionStat;
			ActiveCatchStat = row->baseCatchStat;
			ActiveAccuracyStat = row->baseAccuracyStat;
			break;
		}
	}
}


