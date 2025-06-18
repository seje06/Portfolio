#include "DodgeballGameInstance.h"

void UDodgeballGameInstance::SetMasterVolume(float _volume)
{
	// ���� 0~1 ���� ����� �ʰ� ����
	masterVolume = FMath::Clamp(_volume, 0.0f, 1.0f);
}

float UDodgeballGameInstance::GetMasterVolume() const
{
	return masterVolume;
}
