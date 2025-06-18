// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/InGameGameMode/InGameGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/DodgeballGameState.h"
#include "InGameLevel/Player/DodgeballPlayerController.h"
#include "InGameLevel/AI/DodgeballAIManager.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/InGameGameMode/InGameUIComponent.h"
#include "InGameLevel/Ball/BallManagerComponent.h"
#include "InGameLevel/UI/FadeWidget.h"
#include "DodgeballGameInstance.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

AInGameGameMode::AInGameGameMode()
{

}

void AInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	Cast<ADodgeballGameState>(GetWorld()->GetGameState())->currentSet = &currentSet;
}

void AInGameGameMode::ShowControlGuide()
{
	if (!controlGuideWidgetClass) return;

	APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!pc) return;

	controlGuideWidget = CreateWidget<UUserWidget>(pc, controlGuideWidgetClass);
	if (controlGuideWidget)
	{
		controlGuideWidget->AddToViewport();

		FInputModeGameAndUI inputMode;
		inputMode.SetWidgetToFocus(controlGuideWidget->TakeWidget());
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		pc->SetInputMode(inputMode);
		pc->bShowMouseCursor = true;

		// ���� ����
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

}

void AInGameGameMode::OnControlGuideClosed()
{
	if (controlGuideWidget)
	{
		controlGuideWidget->RemoveFromParent();
		controlGuideWidget = nullptr;
	}

	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	APlayerController* pc = Cast<APlayerController>(player->GetController());
	if (pc)
	{
		//�Է��� �ٽ� ���� �������� ����
		FInputModeGameOnly inputMode;
		pc->SetInputMode(inputMode);
		pc->bShowMouseCursor = false;
	}

	// ���� �簳, ���߿� �� �� �۾��� �ʿ��ҵ�.
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	bControlGuideClosed = true;

	// ����â ���� �� �ΰ��� UI ����
	InitGameUI();

	//���� ���� ���� ���� (��: StartSet)

	StartSet();
}

void AInGameGameMode::AddScore(ECharacterTeamType _teamType, int32 _amount)
{
	if (_teamType == ECharacterTeamType::A)
	{
		teamAScore += _amount;
		if (isTimeOut)
		{
			ProceedSet(ECharacterTeamType::A);
			isTimeOut = false;
			return;
		}
	}
	else if (_teamType == ECharacterTeamType::B)
	{
		teamBScore += _amount;
		if (isTimeOut) 
		{
			ProceedSet(ECharacterTeamType::B);
			isTimeOut = false;
			return;
		}
	}

	// ��Ʈ �¸� ���� �˻�
	if (teamAScore >= scoreToWinSet)
	{
		ProceedSet(ECharacterTeamType::A);
	}
	else if (teamBScore >= scoreToWinSet)
	{
		ProceedSet(ECharacterTeamType::B);
	}

	if (inGameUIComponent)
	{
		inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);
	}

}

void AInGameGameMode::SubtractScore(ECharacterTeamType _teamType, int32 _amount)
{
	if (_teamType == ECharacterTeamType::A)
	{
		teamAScore = FMath::Max(0, teamAScore - _amount);
		if (isTimeOut) 
		{
			ProceedSet(ECharacterTeamType::B);
			isTimeOut = false;
		}
	}
	else if (_teamType == ECharacterTeamType::B)
	{
		teamBScore = FMath::Max(0, teamBScore - _amount);
		if (isTimeOut)
		{
			ProceedSet(ECharacterTeamType::A);
			isTimeOut = false;
		}
	}

	if (inGameUIComponent)
	{
		inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);
	}
}

