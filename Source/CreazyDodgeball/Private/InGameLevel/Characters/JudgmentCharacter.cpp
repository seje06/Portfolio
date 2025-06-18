// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Characters/JudgmentCharacter.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/DodgeballGameState.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/InGameGameMode/InGameGameMode.h"
#include "Components/SphereComponent.h"
#include "InGameLevel/InGameGameMode/InGameGameMode.h"
#include "Components/SphereComponent.h"
#include "InGameLevel/InGameGameMode/InGameUIComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
// Sets default values
AJudgmentCharacter::AJudgmentCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CapMeshComp"));
	capMesh->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
	capMesh->SetRelativeLocation(FVector(6.25f, -0.02f, 13.10));
	capMesh->SetRelativeRotation(FRotator(1.07f, 269.18f, 3.73f));
	capMesh->SetRelativeScale3D(FVector(0.17f, 0.17f, 0.17f));
}

// Called when the game starts or when spawned
void AJudgmentCharacter::BeginPlay()
{
	Super::BeginPlay();

	capMesh->SetRelativeLocation(FVector(6.25f, -0.02f, 13.10));
	capMesh->SetRelativeRotation(FRotator(1.07f, 269.18f, 3.73f));
	capMesh->SetRelativeScale3D(FVector(0.17f, 0.17f, 0.17f));
	
	//ball->OnCatched.AddDynamic(this, &AJudgmentCharacter::OnBallActionEnded);
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;

	initLocation = GetActorLocation();
	//gameState->isSetStarting = &isIniting;
	gameState->judgementInitLocation = initLocation;
	gameState->isThrowerJudgment = &isThrowerJudgment;

	auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->OnSetStarted.BindUObject(this,&AJudgmentCharacter::Init);
	gameState->currentSet= &gameMode->currentSet;
	gameMode->GameStart();
}

