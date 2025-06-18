#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DodgeballGameInstance.generated.h"

//게임 전체에서 유지될 전역 데이터 클래스
UCLASS()
class CREAZYDODGEBALL_API UDodgeballGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // 사운드 볼륨 설정 (0.0f ~ 1.0f)
    void SetMasterVolume(float _volume);

    // 사운드 볼륨 값 반환
    float GetMasterVolume() const;

    // 마스터 볼륨 값 (1.0 = 100%)
    float masterVolume = 1.0f;

    // 조작설명창
    UPROPERTY(BlueprintReadWrite)
    bool bSkipControlGuide = false;
};
