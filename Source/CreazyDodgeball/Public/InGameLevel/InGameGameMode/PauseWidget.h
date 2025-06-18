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
	// UIComponent ���� �Լ�
	void SetUIComponent(UInGameUIComponent* _uiComponent);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:
	virtual void NativeConstruct() override;

private:
	// Ŭ�� ó�� �Լ�
	UFUNCTION()
	void HandleResumeClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleReturnStartMenuClicked();

	UFUNCTION()
	void HandleQuitClicked();
	
	// ��ư��
	UPROPERTY(meta = (BindWidget))
	UButton* resumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* settingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* returnStartMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* quitButton;

	// ����� UI������Ʈ
	UPROPERTY()
	UInGameUIComponent* uiComponent;

};