void AJudgmentCharacter::OnBallActionEnded()
{
	//게임 스테이트를 업데이트하여 어느 팀이 공을 잡을지 정한다.
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;
	auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	if (!gameMode) return;

	if (gameState->isSetStarting)return;
	if (gameState->isGameEnded) return;
	//if (ball->controllingSkillType != EControllingSkillType::None) return;

	//gameState->catchingTryingProgressingTime = 0.5f;

	//공을 잡은 캐릭터
	ADodgeballCharacter* caughtChar = Cast<ADodgeballCharacter>(gameState->currentCatchingBallCharac);
	ADodgeballCharacter* thrower = Cast<ADodgeballCharacter>(ball->recentThrownCharacter);
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//공이 땅에 떨어 졌을 때
	if (ball->bIsGrounded)
	{	
		//부메랑일때는 리턴
		if (ball->controllingSkillType == ECharacterSkillType::Boomerang)
		{
			ball->OnCatched.Clear();
			ball->OnGrounded.Clear();
			//TWeakObjectPtr<AJudgmentCharacter> WeakThis(this);
			ball->OnCatched.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
			ball->OnGrounded.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
			gameState->recentAimedCharac = nullptr;
			return;
		}
		//gameState->proximateCharacterToBall = nullptr;
		
		//공이 밖으로 나갔을 때
		if (gameState->currentBallPositionType == EBallPositionType::Notting)
		{
			//심판이 공을 소환시킴
			if (ball->hitCharacters.Num() > 0) //맞을 애로 부터 나갔을때
			{
				auto hitCharac = Cast<ADodgeballCharacter>(ball->hitCharacters[0]);
				switch (hitCharac->teamType)
				{
				case ECharacterTeamType::A:
					gameState->currentHavingTurnTeamType = ECharacterTeamType::B;
					break;
				case ECharacterTeamType::B:
					gameState->currentHavingTurnTeamType = ECharacterTeamType::A;
					break;
				case ECharacterTeamType::None:
					break;
				}
			}
			else if (thrower) //던진애로 부터 나갔을때
			{
				switch (thrower->teamType)
				{
				case ECharacterTeamType::A:
					gameState->currentHavingTurnTeamType = ECharacterTeamType::B;
					break;
				case ECharacterTeamType::B:
					gameState->currentHavingTurnTeamType = ECharacterTeamType::A;
					break;
				case ECharacterTeamType::None:
					break;
				}
			}
			else //땅맞고 나갈때
			{
				//랜덤으로
				gameState->currentHavingTurnTeamType = (ECharacterTeamType)FMath::RandRange(0, 1);
			}

			//턴이
			InitBallPositionOnTurnEnded();
		}
		/*gameState->currentBallPositionType = EBallPositionType::In;
		gameState->currentHavingBallTeamType = ECharacterTeamType::A;*/
	}

	//맞은 애들 체크		
	if (!caughtChar && thrower)
	{
		for (auto& hitter : ball->hitCharacters)
		{
			//같은 팀이 아니면
			if (Cast<ADodgeballCharacter>(hitter)->teamType != thrower->teamType)
			{

				gameMode->OutCharacter(hitter);
				UE_LOG(LogTemp, Warning, TEXT("out charac!"));

			}
		}
		//gameMode->AddScore(thrower->teamType, hitCount);
	}
	//잡았을때
	else if(caughtChar)
	{
		//공이 땅에 떨어지지 않은
		//던진애가 있을때
		if (thrower && !ball->bIsGrounded)
		{
			if (caughtChar->teamType != thrower->teamType)//던진놈 팀이랑 잡은놈 팀이랑 다를때
			{
				//기력포인트를 증가
				if (caughtChar->teamType == ECharacterTeamType::A) caughtChar->AddEnergy(1);
				else caughtChar->AddEnergy(1.5f);
				gameMode->ReviveCharacter(caughtChar->teamType);


				//gameState->currentHavingTurnTeamType = caughtChar->teamType;
			}
			else
			{
				for (auto& hitter : ball->hitCharacters)
				{
					//같은 팀이 아니면
					if (Cast<ADodgeballCharacter>(hitter)->teamType != thrower->teamType)
					{
						
						gameMode->OutCharacter(hitter);
						UE_LOG(LogTemp, Warning, TEXT("out charac!"));
						
					}
				}
				//gameMode->AddScore(thrower->teamType, hitCount);
			}
		}

		if (gameState->currentAliveTeamACharacterCount > 0 && gameState->currentAliveTeamBCharacterCount > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d"), (int)gameState->currentHavingTurnTeamType);
			//턴을 잡은애 쪽으로 변경.
			gameState->currentHavingTurnTeamType = caughtChar->teamType;
			gameState->currentBallPositionType = EBallPositionType::In;
		}

	}

	if (isThrowerJudgment)isThrowerJudgment = false;

	//턴이 끝났을 때
	InitOnTurnEnded();
	
	//세트가 끝났을 때
	if (gameState->isSetEnded)
	{
		if (gameState->isGameEnded) //게임이 끝났을 때
		{
			gameState->currentHavingTurnTeamType = ECharacterTeamType::None;
			gameState->currentBallPositionType = EBallPositionType::Notting;
			if (!gameState->isSetStarting)
			{
				gameMode->StartSet();
				gameState->isSetStarting = true;
			}
		}
		else
		{
			if (!gameState->isSetStarting)
			{
				gameMode->StartSet();
				gameState->isSetStarting = true;
			}
		}
	}
	
}

