#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlGuideWidget.generated.h"

class UButton;

UCLASS()
class CREAZYDODGEBALL_API UControlGuideWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	// 닫기 버튼 (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;

private:
	// 버튼 클릭 처리 함수
	UFUNCTION()
	void HandleCloseButtonClicked();

};
