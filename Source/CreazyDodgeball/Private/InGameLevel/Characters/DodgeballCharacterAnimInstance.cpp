// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Characters/DodgeballCharacterAnimInstance.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDodgeballCharacterAnimInstance::NativeUpdateAnimation(float deltaSeconds)
{
	auto pawn = TryGetPawnOwner();
	if (!pawn) return;

	auto player = Cast<ADodgeballCharacter>(pawn);
	if (!player) return;

	LerpMoveDir(player, deltaSeconds);
	LerpMoveSpeed(player, deltaSeconds);
}

void UDodgeballCharacterAnimInstance::LerpMoveDir(ADodgeballCharacter* player, float deltaTime)
{
	float deltaLerpingSpeed = moveDirLerpingSpeed * deltaTime;

	float movementVecX = player->movementVector.Y != 0 ? (player->movementVector.X > 0 ? 0.5f : -0.5f) : player->movementVector.X;
	float movementVecY = player->movementVector.X != 0 ? (player->movementVector.Y > 0 ? 0.5f : -0.5f) : player->movementVector.Y;

	if (player->movementVector.X > 0)
	{
		lerpedMoveDir.X += deltaLerpingSpeed;
		if (lerpedMoveDir.X > movementVecX)lerpedMoveDir.X = movementVecX;
	}
	else if (player->movementVector.X == 0)
	{
		if (lerpedMoveDir.X > 0 && lerpedMoveDir.X < deltaLerpingSpeed) lerpedMoveDir.X = 0;
		else if (lerpedMoveDir.X < 0 && lerpedMoveDir.X * -1 < deltaLerpingSpeed) lerpedMoveDir.X = 0;
		else if (lerpedMoveDir.X != 0) lerpedMoveDir.X += lerpedMoveDir.X > 0 ? -1 * deltaLerpingSpeed : deltaLerpingSpeed;
	}
	else
	{
		lerpedMoveDir.X -= deltaLerpingSpeed;
		if (lerpedMoveDir.X < movementVecX)lerpedMoveDir.X = movementVecX;
	}

	if (player->movementVector.Y > 0)
	{
		lerpedMoveDir.Y += deltaLerpingSpeed;
		if (lerpedMoveDir.Y > movementVecY)lerpedMoveDir.Y = movementVecY;
	}
	else if (player->movementVector.Y == 0)
	{
		if (lerpedMoveDir.Y > 0 && lerpedMoveDir.Y < deltaLerpingSpeed) lerpedMoveDir.Y = 0;
		else if (lerpedMoveDir.Y < 0 && lerpedMoveDir.Y * -1 < deltaLerpingSpeed) lerpedMoveDir.Y = 0;
		else if (lerpedMoveDir.Y != 0) lerpedMoveDir.Y += lerpedMoveDir.Y > 0 ? -1 * deltaLerpingSpeed : deltaLerpingSpeed;

	}
	else
	{
		lerpedMoveDir.Y -= deltaLerpingSpeed;
		if (lerpedMoveDir.Y < movementVecY)lerpedMoveDir.Y = movementVecY;
	}
}

void UDodgeballCharacterAnimInstance::LerpMoveSpeed(ADodgeballCharacter* player, float deltaTime)
{
	switch (player->moveSpeedType)
	{
	case ECharacterMoveSpeedType::Walk:
		if (lerpedMaxMoveSpeed > player->GetCharacterMovement()->MaxWalkSpeed)
		{
			lerpedMaxMoveSpeed -= deltaTime * moveSpeedLerpingSpeed;
			if (lerpedMaxMoveSpeed < player->GetCharacterMovement()->MaxWalkSpeed)
			{
				lerpedMaxMoveSpeed = player->GetCharacterMovement()->MaxWalkSpeed;
			}
		}
		break;
	case ECharacterMoveSpeedType::Run:
		if (lerpedMaxMoveSpeed < player->GetCharacterMovement()->MaxWalkSpeed)
		{
			lerpedMaxMoveSpeed += deltaTime * moveSpeedLerpingSpeed;
			if (lerpedMaxMoveSpeed > player->GetCharacterMovement()->MaxWalkSpeed)
			{
				lerpedMaxMoveSpeed = player->GetCharacterMovement()->MaxWalkSpeed;
			}
		}
		break;
	}
}
