#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartMenuWidget.generated.h"

class UButton;
class AStartLevelManager;

UCLASS()
class CREAZYDODGEBALL_API UStartMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetManager(AStartLevelManager* _manager);

protected:
	virtual void NativeConstruct() override;

private:	
	
	//��ưŬ�� �ڵ鷯��
	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleQuitClicked();
	
protected:

	//������ ���ε� �� ��ư��
	UPROPERTY(meta = (BindWidget))
	UButton* startButton;

	UPROPERTY(meta = (BindWidget))
	UButton* settingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* quitButton;

private:

	//StartLevelManager ����(��� ȣ���)
	UPROPERTY()
	AStartLevelManager* manager;

};
