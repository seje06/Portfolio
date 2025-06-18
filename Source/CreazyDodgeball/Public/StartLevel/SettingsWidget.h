#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundClass.h"
#include "SettingsWidget.generated.h"


class USlider;
class UTextBlock;
class UButton;

UCLASS()
class CREAZYDODGEBALL_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	//�����̴� �� ���� ��
	UFUNCTION()
	void OnVolumeChanged(float _value);

	//�ݱ� ��ư
	UFUNCTION()
	void OnCloseCliked();

	//���� ���� �ؽ�Ʈ ǥ��
	void UpdateVolumeText(float _value);

	//���� �ν��ͽ��� ����
	void SaveVolumeToGameInstance(float _value);

public:

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundClass* masterSoundClass;

protected:
	
	UPROPERTY(meta = (BindWidget))
	USlider* volumeSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* volumeValueText;

	UPROPERTY(meta = (BindWidget))
	UButton* closeButton;
};
