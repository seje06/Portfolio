#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnConfirmWidget.generated.h"

class UButton;

UCLASS()
class CREAZYDODGEBALL_API UReturnConfirmWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnYesClicked();

	UFUNCTION()
	void OnNoClicked();

	// 위젯에 연결된 버튼들
	UPROPERTY(meta = (BindWidget))
	UButton* yesButton;

	UPROPERTY(meta = (BindWidget))
	UButton* noButton;
	
};