void AJudgmentCharacter::Init()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	//상태 초기화
	gameState->currentHavingTurnTeamType = ECharacterTeamType::None;
	gameState->currentBallPositionType = EBallPositionType::Notting;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	//GetMesh()->GetAnimInstance()->Montage_Stop();
	//승리한 팀에게 신호 줌. 첫 세트 시작이라면 안함.
	if (gameState->recentWinnerTeam == ECharacterTeamType::A)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(aM_WinJesture);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("A"), aM_WinJesture);
		UGameplayStatics::PlaySoundAtLocation(this, whistleSound, GetActorLocation());
	}
	else if (gameState->recentWinnerTeam == ECharacterTeamType::B)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(aM_WinJesture);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(TEXT("B"), aM_WinJesture);
		UGameplayStatics::PlaySoundAtLocation(this, whistleSound, GetActorLocation());
	}

	float newSetDelay = 2;
	if (gameState->recentWinnerTeam == ECharacterTeamType::None) newSetDelay = 1;
	if (!gameState->isGameEnded)
	{
		//1초 뒤 볼 초기화
		GetWorld()->GetTimerManager().SetTimer(timerHandleInitBall, this, &AJudgmentCharacter::InitBall, newSetDelay, false);
	
		//2초 뒤 던지기 시작
	
		GetWorld()->GetTimerManager().SetTimer(timerHandleStartThrow, this, &AJudgmentCharacter::StartThrow, newSetDelay + 1, false);
	}
	else
	{
		auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
		GetWorld()->GetTimerManager().SetTimer(timerHandleEndGame, gameMode, &AInGameGameMode::EndGame, 2, false);

		//게임 끝나면 ui모드로
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
	//플레이어 입력 무시
	Cast<ADodgeballCharacter>(PC->GetPawn())->DisableInput(PC);

}

void AJudgmentCharacter::InitBall()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	//볼 생성하고 게임 스테이트에 할당.
	if (ball)
	{
		ball->OnCatched.Clear();
		ball->OnGrounded.Clear();
		ball->Destroy();
	}

	SetActorLocation(initLocation);

	ball = GetWorld()->SpawnActor<ADodgeBall>(ballClass);
	ball->sphereComp->SetSimulatePhysics(false);
	ball->GetRootComponent()->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("hand_rSocket"));
	ball->GetRootComponent()->SetRelativeLocation(FVector(0, ball->sphereComp->GetUnscaledSphereRadius() * 0.25f, 0));
	ball->sphereComp->SetAllPhysicsLinearVelocity(FVector::Zero());

	gameState->ball = ball;
	ball->OnCatched.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
	ball->OnGrounded.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);

	GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, aM_WinJesture);
}

void AJudgmentCharacter::StartThrow()
{
	//플레이어 입력 풀기
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	Cast<ADodgeballCharacter>(PC->GetPawn())->EnableInput(PC);

	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	isThrowerJudgment = true;
	gameState->isThrowerJudgment = &isThrowerJudgment;

	gameState->currentAliveTeamACharacterCount = gameState->floorCountOfOneTeam;
	gameState->currentAliveTeamBCharacterCount = gameState->floorCountOfOneTeam;

	gameState->currentHavingTurnTeamType = ECharacterTeamType::None;
	gameState->currentBallPositionType = EBallPositionType::Notting;

	GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, aM_ThrowingUp);
	GetMesh()->GetAnimInstance()->Montage_Play(aM_ThrowingUp);
	UGameplayStatics::PlaySoundAtLocation(this, whistleSound, GetActorLocation());

	gameState->isSetStarting = false;

	//배경 사운드
	if (!audioComp || !audioComp->IsValidLowLevel())
	{
		audioComp = NewObject<UAudioComponent>(this);
		audioComp->bAutoActivate = false;
		audioComp->bAutoDestroy = false;
		audioComp->SetSound(backgroundSound);
		audioComp->SetupAttachment(RootComponent);
		audioComp->RegisterComponent();
		audioComp->OnAudioFinished.AddDynamic(this, &AJudgmentCharacter::PlayBackgroundSound);
		audioComp->Play();
	}
}

void AJudgmentCharacter::PlayBackgroundSound()
{
	audioComp->Play();
}

