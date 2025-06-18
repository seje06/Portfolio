#include "InGameLevel/InGameGameMode/ReturnConfirmWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


void UReturnConfirmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (yesButton)
	{
		yesButton->OnClicked.AddDynamic(this, &UReturnConfirmWidget::OnYesClicked);
	}

	if (noButton)
	{
		noButton->OnClicked.AddDynamic(this, &UReturnConfirmWidget::OnNoClicked);
	}
}

void UReturnConfirmWidget::OnYesClicked()
{
	UGameplayStatics::OpenLevel(this, FName("StartLevel"));
}

void UReturnConfirmWidget::OnNoClicked()
{
	RemoveFromParent();

	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameAndUI inputMode;
		pc->SetInputMode(inputMode);
		pc->bShowMouseCursor = true;
	}
}
