#include "InGameLevel/InGameGameMode/EndSetWidget.h"
#include "InGameLevel/InGameGameMode/InGameUIComponent.h"
#include "InGameLevel/Characters/CharacterEnums.h"
#include "DodgeballGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"



void UEndSetWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (restartGameButton)
		restartGameButton->OnClicked.AddDynamic(this, &UEndSetWidget::OnRestartGameClicked);

	if (returnToStartMenuButton)
		returnToStartMenuButton->OnClicked.AddDynamic(this, &UEndSetWidget::HandleReturnToMenuClicked);

	if (quitGameButton)
		quitGameButton->OnClicked.AddDynamic(this, &UEndSetWidget::HandleQuitGameClicked);
}

void UEndSetWidget::OnRestartGameClicked()
{
	if (auto gameInstance = Cast<UDodgeballGameInstance>(GetGameInstance()))
	{
		gameInstance->bSkipControlGuide = true;
	}
	UGameplayStatics::OpenLevel(this, FName("Demo_SKS"));
}

void UEndSetWidget::HandleReturnToMenuClicked()
{
	if (uiComponent)
	{
		uiComponent->ShowReturnConfirmWidget();
	}
}

void UEndSetWidget::HandleQuitGameClicked()
{
	if (uiComponent)
	{
		uiComponent->ShowExitConfirmWidget();
	}
}

void UEndSetWidget::SetResultByWinner(ECharacterTeamType _winnerTeam)
{
	if (!resultText) return;

	switch (_winnerTeam)
	{
	case ECharacterTeamType::A:
		resultText->SetText(FText::FromString(TEXT("WIN")));
		break;

	case ECharacterTeamType::B:
		resultText->SetText(FText::FromString(TEXT("LOSE")));
		break;

	default:
		resultText->SetText(FText::FromString(TEXT("?")));
		break;
	}
}

void UEndSetWidget::SetUIComponent(UInGameUIComponent* _uiComponent)
{
	uiComponent = _uiComponent;
}
