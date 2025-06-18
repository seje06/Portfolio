// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DodgeballCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CREAZYDODGEBALL_API UDodgeballCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float deltaSeconds) override;

private:
	void LerpMoveDir(class ADodgeballCharacter* player, float deltaTime);
	void LerpMoveSpeed(class ADodgeballCharacter* player, float deltaTime);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D lerpedMoveDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveDirLerpingSpeed = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float lerpedMaxMoveSpeed = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveSpeedLerpingSpeed = 200;
};
