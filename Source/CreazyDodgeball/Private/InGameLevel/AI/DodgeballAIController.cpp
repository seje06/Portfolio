// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/AI/DodgeballAIController.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InGameLevel/DodgeballGameState.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "InGameLevel/Ball/BallManagerComponent.h"
#include "InGameLevel/AI/AIAbilityComponent.h"
//#include "Components/CapsuleComponent.h"

ADodgeballAIController::ADodgeballAIController()
{
}

void ADodgeballAIController::Tick(float DeltaTime)
{
	auto aICharacter = Cast<ADodgeballCharacter>(GetPawn());
	if (!aICharacter) return;
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;

	if (gameState->isGameEnded)
	{
		InitState();
		return;
	}

	if (!gameState->ball) return;

	if (aICharacter->characterActionType == ECharacterActionType::Hit) return;

	if (aIFloorNums.Num() == 0) return;

	//B팀이고, 플레이어와 마주보는 자리인 3번자리인 캐릭터만 볼을 잡으려함.
	if (!gameState->isThrowerJudgment) return;
	if (*(gameState->isThrowerJudgment) && aIFloorNums[0] == 3)
	{
		OnRecentThrowerIsJudgment(DeltaTime, aICharacter, gameState);
		return;
	}
	else if (*(gameState->isThrowerJudgment))
	{
		InitState();
		FVector goalDir = gameState->judgementInitLocation - aICharacter->GetActorLocation();
		TurnAI(DeltaTime, aICharacter, goalDir, 5, true);
	}

	//세트가 초기화 중일 때 모두 정지
	//if (!gameState->isSetStarting) return;
	if ((gameState->isSetStarting))
	{
		InitState();
		return;
	}

	if (gameState->currentHavingTurnTeamType == ECharacterTeamType::None) return;

	aICharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	
	if (preTeamTurnType != gameState->currentHavingTurnTeamType)
	{
		InitState();
	}
	preTeamTurnType = gameState->currentHavingTurnTeamType;

	//aI가 바닥 위에서 이동해야할 방법을 갱신한다.
	UpdateAIMovingStateOnFloor();

	//내팀 턴일 때
	if (aICharacter->teamType == gameState->currentHavingTurnTeamType)
	{
		auto ball = Cast<ADodgeBall>(gameState->ball);
		//내야에 있을때
		switch (gameState->currentBallPositionType)
		{
		case EBallPositionType::In:
			if (!gameState->currentCatchingBallCharac && gameState->proximateCharacterToBall == aICharacter && aIState != EAIState::CatchingOnSafeState &&
				ball->bIsGrounded)
			{
				//InitState();
				aIState = EAIState::CatchingOnSafeState;
			}
			else if (gameState->currentCatchingBallCharac == aICharacter && aIState != EAIState::Attacking)
			{
				//InitState();
				aIState = EAIState::Attacking;
			}
			else
			{
				
				//aIState = EAIState::FloorMoving;
			}
			break;
		case EBallPositionType::Out:
			break;
		case EBallPositionType::Notting:
			break;
		}
	}
	//적팀 턴일 때
	else if(gameState->currentHavingTurnTeamType!=ECharacterTeamType::None)
	{
		//공이 내야에 있을 때
		if (gameState->currentBallPositionType == EBallPositionType::In)
		{
			//조종되는 aI가 타겟이 되었을 때
			if (!gameState->currentCatchingBallCharac && gameState->recentAimedCharac == aICharacter && aIState != EAIState::Aimed)
			{
				//InitState();
				aIState = EAIState::Aimed;
			}
			else if ((gameState->currentCatchingBallCharac || Cast<ADodgeBall>(gameState->ball)->recentThrownCharacter) && aIState != EAIState::Guarding)
			{
				//InitState();
				aIState = EAIState::Guarding;
			}
		}
	}

	//무슨 이윤진 모르겠지만 볼을 잡았는데도 턴을 가진 팀이 본 ai의 팀이 아닐경우 갱신, aI스테이트도 공격으로 갱신
	if (gameState->currentCatchingBallCharac == aICharacter&& aICharacter->teamType != gameState->currentHavingTurnTeamType&& aIState != EAIState::Attacking)
	{
		gameState->currentHavingTurnTeamType = aICharacter->teamType;
		aIState = EAIState::Attacking;
	}

	//UE_LOG(LogTemp, Warning, TEXT("floorNum%d state : %d"), aIFloorNums[selectedFloorIndex], (int)aIState);

	switch (aIState)
	{
	case EAIState::Notting:
		break;
	case EAIState::FloorMoving:
		OnFloorMoving(DeltaTime, aICharacter, gameState);
		break;
	case EAIState::CatchingOnSafeState:
		OnCatchingBallOnSafeState(DeltaTime, aICharacter, gameState);
		break;
	case EAIState::Attacking:
		OnAttacking(DeltaTime, aICharacter, gameState);
		break;
	case EAIState::Aimed:
		OnAIAimed(DeltaTime, aICharacter, gameState);
		break;
	case EAIState::Guarding:
		OnGuarding(DeltaTime, aICharacter, gameState);
	}
}

