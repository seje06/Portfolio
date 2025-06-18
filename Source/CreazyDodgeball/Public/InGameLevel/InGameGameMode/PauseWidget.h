#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"

class UButton;
class UInGameUIComponent;

UCLASS()
class CREAZYDODGEBALL_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// UIComponent 설정 함수
	void SetUIComponent(UInGameUIComponent* _uiComponent);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	virtual void NativeConstruct() override;

private:
	// 클릭 처리 함수
	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleReturnStartMenuClicked();

	UFUNCTION()
	void HandleQuitClicked();
	
	// 버튼들
	UPROPERTY(meta = (BindWidget))
	UButton* resumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* settingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* returnStartMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* quitButton;

	// 연결된 UI컴포넌트
	UPROPERTY()
	UInGameUIComponent* uiComponent;

};
