#include "DodgeballGameInstance.h"

void UDodgeballGameInstance::SetMasterVolume(float _volume)
{
	// 값이 0~1 범위 벗어나지 않게 보정
	masterVolume = FMath::Clamp(_volume, 0.0f, 1.0f);
}

float UDodgeballGameInstance::GetMasterVolume() const
{
	return masterVolume;
}
