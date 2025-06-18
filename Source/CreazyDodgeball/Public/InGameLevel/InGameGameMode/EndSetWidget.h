#pragma once

#include "CoreMinimal.h"
#include "InGameLevel/Characters/CharacterEnums.h"
#include "Blueprint/UserWidget.h"
#include "EndSetWidget.generated.h"

class UButton;
class UInGameUIComponent;
class UTextBlock;


UCLASS()
class CREAZYDODGEBALL_API UEndSetWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetResultByWinner(ECharacterTeamType _winnerTeam);
	void SetUIComponent(class UInGameUIComponent* _uiComponent);
protected:
	virtual void NativeConstruct() override;

private:

	UFUNCTION()
	void OnRestartGameClicked();

	UFUNCTION()
	void HandleReturnToMenuClicked();

	UFUNCTION()
	void HandleQuitGameClicked();
	
	UPROPERTY(meta = (BindWidget))
	UButton* restartGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* returnToStartMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* quitGameButton;

	// WIN 또는 LOSE
	UPROPERTY(meta = (BindWidget))
	UTextBlock* resultText;

	// 연결된 UI컴포넌트
	UPROPERTY()
	UInGameUIComponent* uiComponent;

};
