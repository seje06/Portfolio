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
	// �ݱ� ��ư (BindWidget)
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;

private:
	// ��ư Ŭ�� ó�� �Լ�
	UFUNCTION()
	void HandleCloseButtonClicked();

};
