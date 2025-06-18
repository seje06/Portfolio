#include "StartLevel/SettingsWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "DodgeballGameInstance.h"
#include "Sound/SoundClass.h"


void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (volumeSlider)
	{
		volumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnVolumeChanged);
	}

	if (closeButton)
	{
		closeButton->OnClicked.AddDynamic(this, &USettingsWidget::OnCloseCliked);
	}

	//GameInstance에서 현재 볼륨 값 가져와서 초기화
	if (auto* gameInstance = Cast<UDodgeballGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		float savedVolume = gameInstance->GetMasterVolume();
		volumeSlider->SetValue(savedVolume);
		UpdateVolumeText(savedVolume);
	}
}

void USettingsWidget::OnVolumeChanged(float _value)
{
	UpdateVolumeText(_value);
	SaveVolumeToGameInstance(_value);

	if (masterSoundClass)
	{
		masterSoundClass->Properties.Volume = _value;
	}
}

void USettingsWidget::OnCloseCliked()
{
	// 위젯 닫기
	this->RemoveFromParent(); 
}

void USettingsWidget::UpdateVolumeText(float _value)
{
	if (volumeValueText)
	{
		int32 intValue = FMath::RoundToInt(_value * 10.0f); // 0.0~1.0 → 0~10 변환
		volumeValueText->SetText(FText::FromString(FString::Printf(TEXT("%d"), intValue)));
	}
}

void USettingsWidget::SaveVolumeToGameInstance(float _value)
{
	// 변경된 값을 GameInstance에 저장
	if (auto* gameInstance = Cast<UDodgeballGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		gameInstance->SetMasterVolume(_value);
	}
}
