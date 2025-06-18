#include "InGameLevel/InGameGameMode/InGameUIComponent.h"
#include "InGameLevel/UI/FadeWidget.h"
#include "InGameLevel/InGameGameMode/PauseWidget.h"
#include "InGameLevel/InGameGameMOde/EndSetWidget.h"
#include "InGameLevel/InGameGameMode/TeamShotClockWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"



UInGameUIComponent::UInGameUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInGameUIComponent::FadeOut(float _fadeDuration)
{
	if (fadeWidgetInstance) fadeWidgetInstance->RemoveFromParent();
	fadeWidgetInstance = nullptr;
	// FadeWidget 생성
	if (fadeWidgetClass)
	{
		fadeWidgetInstance = CreateWidget<UFadeWidget>(GetWorld(), fadeWidgetClass);
		if (fadeWidgetInstance)
		{
			fadeWidgetInstance->AddToViewport(100); // ZOrder 100으로 항상 제일 위에
		}
	}

	// 페이드아웃 시작
	if (fadeWidgetInstance && !fadeWidgetInstance->IsFading())
	{
		fadeWidgetInstance->StartFadeOut(_fadeDuration); // 1초 동안 페이드아웃
		timerHandle_fade;
		GetWorld()->GetTimerManager().SetTimer(timerHandle_fade, fadeWidgetInstance, &UWidget::RemoveFromParent, _fadeDuration, false);
		// 1초 후에 레벨 이동
	}

}

void UInGameUIComponent::FadeIn(float _fadeDuration)
{
	if (IsValid(fadeWidgetInstance)) fadeWidgetInstance->RemoveFromParent();
	fadeWidgetInstance = nullptr;
	// FadeWidget 생성
	if (fadeWidgetClass)
	{
		fadeWidgetInstance = CreateWidget<UFadeWidget>(GetWorld(), fadeWidgetClass);
		if (fadeWidgetInstance)
		{
			fadeWidgetInstance->AddToViewport(100); // ZOrder 100으로 항상 제일 위에
		}
	}

	if (fadeWidgetInstance)
	{
		fadeWidgetInstance->StartFadeIn(_fadeDuration);
		timerHandle_fade;
		GetWorld()->GetTimerManager().SetTimer(timerHandle_fade, fadeWidgetInstance, &UWidget::RemoveFromParent, _fadeDuration, false);
	}
}

void UInGameUIComponent::UpdateShotClock(ECharacterTeamType _teamType, int32 _remainingTime)
{
	if (!teamShotClockWidgetClass) return;

	if (!teamShotClockWidget)
	{
		teamShotClockWidget = CreateWidget<UTeamShotClockWidget>(GetWorld(), teamShotClockWidgetClass);
		teamShotClockWidget->AddToViewport();
	}

	if (_teamType == ECharacterTeamType::A && teamShotClockWidget)
	{
		teamShotClockWidget->teamBShotClockText->SetVisibility(ESlateVisibility::Hidden);
		teamShotClockWidget->teamAShotClockText->SetVisibility(ESlateVisibility::Visible);
		teamShotClockWidget->teamAShotClockText->SetText(FText::AsNumber(_remainingTime));
	}
	else if (_teamType == ECharacterTeamType::B && teamShotClockWidget)
	{
		teamShotClockWidget->teamAShotClockText->SetVisibility(ESlateVisibility::Hidden);
		teamShotClockWidget->teamBShotClockText->SetVisibility(ESlateVisibility::Visible);
		teamShotClockWidget->teamBShotClockText->SetText(FText::AsNumber(_remainingTime));
	}
}

void UInGameUIComponent::ShowEndSetWidget(ECharacterTeamType _winnerTeam)
{
	if (!endSetWidgetClass) return;

	if (!endSetWidget)
	{
		endSetWidget = CreateWidget<UEndSetWidget>(GetWorld(), endSetWidgetClass);
		endSetWidget->SetUIComponent(this);
	}

	if (endSetWidget && !endSetWidget->IsInViewport())
	{
		endSetWidget->AddToViewport();
		endSetWidget->SetResultByWinner(_winnerTeam);

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			/*pc->SetShowMouseCursor(true);
			pc->SetInputMode(FInputModeUIOnly());*/
		}
	}
}

void UInGameUIComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UInGameUIComponent::CreateInGameLevelWidget()
{
	APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!pc) return;

	//헤더 디스플레이 위젯 생성 및 추가
	if (headerDisplayWidgetClass)
	{
		headerDisplayWidget = CreateWidget<UUserWidget>(pc, headerDisplayWidgetClass);
		if (headerDisplayWidget)
		{
			headerDisplayWidget->AddToViewport();

			//내부 위젯 요소 바인딩
			timerText = Cast<UTextBlock>(headerDisplayWidget->GetWidgetFromName(TEXT("timerText")));
			teamAScoreText = Cast<UTextBlock>(headerDisplayWidget->GetWidgetFromName(TEXT("teamAScoreText")));
			teamBScoreText = Cast<UTextBlock>(headerDisplayWidget->GetWidgetFromName(TEXT("TeamBScoreText")));
			teamASetBox = Cast<UHorizontalBox>(headerDisplayWidget->GetWidgetFromName(TEXT("TeamASetBox")));
			teamBSetBox = Cast<UHorizontalBox>(headerDisplayWidget->GetWidgetFromName(TEXT("TeamBSetBox")));
		}
	}
}

void UInGameUIComponent::UpdateScoreText(int32 _teamAScore, int32 _teamBScore)
{
	if (teamAScoreText)
	{
		teamAScoreText->SetText(FText::AsNumber(_teamAScore));
	}

	if (teamBScoreText)
	{
		teamBScoreText->SetText(FText::AsNumber(_teamBScore));
	}
}

void UInGameUIComponent::UpdateSetScore(ECharacterTeamType _teamType, int32 _winSetCount)
{
	UHorizontalBox* targetBox = nullptr;

	if (_teamType == ECharacterTeamType::A)
	{
		targetBox = teamASetBox;
	}
	else
	{
		targetBox = teamBSetBox;
	}
	if (!targetBox) return;

	int32 total = targetBox->GetChildrenCount();

	for (int32 i = 0; i < total; i++)
	{
		int32 targetIndex = 0;

		// A팀은 오른쪽부터 채우기 (오른쪽이 글자에 가까움)
		if (_teamType == ECharacterTeamType::A)
		{
			targetIndex = total - 1 - i;
		}
		// B팀은 왼쪽부터 채우기 (왼쪽이 글자에 가까움)
		else
		{
			targetIndex = i;
		}

		if (UImage* icon = Cast<UImage>(targetBox->GetChildAt(targetIndex)))
		{
			if (i < _winSetCount)
			{
				icon->SetBrushFromTexture(activeSetIcon); // 꽉찬 원
			}
			else
			{
				icon->SetBrushFromTexture(inactiveSetIcon); // 빈 원
			}
		}
	}
}

void UInGameUIComponent::StartGameTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	currentTime = maxTime;
	// 1초마다 UpdateTimer 호출
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UInGameUIComponent::UpdateTimer, 1.0f, true);

	// 바로 첫 타이밍 표시
	UpdateTimer();
}

void UInGameUIComponent::UpdateTimer()
{
	// 먼저 시간 표시
	int32 minutes = currentTime / 60;
	int32 seconds = currentTime % 60;

	if (timerText)
	{
		FString timeString = FString::Printf(TEXT("%d:%02d"), minutes, seconds);
		timerText->SetText(FText::FromString(timeString));
	}

	// 그 다음 1초 감소
	currentTime--;

	// 0초 이하 되면 타이머 종료
	if (currentTime < 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);

		// [여기에] 타이머 종료 시 처리할 내용 작성!
		UE_LOG(LogTemp, Warning, TEXT("Game Timer Ended"));
		currentTime = maxTime;
		OnUITimeOut.ExecuteIfBound();
		// 예: GameMode->EndSet() 같은 함수 호출
	}
}

