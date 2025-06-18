// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeWidget.generated.h"

class UImage;

UCLASS()
class CREAZYDODGEBALL_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 페이드 인
    UFUNCTION(BlueprintCallable)
    void StartFadeIn(float _fadeDuration);

    // 페이드 아웃
    UFUNCTION(BlueprintCallable)
    void StartFadeOut(float _fadeDuration);

    virtual void NativeDestruct() override;

    // 현재 페이드가 진행 중인지 체크
    UFUNCTION(BlueprintPure, Category = "Fade")
    bool IsFading() const { return bIsFading; }


protected:
    virtual void NativeConstruct() override;

private:
    
    // Tick처럼 주기적으로 알파를 업데이트할 함수
    void UpdateFade();

    // 바인딩할 페이드용 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* fadeImage;

    // 현재 알파값
    float currentAlpha;

    // 알파값 변화 속도
    float fadeSpeed;

    // 페이드 타겟 (0.0 = 투명, 1.0 = 불투명)
    float targetAlpha;

    // 페이드 중 여부
    bool bIsFading;

    // 페이드 진행 방향 (true = In, false = Out)
    bool bFadeIn;
 
    // 내부 타이머 핸들
    FTimerHandle fadeTimerHandle;
};