void AInGameGameMode::ProceedSet(ECharacterTeamType _winnerTeam)
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (!gameState) return;
	if (gameState->isSetEnded) return;


	if (_winnerTeam == ECharacterTeamType::A)
	{
		teamASetWin++;
		if (inGameUIComponent) inGameUIComponent->UpdateSetScore(_winnerTeam, teamASetWin);
		gameState->currentAliveTeamBCharacterCount = 0;
	}
	else if (_winnerTeam == ECharacterTeamType::B)
	{
		teamBSetWin++;
		if (inGameUIComponent) inGameUIComponent->UpdateSetScore(_winnerTeam, teamBSetWin);
		gameState->currentAliveTeamACharacterCount = 0;
	}
	
	gameState->recentWinnerTeam = _winnerTeam;

	// UI�� ���� ���� �ݿ�
	/*if (inGameUIComponent)
	{
		inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);
		inGameUIComponent->UpdateSetText(currentSet);
	}*/

	// ���� ��Ʈ ����
	//StartSet();  // (��ġ �ʱ�ȭ ���� ���⿡)
	

	InitSet();
	if (isTimeOut)StartSet();
	//StartSet();
}

void AInGameGameMode::StartSet()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->isSetStarting = true;
	isTimeOut = false;
	gameState->isSetEnded = false;

	if (currentSet == 1)
	{
		//�÷��̾� �Է� ����
		GetWorld()->GetFirstPlayerController()->DisableInput(GetWorld()->GetFirstPlayerController());
		GetWorld()->GetFirstPlayerController()->GetPawn()->DisableInput(GetWorld()->GetFirstPlayerController());
		//ù��Ʈ�� ���̵� �θ�
		inGameUIComponent->FadeIn(1.f);
		//������ �ִ� ������ ���� ��������
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelSequenceActor::StaticClass(), FoundActors);
		levelSequenceActor =Cast<ALevelSequenceActor>(FoundActors[0]);

		if (levelSequenceActor)
		{
			FMovieSceneSequencePlaybackSettings playbackSettings;
			playbackSettings.bAutoPlay = true;

			//ALevelSequenceActor* outActor = nullptr;
			//auto sequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), inGameIntroSequence, playbackSettings, outActor);

			if (levelSequenceActor->SequencePlayer)
			{

				TWeakObjectPtr<AInGameGameMode> weakThis(this);
				//�ó׸�ƽ ��� 
				levelSequenceActor->SequencePlayer->Play();
				GetWorld()->GetTimerManager().SetTimer(firstSetHandleOnFadeInEnded, [weakThis,gameState]()
					{
						if (!weakThis.IsValid()) return;

						for (auto& charac : *gameState->charactersA) charac->WarmingUp();
						for (auto& charac : *gameState->charactersB) charac->WarmingUp();
						
					}, 1, false);

				//�ó׸�ƽ ��� �ð���ŭ ��� �� ��Ʈ ����
				GetWorld()->GetTimerManager().SetTimer(handleOnCinematicEnded, [weakThis,gameState]()
					{
						if (!weakThis.IsValid()) return;

						weakThis->GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(weakThis->GetWorld()->GetFirstPlayerController()->GetPawn(), 1.f);

						weakThis->OnSetStarted.ExecuteIfBound();

						weakThis->inGameUIComponent->StartGameTimer();
						weakThis->inGameUIComponent->UpdateScoreText(weakThis->teamAScore, weakThis->teamBScore);

						for (auto& charac : *gameState->charactersA) charac->GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);
						for (auto& charac : *gameState->charactersB) charac->GetMesh()->GetAnimInstance()->StopAllMontages(0.1f);

						gameState->isProgressingFirstSetIntro = false;

						//�÷��̾� �Է� ������ Ǯ��
						//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(weakThis->GetWorld(), 0);
						weakThis->GetWorld()->GetFirstPlayerController()->EnableInput(weakThis->GetWorld()->GetFirstPlayerController());
						weakThis->GetWorld()->GetFirstPlayerController()->GetPawn()->EnableInput(weakThis->GetWorld()->GetFirstPlayerController());

					}, levelSequenceActor->SequencePlayer->GetDuration().AsSeconds() , false);
			}
		}
		else
		{
			OnSetStarted.ExecuteIfBound();

			inGameUIComponent->StartGameTimer();
			inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);
		}

		//ù��Ʈ�� ���̵� �θ�
		//inGameUIComponent->FadeIn(1.f);
		/*OnSetStarted.ExecuteIfBound();

		inGameUIComponent->StartGameTimer();
		inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);*/
	}
	else if(!gameState->isGameEnded)
	{
		//���̵� �ƿ� �ϰ� ���� ���� 1�� ��
		inGameUIComponent->FadeOut(2.0f);
		GetWorld()->GetTimerManager().SetTimer(timerHandleFade, [this]() mutable
			{
				aIManager->DestroyAllAI();
				aIManager->Init();
				inGameUIComponent->FadeIn(1.0f);

				inGameUIComponent->StartGameTimer();
				inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);

				auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
				player->DoReadyArmAction();
				player->DoArmAction();
				player->ForceToDetachBall();

			}, 2.0f, false);

		OnSetStarted.ExecuteIfBound();
	}
	else
	{
		OnSetStarted.ExecuteIfBound();
	}
}