void ADodgeballAIController::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GetWorld()) return;
	if (!GetWorld()->GetGameState()) return;

	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;

	auto ball = Cast<ADodgeBall>(gameState->ball);
	if (!ball) return;

	ball->OnCatched.RemoveAll(this);
	ball->OnGrounded.RemoveAll(this);

	//UE_LOG(LogTemp, Warning, TEXT("beginDestroy!!!!!!!!!!!!!!!!"));

	if (aIAbilityComp)
	{
		aIAbilityComp->DestroyComponent();
		aIAbilityComp = nullptr;
	}
}

void ADodgeballAIController::UpdateAIMovingStateOnFloor()
{
	if (aIFloorNums.Num() <= 0) return;
	if (floors.Num() <= 0) return;

	if (preFloorCount != aIFloorNums.Num())
	{
		selectedFloorIndex = FMath::RandRange(0, aIFloorNums.Num() - 1);
		preFloorCount = aIFloorNums.Num();
	}
	auto aICharacter = Cast<ADodgeballCharacter>(GetPawn());
	if (!aICharacter) return;

	//ai가 밖으로 안나갔을때만 리턴을 할수 있음
	if (aICharacter->currentPositionType == ECharacterFieldPositionType::In)
	{
		if (aIState != EAIState::Notting && aIState != EAIState::FloorMoving) return;
		//if (aIState == EAIState::FloorMoving) return;
		if (aIState == EAIState::FloorMoving && stateOnFloorMoving == EAIStateOnFloorMoving::ToMyFloor) return;
	}

	aIState = EAIState::FloorMoving;
	aICharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	//ai게 지정된 구역을 벗어나지 않았을 경우 리턴
	if(aICharacter->currentPositionType==ECharacterFieldPositionType::In)
	for (auto& floorNum : aIFloorNums)
	{
		if (aICharacter->currentFloorNum == floorNum) return;
	}

	//aICharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	stateOnFloorMoving = EAIStateOnFloorMoving::ToMyFloor;
	if(stateOnFloorMoving != EAIStateOnFloorMoving::ToMyFloor) selectedFloorIndex = FMath::RandRange(0, aIFloorNums.Num() - 1);
	//aIState = EAIState::FloorMoving;
}

