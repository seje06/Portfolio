// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/Ball/BallManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "InGameLevel/Skills/Skill.h"
#include "InGameLevel/DodgeballGameState.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ADodgeballCharacter::ADodgeballCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ballManagerCom = CreateDefaultSubobject<UBallManagerComponent>(TEXT("BallManagerCom"));

	ballManagerCom->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
	ballManagerCom->SetRelativeLocation(FVector(40, 0, -6));
	ballManagerCom->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	//캡 메쉬 생성 및 트랜스폼 설정
	auto worldOffsetOfCap = GetMesh()->GetBoneTransform(TEXT("root")).TransformPosition(FVector(0, -11, 172));
	auto worldOffsetQutOfCap = GetMesh()->GetBoneTransform(TEXT("root")).TransformRotation(FRotator(0, 180, 35).Quaternion());
	auto localOffsetOfCap = GetMesh()->GetBoneTransform(TEXT("HeadSocket")).InverseTransformPosition(worldOffsetOfCap);
	auto localOffsetQutOfCap = GetMesh()->GetBoneTransform(TEXT("HeadSocket")).InverseTransformRotation(worldOffsetQutOfCap);
	
	capMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CapMeshComp"));
	capMesh->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
	capMesh->SetRelativeLocation(FVector(-5.78f,0.38f,11.33f));
	capMesh->SetRelativeRotation(FRotator(31.26f, 88.46f, -1.26f));
	capMesh->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));

	//오버랩 힛 델리게이트 바인딩
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ADodgeballCharacter::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ADodgeballCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ADodgeballCharacter::OnEndOverlap);
}

//already existent event
// Called when the game starts or when spawned
void ADodgeballCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	PrimaryActorTick.TickGroup = TG_DuringPhysics;

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
	ballManagerCom->AttachToComponent(GetMesh(), rules, TEXT("HeadSocket"));
	ballManagerCom->SetRelativeLocation(FVector(40, 0, -6));

	GetMesh()->GlobalAnimRateScale = 0.5f;

	fixHeadTrs = GetMesh()->GetBoneTransform(TEXT("HeadSocket"));
}

// Called every frame
void ADodgeballCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(type==ECharacterType::Player)
	UE_LOG(LogTemp, Warning, TEXT("floorNum%d : %f, %f, %f"),currentFloorNum, currentRotation.X, currentRotation.Y, currentRotation.Z);

	if (GetCharacterMovement()->Velocity.Z < -2 && characterActionType != ECharacterActionType::Avoid)
	{
		characterActionType = ECharacterActionType::Jump;
		jumpingState = ECharacterJumpingState::JumpDown;
	}
	else if (characterActionType == ECharacterActionType::Jump && jumpingState == ECharacterJumpingState::JumpDown)
	{
		jumpingState = ECharacterJumpingState::JumpLanding;
	}

	//점핑중 이동
	MoveOnJumping();

	//캡슐과 매쉬 위치 맞게 조정
	SetFitOfCapsuleAndMesh();

	//볼 매니저(센서) 위치 업데이트 
	UpdateBallManagerLocation();

	//볼 획득을 시도
	TryObtainBall();

	//볼 파워 업데이트
	UpdateBallThrowingPower();

	//던지기 준비중일때 하는것
	OnPreparingThrowing();

	//볼의 위치를 갱신
	ballManagerCom->UpdateSensedBallLocation();

	//발밑을 확인해서 어느 바닥 영역에 있는지 확인하고 갱신.
	CheckUnderFoot();
}

void ADodgeballCharacter::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	/*if (_otherActor && _otherActor->Tags.Num() == 1)
	{
		for (auto& tag : _otherActor->Tags)
		{
			if (tag.ToString().Contains("MiddleLine")) currentPositionType = ECharacterFieldPositionType::MiddleLine;
			else if (tag.ToString().Contains("LeftLine")) currentPositionType = ECharacterFieldPositionType::LeftLine;
			else if (tag.ToString().Contains("RightLine")) currentPositionType = ECharacterFieldPositionType::RightLine;
			else if (tag.ToString().Contains("BackLine")) currentPositionType = ECharacterFieldPositionType::BackLine;
		}
	}*/

	/*if (Cast<SphereComp>_otherComp)
	{

	}*/
}

