#pragma once

#include "CoreMinimal.h"
#include "CharacterEnums.generated.h"

UENUM(BlueprintType)
enum class ECharacterTeamType :uint8
{
	A,
	B,
	None
};

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	AI,
	Player
};

UENUM(BlueprintType)
enum class ECharacterMoveSpeedType :uint8
{
	Run,
	Walk
};

UENUM(BlueprintType)
enum class ECharacterActionType :uint8
{
	Move,
	Jump,
	Idle,
	Avoid,
	Crouching,
	Hit
};

UENUM(BlueprintType)
enum class ECharacterArmActionType :uint8
{
	ReadyCatching,
	Catching,
	PerfectCatching,
	ReadyThrowing,
	Throwing,
	None
};

UENUM(BlueprintType)
enum class ECharacterJumpingState : uint8
{
	JumpReady,
	JumpUp,
	JumpDown,
	JumpLanding,
	None
};

UENUM(BlueprintType)
enum class ECharacterFieldPositionType :uint8
{
	In,
	MiddleLine,
	LeftLine,
	RightLine,
	BackLine
};

UENUM(BlueprintType)
enum class ECharacterSkillType : uint8
{
	None,
	Boomerang,
	Giant,
	Clone,
	Fire
};