// Called every frame
void AJudgmentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	if (!gameMode) return;

	//세트가 시작중 일 때 캐릭터 심판을 보게끔 회전
	if (!PC->GetPawn()) return;
	if (auto player = Cast<ADodgeballCharacter>(PC->GetPawn()))
	{
		if ((gameState->isSetStarting && !isThrowerJudgment) && !gameState->isProgressingFirstSetIntro)
		{
			if (player->characterActionType == ECharacterActionType::Crouching) player->Crouch();
			player->DoEndReadyArmAction();
			player->OnMoveCompleted();
			
			FVector goalLook = GetActorLocation() - player->GetActorLocation();
			TurnCharac(DeltaTime, player, goalLook, 10, true);
		}
	}

	//턴을 진행 시간 체크
	if (ball && !gameState->isSetStarting)
	{
		if (gameState->currentHavingTurnTeamType == ECharacterTeamType::None)
		{
			turnProgressingTime = 10;
			preHavingTurnTeam = ECharacterTeamType::None;
		}
		else
		{
			if (preHavingTurnTeam == ECharacterTeamType::None)
			{
				preHavingTurnTeam = gameState->currentHavingTurnTeamType;
			}
			else
			{
				if (preHavingTurnTeam != gameState->currentHavingTurnTeamType)
				{
					preHavingTurnTeam = gameState->currentHavingTurnTeamType;
					turnProgressingTime = 10;
				}
				turnProgressingTime -= DeltaTime;

				//턴을 가진채 10초가 지났을 때
				if (turnProgressingTime < 0.f)
				{
					if (gameState->currentCatchingBallCharac)
					{
						//강제로 볼 뺏고 볼 위치 상대 팀으로
						auto chatchingBallCharac = Cast<ADodgeballCharacter>(gameState->currentCatchingBallCharac);
						chatchingBallCharac->ForceToDetachBall();
						turnProgressingTime = 10;
						if (chatchingBallCharac->teamType == ECharacterTeamType::A) gameState->currentHavingTurnTeamType = ECharacterTeamType::B;
						else gameState->currentHavingTurnTeamType = ECharacterTeamType::A;
						InitBallPositionOnTurnEnded();
						InitOnTurnEnded();
						turnProgressingTime = 10;
					}
					else if (ball->bIsGrounded)
					{
						turnProgressingTime = 10;
						if (gameState->currentHavingTurnTeamType == ECharacterTeamType::A) gameState->currentHavingTurnTeamType = ECharacterTeamType::B;
						else gameState->currentHavingTurnTeamType = ECharacterTeamType::A;
						InitBallPositionOnTurnEnded();
						InitOnTurnEnded();
						turnProgressingTime = 10;
					}
					else if (ball->recentThrownCharacter && Cast<ADodgeballCharacter>(ball->recentThrownCharacter)->teamType == preHavingTurnTeam)
					{

					}

				}
			}
		}

	}

	if (gameState->currentHavingTurnTeamType == ECharacterTeamType::A)
	{
		gameMode->inGameUIComponent->UpdateShotClock(ECharacterTeamType::A, turnProgressingTime);
	}
	else if (gameState->currentHavingTurnTeamType == ECharacterTeamType::B)
	{
		gameMode->inGameUIComponent->UpdateShotClock(ECharacterTeamType::B, turnProgressingTime);
	}

	//배경 브금 
	if (audioComp && !audioComp->IsPlaying() && !gameState->isGameEnded)
	{
		audioComp->Play();
	}
	else if (audioComp && audioComp->IsPlaying() && gameState->isGameEnded)
	{
		audioComp->Stop();
	}
}

// Called to bind functionality to input
void AJudgmentCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJudgmentCharacter::InitBallPositionOnTurnEnded()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	int radomMultiValueX = FMath::RandRange(1, 3);
	int radomMultiValueY = FMath::RandRange(1, 3);
	switch (gameState->currentHavingTurnTeamType)
	{
	case ECharacterTeamType::A:
		ball->SetActorLocation(GetTransform().TransformPosition(FVector(300 * radomMultiValueX, 200 * radomMultiValueY, 0)));
		ball->sphereComp->SetPhysicsLinearVelocity(FVector(0, 0, ball->sphereComp->GetPhysicsLinearVelocity().Z));
		break;
	case ECharacterTeamType::B:
		ball->SetActorLocation(GetTransform().TransformPosition(FVector(300 * radomMultiValueX, -200 * radomMultiValueY, 0)));
		ball->sphereComp->SetPhysicsLinearVelocity(FVector(0, 0, ball->sphereComp->GetPhysicsLinearVelocity().Z));
		break;
	case ECharacterTeamType::None:

		break;
	}
	ball->sphereComp->SetEnableGravity(true);
	ball->sphereComp->SetAllPhysicsLinearVelocity(FVector::Zero());
	ball->sphereComp->SetAllPhysicsAngularVelocityInRadians(FVector::Zero());
	ball->sphereComp->SetAllPhysicsAngularVelocityInDegrees(FVector::Zero());
	ball->sphereComp->SetPhysicsLinearVelocity(FVector(0, 0, -100));
	ball->bIsGrounded = false;
}