void ADodgeballCharacter::OnBeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherActor && _otherActor->Tags.Num() == 1)
	{
		for (auto& tag : _otherActor->Tags)
		{
			if (currentPositionType != ECharacterFieldPositionType::In) return;

			if (tag.ToString().Contains("MiddleLine")) currentPositionType = ECharacterFieldPositionType::MiddleLine;
			else if (tag.ToString().Contains("LeftLine")) currentPositionType = ECharacterFieldPositionType::LeftLine;
			else if (tag.ToString().Contains("RightLine")) currentPositionType = ECharacterFieldPositionType::RightLine;
			else if (tag.ToString().Contains("BackLine")) currentPositionType = ECharacterFieldPositionType::BackLine;
		}
	}
}

void ADodgeballCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->Tags.Num() == 1)
	{
		for (auto& tag : OtherActor->Tags)
		{
			if (tag.ToString().Contains("MiddleLine") && currentPositionType == ECharacterFieldPositionType::MiddleLine) currentPositionType = ECharacterFieldPositionType::In;
			else if (tag.ToString().Contains("LeftLine") && currentPositionType == ECharacterFieldPositionType::LeftLine) currentPositionType = ECharacterFieldPositionType::In;
			else if (tag.ToString().Contains("RightLine") && currentPositionType == ECharacterFieldPositionType::RightLine) currentPositionType = ECharacterFieldPositionType::In;
			else if (tag.ToString().Contains("BackLine") && currentPositionType == ECharacterFieldPositionType::BackLine) currentPositionType = ECharacterFieldPositionType::In;
		}
	}
}

// Called to bind functionality to input
void ADodgeballCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	onCharacterDonePlayer.ExecuteIfBound(PlayerInputComponent);
	onCharacterDonePlayer.Unbind();
}


//public
void ADodgeballCharacter::Look(FVector2D moveVec)
{
	if (characterActionType == ECharacterActionType::Avoid) return;
	if (characterActionType == ECharacterActionType::Hit) return;
	if (UGameplayStatics::GetGlobalTimeDilation(GetWorld()) != 1) return;

	if (moveVec.Y > 10) moveVec.Y = 10;
	if (moveVec.Y < -10)moveVec.Y = -10;

	lookAxisVector = moveVec;
	//lookAxisX = LookAxisVector.X;
	//if(type==ECharacterType::Player)
	if (jumpingState != ECharacterJumpingState::JumpLanding && characterActionType != ECharacterActionType::Crouching) AddControllerYawInput(lookAxisVector.X);

	//idle일때 회전
	if (lookAxisVector.X != 0.f && characterActionType == ECharacterActionType::Idle)
	{
		if (GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(aM_TurnLeg) != TEXT("Left") &&
			lookAxisVector.X < 0)
		{
			GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, aM_TurnLeg);
			GetMesh()->GetAnimInstance()->Montage_Play(aM_TurnLeg, 1.0f * lookAxisVector.X, EMontagePlayReturnType::MontageLength, 0.0f, false);
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Left"), aM_TurnLeg);
		}
		else if (GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(aM_TurnLeg) != TEXT("Right") &&
			lookAxisVector.X > 0)
		{
			GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, aM_TurnLeg);
			GetMesh()->GetAnimInstance()->Montage_Play(aM_TurnLeg, 1.0f * lookAxisVector.X, EMontagePlayReturnType::MontageLength, 0.0f, false);
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Right"), aM_TurnLeg);
		}
		GetMesh()->GetAnimInstance()->Montage_SetPlayRate(aM_TurnLeg, 1.0f * lookAxisVector.X);
	}
	else
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(aM_TurnLeg))
		{
			GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, aM_TurnLeg);
		}
	}

	SetHeadAndArmRotation();
}

void ADodgeballCharacter::Move(FVector2D moveVec)
{
	if(characterActionType != ECharacterActionType::Move && characterActionType != ECharacterActionType::Idle) return;

	characterActionType = ECharacterActionType::Move;

	movementVector = moveVec;

	//NarrowGapOfHeadBetweenBody();

	if (Controller != nullptr)
	{
		// add movement 
		movementVector = movementVector.GetSafeNormal();
		AddMovementInput(GetMesh()->GetRightVector(), movementVector.X);
		AddMovementInput(GetMesh()->GetForwardVector() * -1, movementVector.Y);
	}

	if (moveVec.Y > 0) recentHorizionMoveDir = 1;
	if (moveVec.Y < 0) recentHorizionMoveDir = -1;
}