void AInGameGameMode::InitSet()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->isSetEnded = true;
	currentSet++;

	// ��� ���� ���� Ȯ��
	if (teamASetWin >= 2)
	{
		// ���� ���� ó�� (��: �¸��� UI ǥ��, �̵� ���� ��)
		UE_LOG(LogTemp, Warning, TEXT("Game Over! Winner: %s"),
			*UEnum::GetValueAsString(ECharacterTeamType::A));
		gameState->isGameEnded = true;

		return;
	}
	else if (teamBSetWin >= 2)
	{
		// ���� ���� ó�� (��: �¸��� UI ǥ��, �̵� ���� ��)
		UE_LOG(LogTemp, Warning, TEXT("Game Over! Winner: %s"),
			*UEnum::GetValueAsString(ECharacterTeamType::B));
		gameState->isGameEnded = true;

		return;
	}


	// ���� �ʱ�ȭ
	teamAScore = 0;
	teamBScore = 0;

	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//������ִ¾� ������ ������ ����
	//auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (gameState->currentCatchingBallCharac)
	{
		auto catchingBallCharac = Cast<ADodgeballCharacter>(gameState->currentCatchingBallCharac);

		catchingBallCharac->DoReadyArmAction();
		catchingBallCharac->DoArmAction();

		catchingBallCharac->ForceToDetachBall();
		gameState->currentCatchingBallCharac = nullptr;
	}
	//��Ʈ �ʱ�ȭ
	player->AddEnergy(-100);
	/*inGameUIComponent->StartGameTimer();
	inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);*/
}

void AInGameGameMode::GameStart()
{
	//ui������Ʈ ã�Ƽ� ������ �Ҵ�
	inGameUIComponent = Cast<UInGameUIComponent>(FindComponentByClass<UInGameUIComponent>());
	if(!inGameUIComponent->OnUITimeOut.IsBoundToObject(this)) inGameUIComponent->OnUITimeOut.BindUObject(this, &AInGameGameMode::OnTimeOut);

	//�÷��̾� ��Ʈ�ѷ� �ʱ�ȭ, ai �Ŵ��� ����.
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	auto playerController = Cast<ADodgeballPlayerController>(player->GetController());
	if (!playerController) return;
	playerController->Init();
	aIManager = GetWorld()->SpawnActor<ADodgeballAIManager>(aiManagerClass);

	// ���� ���� �̸� Ȯ��
	FString currentLevelName = GetWorld()->GetMapName();
	currentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // ���ξ� ����

	auto gameInstance = Cast<UDodgeballGameInstance>(GetGameInstance());
	
		
	if (!gameInstance->bSkipControlGuide)
	{
		if (currentLevelName == "InGameLevel" || currentLevelName == "ThirdPersonMap" || currentLevelName == "Demo_SKS")// ���߿� InGameLevel�� ���� ����
		{
			ShowControlGuide();
		}
	}
	else
	{
		OnControlGuideClosed();
	}
}


