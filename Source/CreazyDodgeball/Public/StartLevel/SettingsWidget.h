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
	//슬라이더 값 변경 시
	UFUNCTION()
	void OnVolumeChanged(float _value);

	//닫기 버튼
	UFUNCTION()
	void OnCloseCliked();

	//볼륨 값을 텍스트 표시
	void UpdateVolumeText(float _value);

	//게임 인스터스에 저장
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