void ADodgeballCharacter::OnMoveCompleted()
{
	if (characterActionType != ECharacterActionType::Idle && characterActionType != ECharacterActionType::Move) return;

	characterActionType = ECharacterActionType::Idle;
	lookAxisVector = FVector2D::Zero();
	movementVector = FVector2D::Zero();
}

void ADodgeballCharacter::TurnMoveMode(bool canRun)
{
	if (characterActionType != ECharacterActionType::Idle && characterActionType != ECharacterActionType::Move) return;

	if (canRun && movementVector != FVector2D::ZeroVector)
	{
		moveSpeedType = ECharacterMoveSpeedType::Run;
		GetCharacterMovement()->MaxWalkSpeed = 400;
	}
	else
	{
		moveSpeedType = ECharacterMoveSpeedType::Walk;
		GetCharacterMovement()->MaxWalkSpeed = 200;
	}
}

void ADodgeballCharacter::TryJump()
{
	if (characterActionType != ECharacterActionType::Idle && characterActionType != ECharacterActionType::Move) return;
	if (GetCharacterMovement()->IsFalling()) return;

	jumpingState = ECharacterJumpingState::JumpReady;
	characterActionType = ECharacterActionType::Jump;
	lookAxisVector = FVector2D::Zero();
}

void ADodgeballCharacter::DoReadyArmAction()
{
	if (armActionType != ECharacterArmActionType::None && armActionType != ECharacterArmActionType::PerfectCatching) return;

	switch (armActionType)
	{
	case ECharacterArmActionType::None:
		armActionType = ECharacterArmActionType::ReadyCatching;
		break;
	case ECharacterArmActionType::PerfectCatching:
		armActionType = ECharacterArmActionType::ReadyThrowing;
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1, aM_Throw);
		GetMesh()->GetAnimInstance()->Montage_Play(aM_Throw);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Ready"), aM_Throw);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Throwing ready")));
		break;
	}
}

void ADodgeballCharacter::DoEndReadyArmAction()
{
	if (armActionType != ECharacterArmActionType::ReadyCatching && armActionType != ECharacterArmActionType::ReadyThrowing) return;

	switch (armActionType)
	{
	case ECharacterArmActionType::ReadyCatching:
		armActionType = ECharacterArmActionType::None;
		break;
	case ECharacterArmActionType::ReadyThrowing:
		armActionType = ECharacterArmActionType::PerfectCatching;
		GetMesh()->GetAnimInstance()->Montage_Stop(0.1, aM_Throw);
		ballManagerCom->EndReadyThrowBall();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Throwing Ready End")));
		break;
	}
}

void ADodgeballCharacter::DoArmAction()
{
	if (armActionType != ECharacterArmActionType::ReadyCatching && armActionType != ECharacterArmActionType::ReadyThrowing) return;

	switch (armActionType)
	{
	case ECharacterArmActionType::ReadyCatching:
		if (!ballManagerCom->CanObtainBall()) return;
		ballManagerCom->SetBallCatchingToEasy();
		ballManagerCom->OnBallEscapedFromManager.BindLambda([this]() 
			{
				armActionType = ECharacterArmActionType::ReadyCatching; 

			});
		armActionType = ECharacterArmActionType::Catching;
		break;
	case ECharacterArmActionType::ReadyThrowing:
		armActionType = ECharacterArmActionType::Throwing;
		Throw();

		/*GetMesh()->GetAnimInstance()->Montage_Play(aM_Throw);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Throw"), aM_Throw);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Throw")));*/
		
		break;
	}
}

void ADodgeballCharacter::InitMovement()
{
	movementVector = FVector2D::Zero();

	moveSpeedType = ECharacterMoveSpeedType::Walk;
	GetCharacterMovement()->MaxWalkSpeed = 200;
}

void ADodgeballCharacter::InitLook()
{
	lookAxisVector = FVector2D::Zero();
	Look(lookAxisVector);
}