void UInGameUIComponent::UpdateSkillEnergyBar(int32 _currentEnergy)
{
	if (!skillEnergyBar) return;

	// 에너지 값은 0~3 범위
	float percent = FMath::Clamp(static_cast<float>(_currentEnergy) / 3.0f, 0.0f, 1.0f);
	skillEnergyBar->SetPercent(percent);
}

//일시정지창 출력
void UInGameUIComponent::ShowPauseWidget()
{
	if (!pauseWidgetClass) return;

	if (!pauseWidget)
	{
		pauseWidget = CreateWidget<UUserWidget>(GetWorld(), pauseWidgetClass);
		Cast<UPauseWidget>(pauseWidget)->SetUIComponent(this);
	}

	if (pauseWidget && !pauseWidget->IsInViewport())
	{
		pauseWidget->AddToViewport();
		Cast<UPauseWidget>(pauseWidget)->SetUIComponent(this);
		// 일시정지 처리
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		// 마우스 및 입력 모드 설정
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;

		}
		pauseWidget->bIsFocusable = true;
		//FSlateApplication::Get().SetKeyboardFocus(pauseWidget->TakeWidget(), EFocusCause::SetDirectly);

	}
}

void UInGameUIComponent::OnPauseToggle()
{
	if (fadeWidgetInstance->IsFading())
	{
		UE_LOG(LogTemp, Warning, TEXT("current Fading"));
		//return;
	}
	UE_LOG(LogTemp, Warning, TEXT("pauseWidget closed"));

	// 설정창이 열려있으면 닫기
	if (settingsWidget && settingsWidget->IsInViewport())
	{
		settingsWidget->RemoveFromParent();
		settingsWidget = nullptr;
		return;
	}

	// 종료확인창이 열려있으면 닫기
	if (exitConfirmWidget && exitConfirmWidget->IsInViewport())
	{
		exitConfirmWidget->RemoveFromParent();
		exitConfirmWidget = nullptr;
		return;
	}

	// 시작메뉴확인창이 열려있으면 닫기
	if (returnConfirmWidget && returnConfirmWidget->IsInViewport())
	{
		returnConfirmWidget->RemoveFromParent();
		returnConfirmWidget = nullptr;
		return;
	}

	// 일시정지창이 열려있으면 닫고 게임 계속
	if (pauseWidget && pauseWidget->IsInViewport())
	{
		pauseWidget->RemoveFromParent();
		pauseWidget = nullptr;

		UGameplayStatics::SetGamePaused(GetWorld(), false);

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameOnly inputMode;
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = false;
		}
		return;
	}

	// 아무것도 없으면 일시정지 위젯 생성
	ShowPauseWidget();
}

//설정창 출력
void UInGameUIComponent::ShowSettingsWidget()
{
	if (!settingsWidgetClass) return;

	// 이미 생성되어 있다면 재사용
	if (!settingsWidget)
	{
		settingsWidget = CreateWidget<UUserWidget>(GetWorld(), settingsWidgetClass);
	}

	if (settingsWidget && !settingsWidget->IsInViewport())
	{
		settingsWidget->AddToViewport();

		// UI 모드로 전환 + 마우스 표시
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;
		}
	}
}

//게임종료창 출력
void UInGameUIComponent::ShowExitConfirmWidget()
{
	if (!exitConfirmWidgetClass) return;

	if (!exitConfirmWidget)
	{
		exitConfirmWidget = CreateWidget<UUserWidget>(GetWorld(), exitConfirmWidgetClass);
	}

	if (exitConfirmWidget && !exitConfirmWidget->IsInViewport())
	{
		exitConfirmWidget->AddToViewport();

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;
		}
	}
}

//시작메뉴창 출력
void UInGameUIComponent::ShowReturnConfirmWidget()
{
	if (!returnConfirmWidgetClass) return;

	if (!returnConfirmWidget)
	{
		returnConfirmWidget = CreateWidget<UUserWidget>(GetWorld(), returnConfirmWidgetClass);
	}

	if (returnConfirmWidget && !returnConfirmWidget->IsInViewport())
	{
		returnConfirmWidget->AddToViewport();

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;
		}
	}
}

void UInGameUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	GetWorld()->GetTimerManager().ClearTimer(timerHandle_fade);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}