void ADodgeballAIController::OnFloorMoving(float DeltaTime, class ADodgeballCharacter* aICharacter, ADodgeballGameState* gameState)
{
	FVector targetPos = floors[aIFloorNums[selectedFloorIndex] - 1]->GetActorLocation();
	FVector goalDir = (targetPos - aICharacter->GetActorLocation());

	if (aICharacter->characterActionType == ECharacterActionType::Crouching)aICharacter->Crouch();

	if (aICharacter->currentFloorNum == aIFloorNums[selectedFloorIndex] && FVector::Dist2D(targetPos, aICharacter->GetActorLocation()) < 50&& stateOnFloorMoving != EAIStateOnFloorMoving::ToFree)
	{
		aICharacter->OnMoveCompleted();
		aICharacter->InitMovement();
		aICharacter->InitLook();

		aICharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		aIState = EAIState::Notting;
		stateOnFloorMoving = EAIStateOnFloorMoving::ToFree;
	}
	else if (stateOnFloorMoving == EAIStateOnFloorMoving::ToFree)
	{
		aIFreeMovingTimer += DeltaTime;

		targetPos = gameState->ball->GetActorLocation();
		goalDir = (targetPos - aICharacter->GetMesh()->GetSocketLocation("HeadSocket"));

		TurnAI(DeltaTime, aICharacter, goalDir, 10, true);

		if (aIFreeMovingTimer > aIFreeMovingMaxTime)
		{
			aIFreeMovingTimer = 0;

			stateOnFreeMoving = (EAIStateOnFreeMoving)FMath::RandRange(0, 4);
		}
		aICharacter->TurnMoveMode(false);
		switch (stateOnFreeMoving)
		{
		case EAIStateOnFreeMoving::Idle:
			aICharacter->InitMovement();
			aICharacter->OnMoveCompleted();
			break;
		case EAIStateOnFreeMoving::Left:
			aICharacter->Move(FVector2D(0, -1));
			break;
		case EAIStateOnFreeMoving::Right:
			aICharacter->Move(FVector2D(0, 1));
			break;
		case EAIStateOnFreeMoving::Forward:
			aICharacter->Move(FVector2D(1, 0));
			break;
		case EAIStateOnFreeMoving::Back:
			aICharacter->Move(FVector2D(-1, 0));
			break;
		default:
			break;
		}
	}
	else if(stateOnFloorMoving == EAIStateOnFloorMoving::ToMyFloor)
	{
		//벗어났음을 확인했고, 해당 바닥 으로 이동하게 함.
		TurnAI(DeltaTime, aICharacter, goalDir, 20, false);
		//바라보는 곳을 기준으로 이동
		aICharacter->Move(FVector2D(1, 0));
		aICharacter->TurnMoveMode(true);
		aIFreeMovingTimer = 0;
	}

}

void ADodgeballAIController::OnCatchingBallOnSafeState(float DeltaTime, ADodgeballCharacter* aICharacter,ADodgeballGameState* gameState)
{
	if (gameState->proximateCharacterToBall != aICharacter || gameState->currentCatchingBallCharac)
	{
		InitState();
		return;
	}

	FVector targetPos = gameState->ball->GetActorLocation();
	FVector goalDir = (targetPos - aICharacter->GetMesh()->GetSocketLocation("HeadSocket"));

	FVector targetOffsetFromSelf = aICharacter->GetTransform().InverseTransformPosition(targetPos);

	auto remainRotation = TurnAI(DeltaTime, aICharacter, goalDir, 20, true);

	if (FVector::Dist2D(targetPos, aICharacter->GetActorLocation()) > 60 && aICharacter->armActionType != ECharacterArmActionType::Catching&& aICharacter->characterActionType != ECharacterActionType::Crouching)
	{
		if (aICharacter->characterActionType == ECharacterActionType::Crouching)aICharacter->Crouch();

		//TurnAI(DeltaTime, aICharacter, goalDir, 20, true);
		aICharacter->TurnMoveMode(true);
		aICharacter->Move(FVector2D(1, 0));
		aICharacter->TurnMoveMode(true);
	}
	else if (FVector::Dist2D(targetPos, aICharacter->GetActorLocation()) < 30 && aICharacter->armActionType != ECharacterArmActionType::Catching&& aICharacter->characterActionType != ECharacterActionType::Crouching)
	{
		if (aICharacter->characterActionType == ECharacterActionType::Crouching)aICharacter->Crouch();
		//TurnAI(DeltaTime, aICharacter, goalDir, 20, true);
		aICharacter->TurnMoveMode(true);
		aICharacter->Move(FVector2D(-1, 0));
		aICharacter->TurnMoveMode(true);
	}
	else if (remainRotation <= 5 && aICharacter->characterActionType != ECharacterActionType::Crouching)
	{
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetPhysicsLinearVelocity(FVector::Zero());
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetSimulatePhysics(false);
		FVector ballTargetWorldOffset=aICharacter->GetTransform().TransformPosition(FVector(50, 0, 0));

		catchingTryingTimer += DeltaTime;
		if (catchingTryingTimer > 2)
		{
			aICharacter->AddActorLocalOffset(FVector(-100, 0, 0));
			//Cast<ADodgeBall>(gameState->ball)->AddActorWorldOffset(FVector())
			catchingTryingTimer = 0;
			return;
		}

		//TurnAI(DeltaTime, aICharacter, goalDir, 200, true);
		gameState->ball->SetActorLocation(FVector(ballTargetWorldOffset.X, ballTargetWorldOffset.Y, gameState->ball->GetActorLocation().Z));
		aICharacter->OnMoveCompleted();
		aICharacter->InitMovement();
		aICharacter->Crouch();
		aICharacter->DoReadyArmAction();
	}
	else if (aICharacter->characterActionType == ECharacterActionType::Crouching&& aICharacter->armActionType != ECharacterArmActionType::PerfectCatching)
	{
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetPhysicsLinearVelocity(FVector::Zero());
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		Cast<ADodgeBall>(gameState->ball)->sphereComp->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		//Cast<ADodgeBall>(gameState->ball)->sphereComp->SetSimulatePhysics(false);
		FVector ballTargetWorldOffset = aICharacter->GetTransform().TransformPosition(FVector(50, 0, 0));
		gameState->ball->SetActorLocation(FVector(ballTargetWorldOffset.X, ballTargetWorldOffset.Y, gameState->ball->GetActorLocation().Z));

		aICharacter->DoArmAction();
		
		float ballManagerOffsetDis = FVector::Dist(FVector(0, 14.811268, -37.638097), FVector::Zero());
		float currentBallManagerOffsetDis = FVector::Dist(aICharacter->ballManagerCom->GetComponentLocation(), aICharacter->GetMesh()->GetSocketLocation(TEXT("HeadSocket")));
		if (!aICharacter->ballManagerCom->CanObtainBall())
		{
			//aICharacter->FixBallManagerPosition();
			Cast<ADodgeBall>(gameState->ball)->sphereComp->UpdateOverlaps();
			//UE_LOG(LogTemp, Warning, TEXT("cant obtain ball!! %f"), aICharacter->GetCurrentRotation().X);
		}
		
	}
	else if (aICharacter->armActionType == ECharacterArmActionType::PerfectCatching)
	{
		UE_LOG(LogTemp, Warning, TEXT("PerfectCatching"));
		gameState->currentCatchingBallCharac = aICharacter;
		InitState();
		aIState = EAIState::Attacking;
		catchingTryingTimer = 0;
	}
}

