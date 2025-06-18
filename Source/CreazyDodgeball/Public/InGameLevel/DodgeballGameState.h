// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Characters/CharacterEnums.h"
#include "DodgeballGameState.generated.h"

UENUM(BlueprintType)
enum class EBallPositionType : uint8
{
	In,
	Out,
	Notting
};

/**
 * 
 */
UCLASS()
class CREAZYDODGEBALL_API ADodgeballGameState : public AGameState
{
	GENERATED_BODY()
public:
	void UpdateProximateCharacterToBall(ECharacterTeamType includedTeam, TArray<class ADodgeballCharacter*>& characters);
	class ACharacter* GetProximateEnemyToLocation(class ACharacter* charac,FVector standardLocation);
public:
	EBallPositionType currentBallPositionType = EBallPositionType::Notting;
	//ECharacterTeamType currentHavingBallTeamType = ECharacterTeamType::None;
	ECharacterTeamType currentHavingTurnTeamType = ECharacterTeamType::None;
	class ACharacter* currentCatchingBallCharac=nullptr;
	class ACharacter* proximateCharacterToBall = nullptr;
	class ACharacter* recentAimedCharac = nullptr;
	UPROPERTY(BlueprintReadOnly)
	AActor* ball=nullptr;

	TArray<class ADodgeballCharacter*>* charactersA;
	TArray<class ADodgeballCharacter*>* charactersB;

	float floorCountOfOneTeam = 6;

	float currentAliveTeamACharacterCount = 6;
	float currentAliveTeamBCharacterCount = 6;
	
	float catchingTryingProgressingTime = 0.5f;

	int32* currentSet = nullptr;

	bool* isThrowerJudgment = nullptr;
	bool isSetStarting = false;
	bool isSetEnded = false;
	bool isGameEnded = false;
	bool isProgressingFirstSetIntro = true;

	ECharacterTeamType recentWinnerTeam = ECharacterTeamType::None;

	FVector judgementInitLocation;
};
