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
	//���� ������Ʈ�� ������Ʈ�Ͽ� ��� ���� ���� ������ ���Ѵ�.
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;
	auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	if (!gameMode) return;

	if (gameState->isSetStarting)return;
	if (gameState->isGameEnded) return;
	//if (ball->controllingSkillType != EControllingSkillType::None) return;

	//gameState->catchingTryingProgressingTime = 0.5f;

	//���� ���� ĳ����
	ADodgeballCharacter* caughtChar = Cast<ADodgeballCharacter>(gameState->currentCatchingBallCharac);
	ADodgeballCharacter* thrower = Cast<ADodgeballCharacter>(ball->recentThrownCharacter);
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//���� ���� ���� ���� ��
	if (ball->bIsGrounded)
	{	
		//�θ޶��϶��� ����
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
		
		//���� ������ ������ ��
		if (gameState->currentBallPositionType == EBallPositionType::Notting)
		{
			//������ ���� ��ȯ��Ŵ
			if (ball->hitCharacters.Num() > 0) //���� �ַ� ���� ��������
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
			else if (thrower) //�����ַ� ���� ��������
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
			else //���°� ������
			{
				//��������
				gameState->currentHavingTurnTeamType = (ECharacterTeamType)FMath::RandRange(0, 1);
			}

			//����
			InitBallPositionOnTurnEnded();
		}
		/*gameState->currentBallPositionType = EBallPositionType::In;
		gameState->currentHavingBallTeamType = ECharacterTeamType::A;*/
	}

	//���� �ֵ� üũ		
	if (!caughtChar && thrower)
	{
		for (auto& hitter : ball->hitCharacters)
		{
			//���� ���� �ƴϸ�
			if (Cast<ADodgeballCharacter>(hitter)->teamType != thrower->teamType)
			{

				gameMode->OutCharacter(hitter);
				UE_LOG(LogTemp, Warning, TEXT("out charac!"));

			}
		}
		//gameMode->AddScore(thrower->teamType, hitCount);
	}
	//�������
	else if(caughtChar)
	{
		//���� ���� �������� ����
		//�����ְ� ������
		if (thrower && !ball->bIsGrounded)
		{
			if (caughtChar->teamType != thrower->teamType)//������ ���̶� ������ ���̶� �ٸ���
			{
				//�������Ʈ�� ����
				if (caughtChar->teamType == ECharacterTeamType::A) caughtChar->AddEnergy(1);
				else caughtChar->AddEnergy(1.5f);
				gameMode->ReviveCharacter(caughtChar->teamType);


				//gameState->currentHavingTurnTeamType = caughtChar->teamType;
			}
			else
			{
				for (auto& hitter : ball->hitCharacters)
				{
					//���� ���� �ƴϸ�
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
			//���� ������ ������ ����.
			gameState->currentHavingTurnTeamType = caughtChar->teamType;
			gameState->currentBallPositionType = EBallPositionType::In;
		}

	}

	if (isThrowerJudgment)isThrowerJudgment = false;

	//���� ������ ��
	InitOnTurnEnded();
	
	//��Ʈ�� ������ ��
	if (gameState->isSetEnded)
	{
		if (gameState->isGameEnded) //������ ������ ��
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
	//���� �ʱ�ȭ
	gameState->currentHavingTurnTeamType = ECharacterTeamType::None;
	gameState->currentBallPositionType = EBallPositionType::Notting;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	//GetMesh()->GetAnimInstance()->Montage_Stop();
	//�¸��� ������ ��ȣ ��. ù ��Ʈ �����̶�� ����.
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
		//1�� �� �� �ʱ�ȭ
		GetWorld()->GetTimerManager().SetTimer(timerHandleInitBall, this, &AJudgmentCharacter::InitBall, newSetDelay, false);
	
		//2�� �� ������ ����
	
		GetWorld()->GetTimerManager().SetTimer(timerHandleStartThrow, this, &AJudgmentCharacter::StartThrow, newSetDelay + 1, false);
	}
	else
	{
		auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
		GetWorld()->GetTimerManager().SetTimer(timerHandleEndGame, gameMode, &AInGameGameMode::EndGame, 2, false);

		//���� ������ ui����
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
	//�÷��̾� �Է� ����
	Cast<ADodgeballCharacter>(PC->GetPawn())->DisableInput(PC);

}

void AJudgmentCharacter::InitBall()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	//�� �����ϰ� ���� ������Ʈ�� �Ҵ�.
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
	//�÷��̾� �Է� Ǯ��
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

	//��� ����
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

	//��Ʈ�� ������ �� �� ĳ���� ������ ���Բ� ȸ��
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

	//���� ���� �ð� üũ
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

				//���� ����ä 10�ʰ� ������ ��
				if (turnProgressingTime < 0.f)
				{
					if (gameState->currentCatchingBallCharac)
					{
						//������ �� ���� �� ��ġ ��� ������
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

	//��� ��� 
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

	//�� ���� �ʱ�ȭ
	ball->recentThrownCharacter = nullptr;
	ball->hitCharacters.Empty();

	//ĳ�� �ʱ�ȭ
	for (auto& charac : *gameState->charactersA)if (charac) charac->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	for (auto& charac : *gameState->charactersB)if (charac)  charac->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	ball->OnCatched.Clear();
	ball->OnGrounded.Clear();
	//TWeakObjectPtr<AJudgmentCharacter> WeakThis(this);
	ball->OnCatched.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
	ball->OnGrounded.AddUObject(this, &AJudgmentCharacter::OnBallActionEnded);
	//���� ������Ʈ �ʱ�ȭ
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

	//�� ȸ���� ����
	goalDir.Z = 0;
	goalDir = goalDir.GetSafeNormal();
	FVector lookDir = aICharacter->GetMesh()->GetRightVector();
	float degree = FMath::RadiansToDegrees(FMath::Acos((FVector::DotProduct(lookDir, goalDir))));
	FVector crossDot = FVector::CrossProduct(lookDir, goalDir);
	if (crossDot.Z < 0) degree *= -1;
	degree *= DeltaTime * turnSpeed > 1 ? 1 : DeltaTime * turnSpeed;

	//�Ӹ� ȸ��������
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

	//ȸ��
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