void ADodgeballAIController::OnAttacking(float DeltaTime, ADodgeballCharacter* aICharacter, ADodgeballGameState* gameState)
{
	UE_LOG(LogTemp, Warning, TEXT("OnAttacking"));
	//if (gameState->currentCatchingBallCharac == aICharacter)UE_LOG(LogTemp, Warning, TEXT("%d"), (int)aIState);
	if (aICharacter->characterActionType == ECharacterActionType::Crouching) aICharacter->Crouch();

	auto proximateEnemy = gameState->GetProximateEnemyToLocation(aICharacter,aICharacter->GetActorLocation());
	FVector targetPos = proximateEnemy->GetActorLocation();
	FVector ballTargetPos = Cast<ADodgeBall>(gameState->ball)->ballTrajectortyComp->impactLocation;
	FVector targetOffsetFromSelf = aICharacter->GetTransform().InverseTransformPosition(targetPos);
	FVector ballTargetOffsetFromSelf= aICharacter->GetTransform().InverseTransformPosition(ballTargetPos);

	float remainRotation = TurnAI(DeltaTime, aICharacter, proximateEnemy->GetTransform().TransformPosition(FVector(0, 30, 0)) - aICharacter->GetMesh()->GetSocketLocation("HeadSocket"), 20, true);
	
	bool isTargetAimed = FVector::Dist(ballTargetPos, targetPos) < aICharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()-25;

	if (remainRotation <= 5 && aICharacter->armActionType != ECharacterArmActionType::ReadyThrowing)
	{
		aICharacter->DoReadyArmAction();
		throwingTryingTimer = 0;
	}
	else if (remainRotation <= 5&&isTargetAimed && aICharacter->armActionType == ECharacterArmActionType::ReadyThrowing && aICharacter->throwingPower > 1300)
	{
		DoAIAttack(aICharacter, gameState);
		throwingTryingTimer = 0;
	}
	else if(aICharacter->armActionType == ECharacterArmActionType::ReadyThrowing)
	{
		throwingTryingTimer += DeltaTime;
		if (throwingTryingTimer > 2.5f)
		{
			DoAIAttack(aICharacter, gameState);
			throwingTryingTimer = 0;
			return;
		}
		//파워값은 적절하지만, 타겟 위치가 안좋을때
		if (aICharacter->throwingPower > 1300)
		{
			aICharacter->TurnMoveMode(true);
			switch (aICharacter->currentPositionType)
			{
			case ECharacterFieldPositionType::In: //투명한 벽에 부딪히지 않았을때, 즉 내야를 벗어나지 않았을때
				if (targetOffsetFromSelf.X > ballTargetOffsetFromSelf.X)
				{
					aICharacter->Move(FVector2D(1, 0));
					//TurnAI(DeltaTime, aICharacter, gameState->GetProximateEnemyToSelfCharac(aICharacter)->GetActorLocation() - aICharacter->GetActorLocation(), 20, true);
				}
				else
				{
					aICharacter->Move(FVector2D(-1, 0));
				}
				break;
			case ECharacterFieldPositionType::MiddleLine:DoAIAttack(aICharacter, gameState);
				break;
			case ECharacterFieldPositionType::LeftLine:DoAIAttack(aICharacter, gameState);
				break;
			case ECharacterFieldPositionType::RightLine:DoAIAttack(aICharacter, gameState);
				break;
			case ECharacterFieldPositionType::BackLine:DoAIAttack(aICharacter, gameState);
				break;
			}
		}
		//타겟 위치는 괜찮지만, 파워가 안좋을때
		else if(isTargetAimed)
		{

		}
	}
}

