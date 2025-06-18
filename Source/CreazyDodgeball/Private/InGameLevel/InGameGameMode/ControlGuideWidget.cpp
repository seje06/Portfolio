#include "InGameLevel/InGameGameMode/ControlGuideWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "InGameLevel/InGameGameMode/InGameGameMode.h"

void UControlGuideWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//버튼 바인딩
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UControlGuideWidget::HandleCloseButtonClicked);
	}
}

void UControlGuideWidget::HandleCloseButtonClicked()
{
	if (AInGameGameMode* GameMode = Cast<AInGameGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnControlGuideClosed();
	}
}