AActor* ADodgeballCharacter::Throw()
{
	if (armActionType != ECharacterArmActionType::Throwing) return nullptr;

	GetMesh()->GetAnimInstance()->Montage_Play(aM_Throw);
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Throw"), aM_Throw);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Throw")));

	//머리 앞벡터
	FVector dir = GetMesh()->GetSocketTransform(TEXT("HeadSocket")).GetUnitAxis(EAxis::X);
	dir += FVector::UpVector / 5;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("throw dir : %f, %f, %f"), dir.X, dir.Y, dir.Z));
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("power %f"), throwingPower));

	/*auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->currentCatchingBallCharac = nullptr;*/

	AActor* ball= ballManagerCom->ThrowBall(this, throwingPower, dir.GetSafeNormal());
	if (ball)
	{
		auto gameState=Cast<ADodgeballGameState>(GetWorld()->GetGameState());
		if (gameState->currentHavingTurnTeamType == ECharacterTeamType::A)
		{
			for (auto& charac : *gameState->charactersA) if(charac) charac->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		}
		else
		{
			for (auto& charac : *gameState->charactersB) if (charac) charac->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		}
	}
	return ball;
}

void ADodgeballCharacter::Crouch()
{
	if (characterActionType != ECharacterActionType::Idle && characterActionType != ECharacterActionType::Move &&
		characterActionType != ECharacterActionType::Crouching) return;

	if (characterActionType == ECharacterActionType::Crouching)
	{
		characterActionType = ECharacterActionType::Idle;
	}
	else
	{
		characterActionType = ECharacterActionType::Crouching;
	}
}

void ADodgeballCharacter::Avoid()
{
	if (armActionType != ECharacterArmActionType::ReadyCatching && armActionType != ECharacterArmActionType::None) return;
	if (characterActionType != ECharacterActionType::Idle && characterActionType != ECharacterActionType::Move) return;
	armActionType = ECharacterArmActionType::None;
	characterActionType = ECharacterActionType::Avoid;

	GetMesh()->GetAnimInstance()->Montage_Stop(0.1);
	GetMesh()->GetAnimInstance()->Montage_Play(aM_Avoid, 2);

	FName avoidSectionName;
	if (recentHorizionMoveDir > 0) avoidSectionName = TEXT("Right");
	else avoidSectionName = TEXT("Left");

	GetMesh()->GetAnimInstance()->Montage_JumpToSection(avoidSectionName, aM_Avoid);
}

void ADodgeballCharacter::TrySkill(ESkillType _skillType)
{
	if (armActionType != ECharacterArmActionType::ReadyThrowing) return;
	armActionType = ECharacterArmActionType::Throwing;

	auto spawnedSkill = GetWorld()->SpawnActor<ASkill>(skillArray[(int)_skillType]);
	spawnedSkill->ActiveSkill(Throw());
	AddEnergy(-10);
}

void ADodgeballCharacter::OnHitFromBall(ECharacterSkillType skillType,FVector hitDir)
{
	if (characterActionType == ECharacterActionType::Hit) return;

	switch(skillType)
	{
	case ECharacterSkillType::None:
		return;
		break;
	case ECharacterSkillType::Boomerang:
		GetMesh()->GetAnimInstance()->Montage_Play(aM_Hit);
		characterActionType = ECharacterActionType::Hit;
		break;
	case ECharacterSkillType::Clone:
		GetMesh()->GetAnimInstance()->Montage_Play(aM_Hit);
		characterActionType = ECharacterActionType::Hit;
		break;
	case ECharacterSkillType::Fire:
		GetMesh()->GetAnimInstance()->Montage_Play(aM_Hit);
		characterActionType = ECharacterActionType::Hit;
		break;
	case ECharacterSkillType::Giant:
		if (hitDir.Z < 0) hitDir.Z = 0;
		LaunchCharacter(hitDir * 2500, true, true);
		if(type!=ECharacterType::Player) GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetEnableGravity(false);
		GetMesh()->GetAnimInstance()->Montage_Play(aM_Hit);
		UGameplayStatics::PlaySoundAtLocation(this, screamSound, GetActorLocation());
		characterActionType = ECharacterActionType::Hit;
		break;
	}	
	
	
}

void ADodgeballCharacter::OnGameEnded(bool isVictory)
{
	if (characterActionType == ECharacterActionType::Hit) return;

	ForceToDetachBall();

	if (isVictory)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(aM_EndGameJesture);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Victory"));
		if (type == ECharacterType::Player && victorySound) UGameplayStatics::PlaySoundAtLocation(this, victorySound, GetActorLocation());
	}
	else
	{
		GetMesh()->GetAnimInstance()->Montage_Play(aM_EndGameJesture);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("Defeat"));
		if (type == ECharacterType::Player && defeatSound) UGameplayStatics::PlaySoundAtLocation(this, defeatSound, GetActorLocation());
	}
}