void ADodgeballAIController::OnAIAimed(float DeltaTime, ADodgeballCharacter* aICharacter, ADodgeballGameState* gameState)
{
	//aIcontroller에 붙은 aI 행동 확률 스탯에 따라 피할건지 잡을건지 정함.

	auto ball = Cast<ADodgeBall>(gameState->ball);
	auto randomValue = FMath::FRandRange(0.f, 1.f);

	if (targetedAIState == EAimedAIState::Notting)
	{
		if (FMath::Abs(randomValue - aIAbilityComp->ActiveCatchStat) < FMath::Abs(randomValue - aIAbilityComp->ActiveEvasionStat))
		{
			targetedAIState = EAimedAIState::Catching;
		}
		else
		{
			targetedAIState = EAimedAIState::Avoiding;
		}
	}
	//if (targetedAIState == EAimedAIState::Notting) targetedAIState = (EAimedAIState)(int)aIAbilityComp->aiStatCategory;

	switch (targetedAIState)
	{
	case EAimedAIState::Catching:
		if (!gameState->currentCatchingBallCharac)
		{
			FVector goalDir = ball->GetActorLocation() - aICharacter->GetMesh()->GetSocketLocation("HeadSocket");
			TurnAI(DeltaTime, aICharacter, goalDir, 20, true);

			if (aICharacter->armActionType == ECharacterArmActionType::None) aICharacter->DoReadyArmAction();
			FVector ballTargetOffsetFromAICharac = aICharacter->GetTransform().InverseTransformPosition(ball->ballTrajectortyComp->impactLocation);

			if (aICharacter->characterActionType != ECharacterActionType::Jump && aICharacter->characterActionType != ECharacterActionType::Crouching)
			{
				if (ballTargetOffsetFromAICharac.Y > 5)
				{
					aICharacter->TurnMoveMode(false);
					aICharacter->Move(FVector2D(0, 1));
				}
				else if (ballTargetOffsetFromAICharac.Y < -5)
				{
					aICharacter->TurnMoveMode(false);
					aICharacter->Move(FVector2D(0, -1));
				}
				else
				{
					aICharacter->OnMoveCompleted();
					aICharacter->InitMovement();

					if (!ball->OnGrounded.IsBoundToObject(this))
					{
						aICharacter->OnMoveCompleted();
						aICharacter->InitMovement();
						TWeakObjectPtr<ADodgeballAIController> WeakThis(this);
						ball->OnGrounded.AddLambda([WeakThis, ball, gameState]() { WeakThis->InitState(); ball->OnGrounded.RemoveAll(WeakThis.Get()); ball->OnCatched.RemoveAll(WeakThis.Get()); gameState->recentAimedCharac = nullptr; });
						ball->OnCatched.AddLambda([WeakThis, ball, gameState]() {WeakThis->InitState(); ball->OnCatched.RemoveAll(WeakThis.Get()); ball->OnGrounded.RemoveAll(WeakThis.Get()); gameState->recentAimedCharac = nullptr; });
					}
					
					float offsetZOfBallTargetFromAICharac = ball->ballTrajectortyComp->impactLocation.Z - aICharacter->GetActorLocation().Z;
					if (offsetZOfBallTargetFromAICharac <= 5) aICharacter->Crouch();
					else if (offsetZOfBallTargetFromAICharac > 5 && offsetZOfBallTargetFromAICharac <= 100);
					else aICharacter->TryJump();
				}
			}

			if (aICharacter->armActionType == ECharacterArmActionType::ReadyCatching)
			{
				aICharacter->DoArmAction();
			}
			else if (aICharacter->armActionType == ECharacterArmActionType::PerfectCatching)
			{
				gameState->currentCatchingBallCharac = aICharacter;
				InitState();
				targetedAIState = EAimedAIState::Notting;
				aIState = EAIState::Attacking;
			}
		}
		break;
	case EAimedAIState::Avoiding:
		if (!gameState->currentCatchingBallCharac && FVector::Dist2D(ball->ballTrajectortyComp->impactLocation, aICharacter->GetActorLocation()) <
			aICharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2)
		{
			float offsetZOfBallTargetFromAICharac = ball->ballTrajectortyComp->impactLocation.Z - aICharacter->GetActorLocation().Z;
			float offsetYOfBallFromAICharac = aICharacter->GetTransform().InverseTransformPosition(ball->GetActorLocation()).Y;

			if (offsetZOfBallTargetFromAICharac >= aICharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 0.5f && aICharacter->characterActionType != ECharacterActionType::Crouching)
			{
				aICharacter->InitMovement();
				aICharacter->OnMoveCompleted();
				aICharacter->Crouch();
				TWeakObjectPtr<ADodgeballAIController> WeakThis(this);
				ball->OnGrounded.AddLambda([WeakThis, ball, gameState]() { WeakThis->InitState(); ball->OnGrounded.RemoveAll(WeakThis.Get()); ball->OnCatched.RemoveAll(WeakThis.Get()); gameState->recentAimedCharac = nullptr; });
				ball->OnCatched.AddLambda([WeakThis, ball, gameState]() {WeakThis->InitState(); ball->OnCatched.RemoveAll(WeakThis.Get()); ball->OnGrounded.RemoveAll(WeakThis.Get()); gameState->recentAimedCharac = nullptr; });
				targetedAIState = EAimedAIState::Notting;
			}
			else if (offsetZOfBallTargetFromAICharac <= aICharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * -0.5f && aICharacter->characterActionType != ECharacterActionType::Jump)
			{
				aICharacter->InitMovement();
				aICharacter->OnMoveCompleted();
				aICharacter->TryJump();
				TWeakObjectPtr<ADodgeballAIController> WeakThis(this);
				ball->OnGrounded.AddLambda([WeakThis, ball, gameState]() { WeakThis->InitState(); ball->OnGrounded.RemoveAll(WeakThis.Get()); ball->OnCatched.RemoveAll(WeakThis.Get()); gameState->recentAimedCharac = nullptr; });
				ball->OnCatched.AddLambda([WeakThis, ball, gameState]() {WeakThis->InitState(); ball->OnCatched.RemoveAll(WeakThis.Get()); ball->OnGrounded.RemoveAll(WeakThis.Get()); gameState->recentAimedCharac = nullptr; });
				targetedAIState = EAimedAIState::Notting;
			}
			else if (aICharacter->characterActionType != ECharacterActionType::Crouching && aICharacter->characterActionType != ECharacterActionType::Jump&& FMath::Abs(offsetYOfBallFromAICharac)<50)
			{
				FVector goalDir = ball->GetActorLocation() - aICharacter->GetMesh()->GetSocketLocation("HeadSocket");
				TurnAI(DeltaTime, aICharacter, goalDir, 10, true);

				FVector ballOffsetFromAICharac = aICharacter->GetTransform().InverseTransformPosition(ball->GetActorLocation());
				aICharacter->TurnMoveMode(true);
				if (ballOffsetFromAICharac.Y > 0) aICharacter->Move(FVector2D(0, -1));
				else aICharacter->Move(FVector2D(0, 1));
			}
			else if(aICharacter->characterActionType != ECharacterActionType::Crouching && aICharacter->characterActionType != ECharacterActionType::Jump)
			{
				InitState();
			}
		}

		break;
	}
}