void AJudgmentCharacter::InitOnTurnEnded()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	//볼 정보 초기화
	ball->recentThrownCharacter = nullptr;
	ball->hitCharacters.Empty();

	//캐릭 초기화
	for (auto& charac : *gameState->charactersA)if (charac) charac->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	for (auto& charac : *gameState->charactersB)if (charac)  charac->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	ball->OnCatched.Clear();
	ball->OnGrounded.Clear();
	//TWeakObjectPtr<AJudgmentCharacter> WeakThis(this);
	ball->OnCatched.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
	ball->OnGrounded.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
	//게임 스테이트 초기화
	gameState->recentAimedCharac = nullptr;

	if (preHavingTurnTeam != gameState->currentHavingTurnTeamType) turnProgressingTime = 10;

	preHavingTurnTeam = gameState->currentHavingTurnTeamType;
}

void AJudgmentCharacter::MoveForThrow()
{
	TWeakObjectPtr<AJudgmentCharacter> WeakThis(this);
	TWeakObjectPtr<UWorld> WeakWorld(GetWorld());
	//FTimerHandle* moveTimerHandle = new FTimerHandle();

	isThrowingUP = true;
	GetWorld()->GetTimerManager().SetTimer(moveTickTimerHandle, [WeakThis]() mutable {
		//AddMovementInput(GetMesh()->GetRightVector(), 1);
		if (!WeakThis.IsValid())
		{
			/*if (WeakWorld.IsValid())
			{
				WeakWorld.Get()->GetTimerManager().ClearTimer(*moveTimerHandle);
			}*/
			//world->GetTimerManager().ClearTimer(*moveTimerHandle);
			//if(moveTimerHandle) delete moveTimerHandle;
			//moveTimerHandle = nullptr;

			UE_LOG(LogTemp, Warning, TEXT("judge is null!!"));
			return;
			//FTimerHandle* tempTimerHandle=moveTimeHan
		}
		WeakThis.Get()->AddActorWorldOffset(WeakThis.Get()->GetActorForwardVector() * 0.02f * 75);
		}, 0.02f, true);

	UE_LOG(LogTemp, Warning, TEXT("success Throw!"));
	
}

void AJudgmentCharacter::MoveStop()
{
	GetWorld()->GetTimerManager().ClearTimer(moveTickTimerHandle);
}

void AJudgmentCharacter::ThrowUp()
{
	if (!ball) return;

	ball->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	ball->sphereComp->SetSimulatePhysics(true);
	ball->LaunchBall(FVector::UpVector, 1000);
	isThrowerJudgment = true;
	isThrowingUP = false;
}

float AJudgmentCharacter::TurnCharac(float DeltaTime, ADodgeballCharacter* aICharacter, FVector goalDir, float turnSpeed, bool bHeadRotation)
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
	float headDegree = 0;
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
		headDegree *= DeltaTime * turnSpeed > 1 ? 1 : DeltaTime * turnSpeed;
		if (lookDir.Z > goalHeadDir.Z)headDegree *= -1;
	}

	//회전
	FVector2D rotateVec(degree, headDegree);
	aICharacter->Look(rotateVec);

	/*if (aICharacter->characterActionType != ECharacterActionType::Crouching && aICharacter->characterActionType != ECharacterActionType::Avoid &&
		aICharacter->characterActionType != ECharacterActionType::Hit)
		aICharacter->AddActorWorldRotation(FRotator(0, degree, 0));*/

	return FMath::Abs(degree / (DeltaTime * turnSpeed));
}

void AJudgmentCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(moveTickTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(timerHandleInitBall);
	GetWorld()->GetTimerManager().ClearTimer(timerHandleStartThrow);
	GetWorld()->GetTimerManager().ClearTimer(timerHandleEndGame);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

