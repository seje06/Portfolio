#include "InGameLevel/InGameGameMode/PauseWidget.h"
#include "InGameLevel/InGameGameMode/InGameUIComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


void UPauseWidget::SetUIComponent(UInGameUIComponent* _uiComponent)
{
	uiComponent = _uiComponent;
	SetKeyboardFocus();
}

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (resumeButton)
		resumeButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleResumeClicked);

	if (settingsButton)
		settingsButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleSettingsClicked);

	if (returnStartMenuButton)
		returnStartMenuButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleReturnStartMenuClicked);

	if (quitButton)
		quitButton->OnClicked.AddDynamic(this, &UPauseWidget::HandleQuitClicked);
}

void UPauseWidget::HandleResumeClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		pc->SetInputMode(FInputModeGameOnly());
		pc->bShowMouseCursor = false;
	}

	RemoveFromParent();
}

void UPauseWidget::HandleSettingsClicked()
{
	if (uiComponent)
	{
		uiComponent->ShowSettingsWidget();
	}
}

void UPauseWidget::HandleReturnStartMenuClicked()
{
	if (uiComponent)
	{
		uiComponent->ShowReturnConfirmWidget();
	}
}

void UPauseWidget::HandleQuitClicked()
{
	if (uiComponent)
	{
		uiComponent->ShowExitConfirmWidget();
	}
}

FReply UPauseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!uiComponent) return FReply::Handled();
	if (InKeyEvent.GetKey() == EKeys::Escape|| InKeyEvent.GetKey() == EKeys::P)
	{
		uiComponent->OnPauseToggle();

		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