void ADodgeballAIController::OnGuarding(float DeltaTime, ADodgeballCharacter* aICharacter, ADodgeballGameState* gameState)
{
	if (aICharacter->currentPositionType == ECharacterFieldPositionType::In)
	{
		FVector goalDir = gameState->ball->GetActorLocation() - aICharacter->GetMesh()->GetSocketLocation("HeadSocket");
		TurnAI(DeltaTime, aICharacter, goalDir, 10, true);

		aICharacter->TurnMoveMode(true);
		aICharacter->Move(FVector2D(-1, 0));
		aICharacter->TurnMoveMode(true);
	}
	else
	{
		if (aICharacter->currentPositionType == ECharacterFieldPositionType::MiddleLine)
		{
			FVector targetPos = floors[aIFloorNums[selectedFloorIndex] - 1]->GetActorLocation();
			FVector goalDir = (targetPos - aICharacter->GetActorLocation());

			if (TurnAI(DeltaTime, aICharacter, -goalDir, 20, false) < 5)
			{
				aICharacter->Move(FVector2D(-1, 0));
				aICharacter->TurnMoveMode(true);
			}
			return;
		}


		if (aIFreeMovingTimer == 0) stateOnFreeMoving = (EAIStateOnFreeMoving)FMath::RandRange(0, 4);
		aIFreeMovingTimer += DeltaTime;
		if (aIFreeMovingTimer > 0.3f)
		{
			aIFreeMovingTimer = 0;
		}

		aICharacter->TurnMoveMode(false);
		switch (stateOnFreeMoving)
		{
		case EAIStateOnFreeMoving::Idle:
			aICharacter->InitMovement();
			aICharacter->OnMoveCompleted();
			break;
		case EAIStateOnFreeMoving::Left:
			//aICharacter->Move(FVector2D(0, -1));
			aICharacter->InitMovement();
			aICharacter->OnMoveCompleted();
			break;
		case EAIStateOnFreeMoving::Right:
			//aICharacter->Move(FVector2D(0, 1));
			aICharacter->InitMovement();
			aICharacter->OnMoveCompleted();
			break;
		case EAIStateOnFreeMoving::Forward:
			aICharacter->Move(FVector2D(1, 0));
			break;
		case EAIStateOnFreeMoving::Back:
			aICharacter->InitMovement();
			aICharacter->OnMoveCompleted();
			//aICharacter->Move(FVector2D(-1, 0));
			break;
		default:
			break;
		}

		FVector goalDir = gameState->ball->GetActorLocation() - aICharacter->GetMesh()->GetSocketLocation("HeadSocket");
		TurnAI(DeltaTime, aICharacter, goalDir, 10, true);
	}
}

