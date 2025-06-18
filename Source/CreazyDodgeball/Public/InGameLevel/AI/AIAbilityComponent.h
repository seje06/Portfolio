// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InGameLevel/AI/AIStatTypes.h"
#include "AIAbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CREAZYDODGEBALL_API UAIAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAIAbilityComponent();

	//스탯로딩함수 (BP에서 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Stat System")
	void LoadStatsFromTable();


protected:
	virtual void BeginPlay() override;

public:
	//실제 스탯 수치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat System")
	float ActiveEvasionStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat System")
	float ActiveCatchStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat System")
	float ActiveAccuracyStat;
protected:
	//캐릭터의 유형 지정 (BP에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat System")
	EAIStatCategory aiStatCategory;

	//스탯 데이터를 가져올 데이터테이블 (BP에서 지정가능)
	UPROPERTY(EditAnywhere, Category = "Stat System")
	UDataTable* statDataTable;

	

	
		
};
