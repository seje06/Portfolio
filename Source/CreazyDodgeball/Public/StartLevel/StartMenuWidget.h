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
	
	//버튼클릭 핸들러들
	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleQuitClicked();
	
protected:

	//위젯에 바인딩 될 버튼들
	UPROPERTY(meta = (BindWidget))
	UButton* startButton;

	UPROPERTY(meta = (BindWidget))
	UButton* settingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* quitButton;

private:

	//StartLevelManager 참조(기능 호출용)
	UPROPERTY()
	AStartLevelManager* manager;

};