void ADodgeballAIController::OnRecentThrowerIsJudgment(float DeltaTime,ADodgeballCharacter* aICharacter, ADodgeballGameState* gameState)
{
	if (aICharacter->teamType == ECharacterTeamType::B && aIFloorNums[0] == 3)
	{
		float disOfBetweenBallAndAI = FVector::Dist2D(gameState->ball->GetActorLocation(), aICharacter->GetActorLocation());
		auto goalDir = (gameState->ball->GetActorLocation() + FVector(0, 0, -10)) -aICharacter->GetMesh()->GetSocketLocation(TEXT("HeadSocket"));
		TurnAI(DeltaTime, aICharacter, goalDir, 10, true);
		if (disOfBetweenBallAndAI >60)
		{
			aICharacter->Move(FVector2D(1, 0));
		}
		else if (aICharacter->armActionType == ECharacterArmActionType::None)
		{
			InitState();
			aICharacter->DoReadyArmAction();
		}
		else if (aICharacter->armActionType != ECharacterArmActionType::PerfectCatching)
		{
			aICharacter->DoArmAction();
		}
	}
}

void ADodgeballAIController::DoAIAttack(ADodgeballCharacter* aICharacter, ADodgeballGameState* gameState)
{
	if (aICharacter->energy >= aICharacter->maxEnergy)
	{
		int skillIndex = FMath::RandRange(0, 3);
		aICharacter->TrySkill((ESkillType)skillIndex);
	}
	else
	{
		aICharacter->DoArmAction();
	}

	auto ballOwnerComp = Cast<ADodgeBall>(gameState->ball)->GetRootComponent()->GetAttachParent();
	if(ballOwnerComp&& ballOwnerComp->GetOwner())
	if (ballOwnerComp->GetOwner() &&Cast<ADodgeballCharacter>(ballOwnerComp->GetOwner()))
	{
		Cast<ADodgeballCharacter>(ballOwnerComp->GetOwner())->ForceToDetachBall();
		InitState();
	}

	//gameState->recentAimedCharac = gameState->GetProximateEnemyToLocation(aICharacter, aICharacter->GetActorLocation());
	gameState->currentCatchingBallCharac = nullptr;
	InitState();
}

