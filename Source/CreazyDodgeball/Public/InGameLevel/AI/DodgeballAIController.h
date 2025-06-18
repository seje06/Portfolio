// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Characters/CharacterEnums.h"
#include "DodgeballAIController.generated.h"


UENUM(BlueprintType)
enum class EAIState : uint8
{
	CatchingOnSafeState,
	Attacking,
	Aimed,
	Notting,
	FloorMoving,
	Guarding
};
UENUM(BlueprintType)
enum class EAIStateOnFloorMoving : uint8
{
	ToMyFloor,
	ToFree
};
UENUM(BlueprintType)
enum class EAIStateOnFreeMoving : uint8
{
	Idle,
	Left,
	Right,
	Forward,
	Back
};

UENUM(BlueprintType)
enum class EAimedAIState : uint8
{
	Notting,
	Avoiding,
	Catching
};
/**
 * 
 */
UCLASS()
class CREAZYDODGEBALL_API ADodgeballAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ADodgeballAIController();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void UpdateAIMovingStateOnFloor();
	void OnFloorMoving(float DeltaTime,class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);
	void OnCatchingBallOnSafeState(float DeltaTime, class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);
	void OnAttacking(float DeltaTime, class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);
	void OnAIAimed(float DeltaTime, class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);
	void OnGuarding(float DeltaTime, class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);

	void OnRecentThrowerIsJudgment(float DeltaTime, class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);

	void DoAIAttack(class ADodgeballCharacter* aICharacter, class ADodgeballGameState* gameState);

	//AI를 회전시키고, 목표 방향까지 남은 몸통 각도를 반환, 남은 머리 각도는 반환하지 않는다.
	float TurnAI(float DeltaTime, class ADodgeballCharacter* aICharacter, FVector goalDir, float turnSpeed ,bool bHeadRotation);

	void InitState();
protected:
	virtual void BeginPlay() override;
public:
	TArray<int> aIFloorNums;
	TArray<AActor*> floors;

	EAIState aIState = EAIState::Notting;
	EAIStateOnFloorMoving stateOnFloorMoving = EAIStateOnFloorMoving::ToFree;
	EAIStateOnFreeMoving stateOnFreeMoving = EAIStateOnFreeMoving::Idle;
	EAimedAIState targetedAIState = EAimedAIState::Notting;

	int selectedFloorIndex=0;
private:
	float aIFreeMovingTimer = 0;
	float aIFreeMovingMaxTime = 0.5f;

	float catchingTryingTimer = 0;
	float throwingTryingTimer = 0;

	int preFloorCount = 0;

	ECharacterTeamType preTeamTurnType = ECharacterTeamType::None;

	class UAIAbilityComponent* aIAbilityComp = nullptr;
};