void AInGameGameMode::OnTimeOut()
{
	if (!isTimeOut)	isTimeOut = true;

	if (teamAScore > teamBScore)
	{
		ProceedSet(ECharacterTeamType::A);
	}
	else if (teamBScore > teamAScore)
	{
		ProceedSet(ECharacterTeamType::B);
	}
}

void AInGameGameMode::ReviveCharacter(ECharacterTeamType getInTeamType)
{
	/*auto charac = Cast<ADodgeballCharacter>(_character);
	if (!charac) return;*/
	if (getInTeamType == ECharacterTeamType::A && teamBScore <= 0) return;
	if(getInTeamType == ECharacterTeamType::B && teamAScore <= 0) return;

	aIManager->OnCharacterIn(getInTeamType);
	if (getInTeamType == ECharacterTeamType::A)
	{
		SubtractScore(ECharacterTeamType::B, 1);
	}
	else
	{
		SubtractScore(ECharacterTeamType::A, 1);
	}
}

void AInGameGameMode::OutCharacter(AActor* _character)
{
	auto charac = Cast<ADodgeballCharacter>(_character);
	if (!charac) return;

	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	//�÷��̾��� ��쿣 ī�޶� �̵� ����
	if (player == charac)
	{
		if (gameState->currentAliveTeamACharacterCount == 1)
		{
			//��Ʈ ����
			gameState->currentAliveTeamACharacterCount--;
			AddScore(ECharacterTeamType::B, 1);

			UE_LOG(LogTemp, Warning, TEXT("A team lose!!. alive count : %f, bTeamSet : %d"), gameState->currentAliveTeamACharacterCount,teamBSetWin);

			return;
		}
		else
		{
			auto& characters = *(gameState->charactersA);
			ADodgeballCharacter* proximateCharac=nullptr;
			float proximateDis = 100000;
			for (int i = 0; i < characters.Num(); i++)
			{
				if (!characters[i]) continue;
				if (characters[i] == player) continue;
				if (Cast<ADodgeBall>(gameState->ball)->hitCharacters.Contains(characters[i])) continue;

				auto dis = FVector::Dist2D(characters[i]->GetActorLocation(), player->GetActorLocation());
				if (dis < proximateDis)
				{
					proximateDis = dis;
					proximateCharac = characters[i];
				}
			}

			//���� �� �ƿ��� ����� �� ����.
			if (!proximateCharac)
			{
				gameState->currentAliveTeamACharacterCount--;
				AddScore(ECharacterTeamType::B, 1);
				return;
			}

			
			proximateCharac->InitLook();
			float headGoalRotation = proximateCharac->currentRotation.X;
			auto cameraEndTrs = proximateCharac->GetMesh()->GetSocketTransform(TEXT("HeadSocket"));
			auto endLocation = cameraEndTrs.TransformPosition(FVector(4, 0, 10));
			auto endRotation = cameraEndTrs.Rotator();
			auto endCameraActor = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), endLocation, endRotation);

			auto cameraStartTrs = player->GetMesh()->GetSocketTransform(TEXT("HeadSocket"));
			auto startLocation = cameraStartTrs.TransformPosition(FVector(4, 0, 10));
			auto startRotation = cameraStartTrs.Rotator();
			auto startCameraActor= GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), startLocation, startRotation);

			if (player) Cast<ADodgeballPlayerController>(player->GetController())->TurnCharacter(proximateCharac);
			aIManager->UpdateTeamAAIFloorNum(GetWorld()->DeltaTimeSeconds);
			aIManager->UpdateTeamBAIFloorNum(GetWorld()->DeltaTimeSeconds);
			if (charac) aIManager->OnCharacterOut(charac);
			player = proximateCharac;

			Cast<ADodgeballPlayerController>(player->GetController())->SetViewTargetWithBlend(startCameraActor, 0, EViewTargetBlendFunction::VTBlend_Cubic);
			
			Cast<ADodgeballPlayerController>(player->GetController())->SetViewTargetWithBlend(endCameraActor, 0.1f, EViewTargetBlendFunction::VTBlend_Cubic);

			UGameplayStatics::SetGlobalTimeDilation(GetWorld(),0.1f);
			
			GetWorld()->GetTimerManager().SetTimer(ResumeHandle, [this, headGoalRotation, player, endCameraActor, startCameraActor]() {

				if (startCameraActor) startCameraActor->Destroy();
				if (endCameraActor) endCameraActor->Destroy();

				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
				//if(player) Cast<ADodgeballPlayerController>(player->GetController())->TurnCharacter(proximateCharac);

				while (FMath::Abs(player->currentRotation.X - headGoalRotation) > 5) player->Look(FVector2D(0, -(player->currentRotation.X - headGoalRotation)));
				}, 0.11f, false);
			AddScore(ECharacterTeamType::B, 1);
		}
	}
	else
	{
		aIManager->OnCharacterOut(charac);

		if (charac->teamType == ECharacterTeamType::A)
		{
			AddScore(ECharacterTeamType::B, 1);
		}
		else
		{
			AddScore(ECharacterTeamType::A, 1);
		}
	}
}