float ADodgeballAIController::TurnAI(float DeltaTime, ADodgeballCharacter* aICharacter, FVector goalDir, float turnSpeed, bool bHeadRotation)
{
	FVector goalHeadDir = goalDir;
	
	//몸 회전값 설정
	goalDir.Z = 0;
	goalDir = goalDir.GetSafeNormal();
	FVector lookDir = aICharacter->GetMesh()->GetRightVector();
	float degree = FMath::RadiansToDegrees(FMath::Acos((FVector::DotProduct(lookDir, goalDir))));
	FVector crossDot = FVector::CrossProduct(lookDir, goalDir);
	if (crossDot.Z < 0) degree *= -1;
	degree *= DeltaTime * turnSpeed > 1 ? 1 : DeltaTime * turnSpeed;

	//머리 회전값설정
	float headDegree=0;
	if (bHeadRotation)
	{
		lookDir = aICharacter->GetMesh()->GetSocketTransform(TEXT("HeadSocket")).GetUnitAxis(EAxis::X);

		lookDir *= goalHeadDir.Size();
		goalHeadDir.X = lookDir.X;
		goalHeadDir.Y = lookDir.Y;
		goalHeadDir = goalHeadDir.GetSafeNormal();
		lookDir = lookDir.GetSafeNormal();
		//lookDir.Y = 0;
		headDegree = FMath::RadiansToDegrees(FMath::Acos((FVector::DotProduct(lookDir, goalHeadDir))));
		crossDot = FVector::CrossProduct(lookDir, goalHeadDir);
		//if (crossDot.Y > 0) headDegree *= -1;
		//UE_LOG(LogTemp, Warning, TEXT("%f"), headDegree);
		//if (aIFloorNums[selectedFloorIndex] == 4) UE_LOG(LogTemp, Warning, TEXT("headDgree dif %f"), headDegree);
		headDegree *= DeltaTime * turnSpeed > 1 ? 1 : DeltaTime * turnSpeed;
		if (lookDir.Z > goalHeadDir.Z)headDegree *= -1;
	}

	//회전
	FVector2D rotateVec(degree, headDegree);
	aICharacter->Look(rotateVec);

	if (aICharacter->characterActionType != ECharacterActionType::Crouching && aICharacter->characterActionType != ECharacterActionType::Avoid &&
		aICharacter->characterActionType != ECharacterActionType::Hit)
	aICharacter->AddActorWorldRotation(FRotator(0, degree, 0));

	return FMath::Abs(degree/ (DeltaTime * turnSpeed));
}

void ADodgeballAIController::InitState()
{
	if (this == nullptr || !IsValid(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("initState : this is null!!!!!!!"));
		return;
	}

	auto aICharacter = Cast<ADodgeballCharacter>(GetPawn());
	if (!aICharacter) return;
	switch (aICharacter->characterActionType)
	{
	case ECharacterActionType::Crouching:
		aICharacter->Crouch();
		break;
	case ECharacterActionType::Avoid:
		break;
	case ECharacterActionType::Jump:
		break;
	case ECharacterActionType::Move:
		break;
	}

	switch (aICharacter->armActionType)
	{
	case ECharacterArmActionType::ReadyCatching:
		aICharacter->DoEndReadyArmAction();
		break;
	}

	aICharacter->OnMoveCompleted();
	aICharacter->InitMovement();
	aICharacter->InitLook();
	aIState = EAIState::Notting;
}

void ADodgeballAIController::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup =  TG_DuringPhysics;
	if (Cast<ADodgeballCharacter>(GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("aiCon Begin"));
		Cast<ADodgeballCharacter>(GetPawn())->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	}

	aIAbilityComp = NewObject<UAIAbilityComponent>(this, TEXT("playerAbilityComp"));
	aIAbilityComp->RegisterComponent();
}
void ADodgeballAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	if (aIAbilityComp)
	{
		aIAbilityComp->DestroyComponent();
		aIAbilityComp = nullptr;
	}
}
