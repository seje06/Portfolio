#include "StartLevel/ExitConfirmWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UExitConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (yesButton)
	{
		yesButton->OnClicked.AddDynamic(this, &UExitConfirmWidget::OnYesClicked);
	}

	if (noButton)
	{
		noButton->OnClicked.AddDynamic(this, &UExitConfirmWidget::OnNoClicked);
	}
}

void UExitConfirmWidget::OnYesClicked()
{
	// 게임 종료
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);

}

void UExitConfirmWidget::OnNoClicked()
{
	// 위젯 제거
	RemoveFromParent();

	// ESC 닫기와 동일하게 마우스 커서 유지
	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameAndUI inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		pc->SetInputMode(inputMode);
		pc->bShowMouseCursor = true;
	}
}
