// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameLevel/Skills/Skill.h"
#include "GigantSkill.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGiantState : uint8
{
	PreGiant,
	Giant,
	EndGiant
};

UCLASS()
class CREAZYDODGEBALL_API AGigantSkill : public ASkill
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

	virtual void ActiveSkill(AActor* _ball) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	FVector ballGoalScale = FVector(3, 3, 3);
	FVector ballInitScale;

	EGiantState state = EGiantState::PreGiant;

	float timer = 0;
};
