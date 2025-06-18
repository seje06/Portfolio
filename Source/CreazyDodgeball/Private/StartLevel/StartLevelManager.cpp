#include "StartLevel/StartLevelManager.h"
#include "StartLevel/StartMenuWidget.h"
#include "StartLevel/SettingsWidget.h"
#include "InGameLevel/UI/FadeWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "DodgeballGameInstance.h"



AStartLevelManager::AStartLevelManager()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AStartLevelManager::BeginPlay()
{
	Super::BeginPlay();
	
	auto gameInstance = Cast<UDodgeballGameInstance>(GetGameInstance());
	if (gameInstance) gameInstance->bSkipControlGuide = false;


	PlayOpeningCinematic();

	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (!pc)
	{
		UE_LOG(LogTemp, Warning, TEXT("[INPUT] PlayerController is null"));
		return;
	}
	
	EnableInput(pc);

	// 1. Enhanced Input Subsystem에 Mapping Context 추가
	if (ULocalPlayer* localPlayer = pc->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* inputSubsystem =
			localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (inputSubsystem && inputMappingContext)
		{
			inputSubsystem->AddMappingContext(inputMappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("[INPUT] Mapping Context added."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[INPUT] Failed to add Mapping Context"));
		}
	}
	// 2. ESC 키 액션 바인딩
	if (UEnhancedInputComponent* enhancedInput = Cast<UEnhancedInputComponent>(pc->InputComponent))
	{
		if (inputActionToggleSettings)
		{
			enhancedInput->BindAction(inputActionToggleSettings, ETriggerEvent::Started, this, &AStartLevelManager::OnToggleSettings);
			UE_LOG(LogTemp, Warning, TEXT("[INPUT] Action Bind SUCCESS"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[INPUT] inputActionToggleSettings is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[INPUT] Failed to cast to EnhancedInputComponent"));
	}

}

void AStartLevelManager::PlayOpeningCinematic()
{
	if (!openingSequence)
	{
		//시네마틱 없으면 바로 시작메뉴
		ShowStartMenu();
		return;
	}

	FMovieSceneSequencePlaybackSettings playbackSettings;
	playbackSettings.bAutoPlay = true;

	ALevelSequenceActor* outActor = nullptr;
	sequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), openingSequence, playbackSettings, outActor);

	if (sequencePlayer)
	{
		sequencePlayer->Play();

		//시네마틱 재생 시간만큼 대기 후 메뉴 표시
		GetWorld()->GetTimerManager().SetTimer(delayHandle, this, &AStartLevelManager::ShowStartMenu, sequencePlayer->GetDuration().AsSeconds(), false);
	}
	else
	{
		ShowStartMenu();
	}

}

void AStartLevelManager::ShowStartMenu()
{
	if (!startMenuWidgetClass) return;

	startMenuWidget = CreateWidget<UUserWidget>(GetWorld(), startMenuWidgetClass);
	if (startMenuWidget)
	{
		startMenuWidget->AddToViewport();

		//매니저 연결
		if (UStartMenuWidget* menu = Cast<UStartMenuWidget>(startMenuWidget))
		{
			menu->SetManager(this);
		}

		//UI 입력 활성화, 키보드입력도 같이
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetWidgetToFocus(startMenuWidget->TakeWidget());
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;
		}
	}
	
}

void AStartLevelManager::ShowSettingsWidget()
{
	if (!settingsWidgetClass) return;

	settingsWidget = CreateWidget<UUserWidget>(GetWorld(), settingsWidgetClass);
	if (settingsWidget)
	{
		settingsWidget->AddToViewport();

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetWidgetToFocus(settingsWidget->TakeWidget());
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;
		}
	}

}

void AStartLevelManager::HideSettingsWidget()
{
	
	if (settingsWidget && settingsWidget->IsInViewport())
	{
		settingsWidget->RemoveFromParent();
		settingsWidget = nullptr;

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

void AStartLevelManager::ShowExitConfirmWidget()
{
	if (!exitConfirmWidgetClass) return;

	exitConfirmWidget = CreateWidget<UUserWidget>(GetWorld(), exitConfirmWidgetClass);
	if (exitConfirmWidget)
	{
		exitConfirmWidget->AddToViewport();

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FInputModeGameAndUI inputMode;
			inputMode.SetWidgetToFocus(exitConfirmWidget->TakeWidget());
			inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			pc->SetInputMode(inputMode);
			pc->bShowMouseCursor = true;
		}
	}
}

void AStartLevelManager::HideExitConfirmWidget()
{
	if (exitConfirmWidget && exitConfirmWidget->IsInViewport())
	{
		exitConfirmWidget->RemoveFromParent();
		exitConfirmWidget = nullptr;

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


void AStartLevelManager::OnStartGame()
{	
	//ESC 바인딩 해제
	DisableEscInput();

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
		fadeWidgetInstance->StartFadeOut(1.0f); // 1초 동안 페이드아웃

		// 로딩창 띄우기 (페이드아웃 도중 보여짐)
		if (!loadingWidgetInstance && loadingWidgetClass)
		{
			loadingWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), loadingWidgetClass);
			if (loadingWidgetInstance)
			{
				loadingWidgetInstance->AddToViewport(101); // Fade보다 아래 ZOrder로
			}
		}

		// 1초 후에 레벨 이동
		GetWorld()->GetTimerManager().SetTimer(fadeOutTimerHandle, this, &AStartLevelManager::MoveToInGameLevel, 1.0f, false);
	}
	else
	{
		// 예외 처리: FadeWidget이 없으면 그냥 레벨 이동
		MoveToInGameLevel();
	}
}

void AStartLevelManager::OnQuitGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[Quit] Quit Game button pressed"));

	if (!exitConfirmWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Quit] exitConfirmWidgetClass is null"));
		return;
	}

	if (exitConfirmWidget && exitConfirmWidget->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Quit] ExitConfirmWidget already open"));
		return;
	}

	ShowExitConfirmWidget(); // 정상적으로 호출되게 확인
}

void AStartLevelManager::OnOpenSettings()
{
	ShowSettingsWidget();
}

void AStartLevelManager::OnToggleSettings(const FInputActionInstance& Instance)
{

	// 설정창을 닫기
	if (settingsWidget && settingsWidget->IsInViewport())
	{
		HideSettingsWidget();
		return; // 다른 위젯 처리 막기 위해 return
	}
	
	// 게임종료창을 닫기
	if (exitConfirmWidget && exitConfirmWidget->IsInViewport())
	{
		HideExitConfirmWidget();
		return; // 다른 위젯 처리 막기 위해 return
	}
	
	// 설정창/종료창 모두 없고, StartMenu만 있을 때 종료 확인창 표시
	if (!settingsWidget || !settingsWidget->IsInViewport())
	{
		if (!exitConfirmWidget || !exitConfirmWidget->IsInViewport())
		{
			if (startMenuWidget && startMenuWidget->IsInViewport())
			{
				ShowExitConfirmWidget();
			}
		}
	}
}

void AStartLevelManager::DisableEscInput()
{
	if (InputComponent)
	{
		InputComponent->ClearActionBindings();
		InputComponent = nullptr;
	}

	DisableInput(GetWorld()->GetFirstPlayerController());
}

void AStartLevelManager::MoveToInGameLevel()
{
	UGameplayStatics::OpenLevel(this, FName("Demo_SKS"));
}

void AStartLevelManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(fadeOutTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(delayHandle);
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}