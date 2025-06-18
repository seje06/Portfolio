// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterEnums.h"
#include "JudgmentCharacter.generated.h"

UCLASS()
class CREAZYDODGEBALL_API AJudgmentCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJudgmentCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InitBallPositionOnTurnEnded();
	void InitOnTurnEnded();

	UFUNCTION(BlueprintCallable)
	void MoveForThrow();
	UFUNCTION(BlueprintCallable)
	void MoveStop();
	UFUNCTION(BlueprintCallable)
	void ThrowUp();

	float TurnCharac(float DeltaTime, class ADodgeballCharacter* aICharacter, FVector goalDir, float turnSpeed, bool bHeadRotation);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void OnBallActionEnded();
	void Init();
	void InitBall();

	void StartThrow();

	void PlayBackgroundSound();
public:
	UPROPERTY(BlueprintReadOnly)
	bool isThrowingUP = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* capMesh = nullptr;
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADodgeBall> ballClass;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_ThrowingUp;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_WinJesture;

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* whistleSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* backgroundSound;

private:
	class ADodgeBall* ball;

	FTimerHandle moveTickTimerHandle;
	FTimerHandle timerHandleInitBall;
	FTimerHandle timerHandleStartThrow;
	FTimerHandle timerHandleEndGame;

	bool isThrowerJudgment = false;

	FVector initLocation;

	float turnProgressingTime = 0;
	ECharacterTeamType preHavingTurnTeam = ECharacterTeamType::None;

	class UAudioComponent* audioComp = nullptr;
};