void ADodgeballCharacter::WarmingUp()
{
	GetMesh()->GetAnimInstance()->Montage_Play(aM_WarmingUp);
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName(FString::FromInt(FMath::RandRange(1, 5))));
}

void ADodgeballCharacter::InitAvoidSetting()
{
	characterActionType = ECharacterActionType::Idle;
}

void ADodgeballCharacter::LaunchOnAvoid()
{
	if (characterActionType != ECharacterActionType::Avoid) return;

	FVector avoidVec = (GetActorRightVector() * recentHorizionMoveDir + FVector(0, 0, 1)).GetSafeNormal();
	float avoidPower = 600;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("launch dir : (%f, %f, %f) , power : %f"), avoidVec.X, avoidVec.Y, avoidVec.Z, avoidPower));
	LaunchCharacter(avoidVec * avoidPower, true, true);

	GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(aM_Avoid)->SetPlayRate(1);
}
//

void ADodgeballCharacter::SetHeadAndArmRotation()
{
	float preRot = currentRotation.X;
	currentRotation.X += lookAxisVector.Y;
	//if (type == ECharacterType::Player) UE_LOG(LogTemp, Warning, TEXT("head rot size : %f"), lookAxisVector.Y);

	if (currentRotation.X > 90)
	{
		currentRotation.X = 90;
		lookAxisVector.Y = 90 - preRot;
	}
	if (currentRotation.X < -90)
	{
		currentRotation.X = -90;
		lookAxisVector.Y = -90 - preRot;
	}
	//if (type == ECharacterType::Player) UE_LOG(LogTemp, Warning, TEXT("head rot size : %f"), currentRotation.X);
}

void ADodgeballCharacter::SetFitOfCapsuleAndMesh()
{
	if (characterActionType != ECharacterActionType::Jump) return;

	FVector headPos = GetMesh()->GetBoneLocation(TEXT("head"));
	FVector footRPos = GetMesh()->GetBoneLocation(TEXT("foot_r"));
	FVector footLPos = GetMesh()->GetBoneLocation(TEXT("foot_l"));

	FVector footBiggerOffset = FMath::Abs(headPos.Z - footRPos.Z) > FMath::Abs(headPos.Z - footLPos.Z) ? footRPos : footLPos;

	float preCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float capsuleCollisionHeight = FMath::Abs(headPos.Z - footBiggerOffset.Z);

	GetCapsuleComponent()->SetCapsuleHalfHeight(capsuleCollisionHeight / 2.f + 10);

	float capsuleBottomZ = GetActorLocation().Z - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float meshGoalOffsetZ = (footBiggerOffset.Z - capsuleBottomZ) * -1;

	meshGoalOffsetZ += 5;

	GetMesh()->AddWorldOffset(FVector(0, 0, meshGoalOffsetZ));

	float capsuleHalfDif = preCapsuleHalfHeight - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	if (GetCharacterMovement()->Velocity.Z > 0) AddActorWorldOffset(FVector(0, 0, capsuleHalfDif));
	else if (GetCharacterMovement()->Velocity.Z < 0) AddActorWorldOffset(FVector(0, 0, capsuleHalfDif));
	else if (!GetCharacterMovement()->IsFalling()) AddActorWorldOffset(FVector(0, 0, -1 * capsuleHalfDif));
}

void ADodgeballCharacter::MoveOnJumping()
{
	//점프중 이동
	if (characterActionType == ECharacterActionType::Jump && movementVector != FVector2D::Zero())
	{
		AddMovementInput(GetMesh()->GetRightVector(), movementVector.X);
		AddMovementInput(GetMesh()->GetForwardVector() * -1, movementVector.Y);
	}
}

