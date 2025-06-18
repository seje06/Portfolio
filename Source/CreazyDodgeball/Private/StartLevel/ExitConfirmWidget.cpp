#include "StartLevel/ExitConfirmWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void UExitConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ��ư ���ε�
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
	// ���� ����
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);

}

void UExitConfirmWidget::OnNoClicked()
{
	// ���� ����
	RemoveFromParent();

	// ESC �ݱ�� �����ϰ� ���콺 Ŀ�� ����
	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameAndUI inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		pc->SetInputMode(inputMode);
		pc->bShowMouseCursor = true;
	}
}
