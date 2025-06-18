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

	//���ȷε��Լ� (BP���� ȣ�� ����)
	UFUNCTION(BlueprintCallable, Category = "Stat System")
	void LoadStatsFromTable();


protected:
	virtual void BeginPlay() override;

public:
	//���� ���� ��ġ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat System")
	float ActiveEvasionStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat System")
	float ActiveCatchStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat System")
	float ActiveAccuracyStat;
protected:
	//ĳ������ ���� ���� (BP���� ����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat System")
	EAIStatCategory aiStatCategory;

	//���� �����͸� ������ ���������̺� (BP���� ��������)
	UPROPERTY(EditAnywhere, Category = "Stat System")
	UDataTable* statDataTable;

	

	
		
};