void ADodgeballCharacter::UpdateBallManagerLocation()
{
	//볼매니저 위치 업데이트
	fixHeadTrs.SetLocation(GetMesh()->GetSocketLocation(TEXT("HeadSocket")));
	fixHeadTrs.SetRotation(GetMesh()->GetComponentQuat());
	if (currentRotation.X < -60)
	{
		FVector ballManagerOffsetFromWorld = fixHeadTrs.TransformPosition(FVector(0, 14.811268, -37.638097));
		FVector ballManagerOffsetFromHead = GetMesh()->GetBoneTransform(TEXT("HeadSocket")).InverseTransformPosition(ballManagerOffsetFromWorld);
		ballManagerCom->SetRelativeLocation(FVector(ballManagerOffsetFromHead.X, ballManagerCom->GetRelativeLocation().Y, ballManagerOffsetFromHead.Z));
	}
	else
	{
		ballManagerOffsetFromFixHead = fixHeadTrs.InverseTransformPosition(ballManagerCom->GetComponentLocation());
		ballManagerCom->SetRelativeLocation(FVector(45, 0, -6));
	}
}

void ADodgeballCharacter::TryObtainBall()
{
	//볼 획득 할지말지
	if (!ballManagerCom->CanObtainBall()) return;
	if (armActionType != ECharacterArmActionType::PerfectCatching && armActionType == ECharacterArmActionType::Catching)
	{
		float distR = FVector::Dist(ballManagerCom->GetSensedBallRightSideLocation(), GetMesh()->GetSocketLocation(TEXT("hand_rSocket")));
		float distL = FVector::Dist(ballManagerCom->GetSensedBallLeftSideLocation(), GetMesh()->GetSocketLocation(TEXT("hand_lSocket")));
		if (distL < 20 && distR < 20 && ballManagerCom->AttachBallToOtherActor(this, TEXT("hand_rSocket")))
		{
			auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
			*(gameState->isThrowerJudgment) = false;
			armActionType = ECharacterArmActionType::PerfectCatching;

			UGameplayStatics::PlaySoundAtLocation(this, catchingSound, ballManagerCom->GetRelativeLocation());
		}
	}
}
void ADodgeballCharacter::UpdateBallThrowingPower()
{
	if (armActionType == ECharacterArmActionType::ReadyThrowing)
	{
		throwingPower = GetMesh()->GetAnimInstance()->Montage_GetPosition(aM_Throw);
		throwingPower = aM_Throw->GetPlayLength() > 0 ? (throwingPower / aM_Throw->GetPlayLength()) * 1000*2.5f : 0;

	}
}
void ADodgeballCharacter::OnPreparingThrowing()
{
	if (armActionType != ECharacterArmActionType::ReadyThrowing) return;

	FVector dir = GetMesh()->GetSocketTransform(TEXT("HeadSocket")).GetUnitAxis(EAxis::X);
	dir += FVector::UpVector / 5;

	ballManagerCom->ReadyThrowBall(throwingPower, dir.GetSafeNormal());
}

void ADodgeballCharacter::CheckUnderFoot()
{
	FHitResult hitResult;
	FVector dir = FVector(0, 0, -1);
	FVector startPos = GetActorLocation() - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	if(GetWorld())
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, startPos, startPos + dir * 300, ECollisionChannel::ECC_WorldStatic))
	{
		if(hitResult.GetActor())
		for (auto& tag : hitResult.GetActor()->Tags)
		{
			if (tag.ToString().Contains("Plane"))
			{
				FString tagStr = tag.ToString();
				FString numberPart = tagStr.RightChop(5); // "Plane"은 5글자
				int floorNum = FCString::Atoi(*numberPart);

				switch (teamType)
				{
				case ECharacterTeamType::A:
					if (floorNum < 7) currentFloorNum = floorNum;
					break;
				case ECharacterTeamType::B:
					if (floorNum > 6) currentFloorNum = floorNum-6;
					break;
				case ECharacterTeamType::None:
					break;
				}

				return;
			}
		}
	}
}

void ADodgeballCharacter::AddEnergy(float energyToAdd)
{
	energy += energyToAdd;
	if (energy >= maxEnergy)
	{
		onEnergyChanged.ExecuteIfBound(true);
		energy = maxEnergy;
	}
	else if (energy < 0)
	{
		energy = 0;
		onEnergyChanged.ExecuteIfBound(false);
	}
	else
	{
		onEnergyChanged.ExecuteIfBound(false);
	}
}

void ADodgeballCharacter::FixBallManagerPosition()
{
	auto rotation = currentRotation;

	Look(FVector2D(0, 180));
	UpdateBallManagerLocation();
	Look(FVector2D(0, -151));
	UpdateBallManagerLocation();
	Look(FVector2D(0, rotation.Y + 61));
	/*auto rotation = currentRotation;
	currentRotation =FVector::Zero();
	UpdateBallManagerLocation();
	currentRotation = FVector(-61, 0, 0);
	UpdateBallManagerLocation();
	currentRotation = rotation;
	UpdateBallManagerLocation();*/
}