//escŰ ���
void AInGameGameMode::HandlePauseMenu()
{
	if (inGameUIComponent)
	{
		inGameUIComponent->OnPauseToggle();
	}
}

void AInGameGameMode::InitGameUI()
{
	if (!inGameUIComponent)
	{
		inGameUIComponent = FindComponentByClass<UInGameUIComponent>();
		if (!inGameUIComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("[UI] UInGameUIComponent�� �پ����� �ʽ��ϴ�!"));
			return;
		}
	}

	if (inGameUIComponent)
	{
		inGameUIComponent->CreateInGameLevelWidget();
		inGameUIComponent->UpdateScoreText(teamAScore, teamBScore);
		inGameUIComponent->UpdateSetScore(ECharacterTeamType::A, teamASetWin);
		inGameUIComponent->UpdateSetScore(ECharacterTeamType::B, teamBSetWin);
		inGameUIComponent->StartGameTimer();
	}
}

void AInGameGameMode::EndGame()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	if (gameState->recentWinnerTeam == ECharacterTeamType::A)
	{
		for (auto& charac : *gameState->charactersA)
		{
			if (charac && IsValid(charac)) charac->OnGameEnded(true);
		}
		for (auto& charac : *gameState->charactersB)
		{
			if (charac && IsValid(charac)) charac->OnGameEnded(false);
		}
	}
	else
	{
		for (auto& charac : *gameState->charactersA)
		{
			if (charac && IsValid(charac)) charac->OnGameEnded(false);
		}
		for (auto& charac : *gameState->charactersB)
		{
			if (charac && IsValid(charac)) charac->OnGameEnded(true);
		}
	}

	TWeakObjectPtr<AInGameGameMode> thisWeak(this);
	GetWorld()->GetTimerManager().SetTimer(timerHandleEndGame, [thisWeak, gameState]() 
		{ 
			if (!thisWeak.IsValid()) return;
			thisWeak->inGameUIComponent->ShowEndSetWidget(gameState->recentWinnerTeam);
		}, 3, false);
}

void AInGameGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(timerHandleFade);
	GetWorld()->GetTimerManager().ClearTimer(ResumeHandle);
	GetWorld()->GetTimerManager().ClearTimer(timerHandleEndGame);
	GetWorld()->GetTimerManager().ClearTimer(firstSetHandleOnFadeInEnded);
	GetWorld()->GetTimerManager().ClearTimer(handleOnCinematicEnded);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}
