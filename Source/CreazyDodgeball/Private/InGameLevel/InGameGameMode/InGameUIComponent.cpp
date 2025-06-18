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
	// FadeWidget ����
	if (fadeWidgetClass)
	{
		fadeWidgetInstance = CreateWidget<UFadeWidget>(GetWorld(), fadeWidgetClass);
		if (fadeWidgetInstance)
		{
			fadeWidgetInstance->AddToViewport(100); // ZOrder 100���� �׻� ���� ����
		}
	}

	// ���̵�ƿ� ����
	if (fadeWidgetInstance && !fadeWidgetInstance->IsFading())
	{
		fadeWidgetInstance->StartFadeOut(_fadeDuration); // 1�� ���� ���̵�ƿ�
		timerHandle_fade;
		GetWorld()->GetTimerManager().SetTimer(timerHandle_fade, fadeWidgetInstance, &UWidget::RemoveFromParent, _fadeDuration, false);
		// 1�� �Ŀ� ���� �̵�
	}

}

void UInGameUIComponent::FadeIn(float _fadeDuration)
{
	if (IsValid(fadeWidgetInstance)) fadeWidgetInstance->RemoveFromParent();
	fadeWidgetInstance = nullptr;
	// FadeWidget ����
	if (fadeWidgetClass)
	{
		fadeWidgetInstance = CreateWidget<UFadeWidget>(GetWorld(), fadeWidgetClass);
		if (fadeWidgetInstance)
		{
			fadeWidgetInstance->AddToViewport(100); // ZOrder 100���� �׻� ���� ����
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

	//��� ���÷��� ���� ���� �� �߰�
	if (headerDisplayWidgetClass)
	{
		headerDisplayWidget = CreateWidget<UUserWidget>(pc, headerDisplayWidgetClass);
		if (headerDisplayWidget)
		{
			headerDisplayWidget->AddToViewport();

			//���� ���� ��� ���ε�
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

		// A���� �����ʺ��� ä��� (�������� ���ڿ� �����)
		if (_teamType == ECharacterTeamType::A)
		{
			targetIndex = total - 1 - i;
		}
		// B���� ���ʺ��� ä��� (������ ���ڿ� �����)
		else
		{
			targetIndex = i;
		}

		if (UImage* icon = Cast<UImage>(targetBox->GetChildAt(targetIndex)))
		{
			if (i < _winSetCount)
			{
				icon->SetBrushFromTexture(activeSetIcon); // ���� ��
			}
			else
			{
				icon->SetBrushFromTexture(inactiveSetIcon); // �� ��
			}
		}
	}
}

void UInGameUIComponent::StartGameTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	currentTime = maxTime;
	// 1�ʸ��� UpdateTimer ȣ��
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UInGameUIComponent::UpdateTimer, 1.0f, true);

	// �ٷ� ù Ÿ�̹� ǥ��
	UpdateTimer();
}

void UInGameUIComponent::UpdateTimer()
{
	// ���� �ð� ǥ��
	int32 minutes = currentTime / 60;
	int32 seconds = currentTime % 60;

	if (timerText)
	{
		FString timeString = FString::Printf(TEXT("%d:%02d"), minutes, seconds);
		timerText->SetText(FText::FromString(timeString));
	}

	// �� ���� 1�� ����
	currentTime--;

	// 0�� ���� �Ǹ� Ÿ�̸� ����
	if (currentTime < 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);

		// [���⿡] Ÿ�̸� ���� �� ó���� ���� �ۼ�!
		UE_LOG(LogTemp, Warning, TEXT("Game Timer Ended"));
		currentTime = maxTime;
		OnUITimeOut.ExecuteIfBound();
		// ��: GameMode->EndSet() ���� �Լ� ȣ��
	}
}

void UInGameUIComponent::UpdateSkillEnergyBar(int32 _currentEnergy)
{
	if (!skillEnergyBar) return;

	// ������ ���� 0~3 ����
	float percent = FMath::Clamp(static_cast<float>(_currentEnergy) / 3.0f, 0.0f, 1.0f);
	skillEnergyBar->SetPercent(percent);
}

//�Ͻ�����â ���
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
		// �Ͻ����� ó��
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		// ���콺 �� �Է� ��� ����
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

	// ����â�� ���������� �ݱ�
	if (settingsWidget && settingsWidget->IsInViewport())
	{
		settingsWidget->RemoveFromParent();
		settingsWidget = nullptr;
		return;
	}

	// ����Ȯ��â�� ���������� �ݱ�
	if (exitConfirmWidget && exitConfirmWidget->IsInViewport())
	{
		exitConfirmWidget->RemoveFromParent();
		exitConfirmWidget = nullptr;
		return;
	}

	// ���۸޴�Ȯ��â�� ���������� �ݱ�
	if (returnConfirmWidget && returnConfirmWidget->IsInViewport())
	{
		returnConfirmWidget->RemoveFromParent();
		returnConfirmWidget = nullptr;
		return;
	}

	// �Ͻ�����â�� ���������� �ݰ� ���� ���
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

	// �ƹ��͵� ������ �Ͻ����� ���� ����
	ShowPauseWidget();
}

//����â ���
void UInGameUIComponent::ShowSettingsWidget()
{
	if (!settingsWidgetClass) return;

	// �̹� �����Ǿ� �ִٸ� ����
	if (!settingsWidget)
	{
		settingsWidget = CreateWidget<UUserWidget>(GetWorld(), settingsWidgetClass);
	}

	if (settingsWidget && !settingsWidget->IsInViewport())
	{
		settingsWidget->AddToViewport();

		// UI ���� ��ȯ + ���콺 ǥ��
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

//��������â ���
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

//���۸޴�â ���
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


