#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DodgeballGameInstance.generated.h"

//���� ��ü���� ������ ���� ������ Ŭ����
UCLASS()
class CREAZYDODGEBALL_API UDodgeballGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // ���� ���� ���� (0.0f ~ 1.0f)
    void SetMasterVolume(float _volume);

    // ���� ���� �� ��ȯ
    float GetMasterVolume() const;

    // ������ ���� �� (1.0 = 100%)
    float masterVolume = 1.0f;

    // ���ۼ���â
    UPROPERTY(BlueprintReadWrite)
    bool bSkipControlGuide = false;
};
