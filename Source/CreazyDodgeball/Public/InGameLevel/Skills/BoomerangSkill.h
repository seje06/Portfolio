// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameLevel/Skills/Skill.h"
#include "BoomerangSkill.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBoomerangState : uint8
{
	PreBoomerang,
	Boomerang,
	EndBoomerang
};

UCLASS()
class CREAZYDODGEBALL_API ABoomerangSkill : public ASkill
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

	virtual void ActiveSkill(AActor* _ball) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

private:
	EBoomerangState state= EBoomerangState::PreBoomerang;

};