void ADodgeballCharacter::OnDisappeared()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	if(characterActionType==ECharacterActionType::Move|| characterActionType == ECharacterActionType::Idle)
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);
		GetMesh()->Stop();
		GetMesh()->GetAnimInstance()->Montage_Play(aM_Shrugging);
	}

	UGameplayStatics::PlaySoundAtLocation(this, dieSound, GetActorLocation());

	// 머티리얼 디졸브 시작
	disSolveValue = 1.5f;
	Mat = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	Mat2 = GetMesh()->CreateAndSetMaterialInstanceDynamic(1);
	CapMat = capMesh->CreateAndSetMaterialInstanceDynamic(0);
	TWeakObjectPtr<ADodgeballCharacter> weakThis(this);
	GetWorld()->GetTimerManager().SetTimer(dissolveTick, [weakThis]() mutable {
		if (!weakThis.IsValid()) return;

		weakThis->disSolveValue -= 0.005f;
		if (weakThis->Mat) weakThis->Mat->SetScalarParameterValue(TEXT("Dissolve Amount"), weakThis->disSolveValue);
		if (weakThis->Mat2) weakThis->Mat2->SetScalarParameterValue(TEXT("Dissolve Amount"), weakThis->disSolveValue);
		if(weakThis->CapMat) weakThis->CapMat->SetScalarParameterValue(TEXT("Dissolve Amount"), weakThis->disSolveValue);
		else UE_LOG(LogTemp, Warning, TEXT("mat is null"));
		if (weakThis->disSolveValue <= 1.1f)
		{
			weakThis->GetWorld()->GetTimerManager().ClearTimer(weakThis->dissolveTick);
			weakThis->SetLifeSpan(0.1f); // 안전하게 삭제
		}
		}, 0.05f, true);
}

void ADodgeballCharacter::OnAppeared()
{
	UGameplayStatics::PlaySoundAtLocation(this, inverseDieSound, GetActorLocation());

	Mat = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	Mat2 = GetMesh()->CreateAndSetMaterialInstanceDynamic(1);
	CapMat = capMesh->CreateAndSetMaterialInstanceDynamic(0);
	disSolveValue = 1.1f;
	Mat->SetScalarParameterValue(TEXT("Dissolve Amount"), disSolveValue);
	Mat2->SetScalarParameterValue(TEXT("Dissolve Amount"), disSolveValue);
	CapMat->SetScalarParameterValue(TEXT("Dissolve Amount"), disSolveValue);

	TWeakObjectPtr<ADodgeballCharacter> weakThis(this);
	GetWorld()->GetTimerManager().SetTimer(dissolveTick, [weakThis]() mutable {

		if (!weakThis.IsValid()) return;

		weakThis->disSolveValue += 0.01f;
		if (weakThis->Mat)weakThis->Mat->SetScalarParameterValue(TEXT("Dissolve Amount"), weakThis->disSolveValue);
		if (weakThis->Mat2) weakThis->Mat2->SetScalarParameterValue(TEXT("Dissolve Amount"), weakThis->disSolveValue);
		if (weakThis->CapMat) weakThis->CapMat->SetScalarParameterValue(TEXT("Dissolve Amount"), weakThis->disSolveValue);
		else UE_LOG(LogTemp, Warning, TEXT("mat is null"));
		if (weakThis->disSolveValue >=1.5f)
		{
			weakThis->GetWorld()->GetTimerManager().ClearTimer(weakThis->dissolveTick);
		}
		}, 0.05f, true);
}

void ADodgeballCharacter::ForceToDetachBall()
{
	if (!ballManagerCom->GetBallActor()) return;
	switch (armActionType)
	{
	case ECharacterArmActionType::None:
		break;
	case ECharacterArmActionType::Catching:
		break;
	case ECharacterArmActionType::PerfectCatching:
		DoReadyArmAction();
		DoArmAction();
		break;
	case ECharacterArmActionType::ReadyThrowing:
		DoArmAction();
		break;
	}

	ballManagerCom->OnBallForceDetached();
}

void ADodgeballCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(dissolveTick);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}
//