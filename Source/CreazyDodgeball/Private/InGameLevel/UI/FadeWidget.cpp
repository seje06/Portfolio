#include "InGameLevel/UI/FadeWidget.h"
#include "Components/Image.h"
#include "TimerManager.h"



void UFadeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기값 설정
    if (fadeImage)
    {
        FLinearColor color = fadeImage->ColorAndOpacity;
        color.A = 0.0f;
        fadeImage->SetColorAndOpacity(color);
    }

    currentAlpha = 0.0f;
    fadeSpeed = 0.0f;
    targetAlpha = 0.0f;
    bIsFading = false;
}

void UFadeWidget::UpdateFade()
{
    if (!fadeImage)
        return;

    // 0.01초마다 알파값 갱신
    float deltaAlpha = fadeSpeed * 0.01f;

    if (bFadeIn)
    {
        UE_LOG(LogTemp, Warning, TEXT("current fadein"));
        currentAlpha -= deltaAlpha;
        if (currentAlpha <= targetAlpha)
        {
            currentAlpha = targetAlpha;
            bIsFading = false;
            GetWorld()->GetTimerManager().ClearTimer(fadeTimerHandle);
            UE_LOG(LogTemp, Warning, TEXT("End fadein"));
        }
    }
    else
    {
        currentAlpha += deltaAlpha;
        if (currentAlpha >= targetAlpha)
        {
            currentAlpha = targetAlpha;
            bIsFading = false;
            GetWorld()->GetTimerManager().ClearTimer(fadeTimerHandle);
        }
    }

    // 실제 적용
    FLinearColor color = fadeImage->ColorAndOpacity;
    color.A = currentAlpha;
    fadeImage->SetColorAndOpacity(color);
}

void UFadeWidget::StartFadeIn(float _fadeDuration)
{
    if (!fadeImage || _fadeDuration <= 0.f)
        return;
    UE_LOG(LogTemp, Warning, TEXT("fadeIning...."));
    GetWorld()->GetTimerManager().ClearTimer(fadeTimerHandle);

    bFadeIn = true;
    bIsFading = true;
    currentAlpha = 1.0f; // 시작은 완전 불투명
    targetAlpha = 0.0f;  // 목표는 완전 투명
    fadeSpeed = 1.0f / _fadeDuration;

    // 강제로 현재 알파 적용
    FLinearColor color = fadeImage->ColorAndOpacity;
    color.A = currentAlpha;
    fadeImage->SetColorAndOpacity(color);

    GetWorld()->GetTimerManager().SetTimer(fadeTimerHandle, this, &UFadeWidget::UpdateFade, 0.01f, true);
}

void UFadeWidget::StartFadeOut(float _fadeDuration)
{
    if (!fadeImage || _fadeDuration <= 0.f)
        return;
    UE_LOG(LogTemp, Warning, TEXT("fadeouting...."));
    GetWorld()->GetTimerManager().ClearTimer(fadeTimerHandle);

    bFadeIn = false;
    bIsFading = true;
    currentAlpha = 0.0f; // 시작은 완전 투명
    targetAlpha = 1.0f;  // 목표는 완전 불투명
    fadeSpeed = 1.0f / _fadeDuration;

    // 강제로 현재 알파 적용
    FLinearColor color = fadeImage->ColorAndOpacity;
    color.A = currentAlpha;
    fadeImage->SetColorAndOpacity(color);

    GetWorld()->GetTimerManager().SetTimer(fadeTimerHandle, this, &UFadeWidget::UpdateFade, 0.01f, true);

}

void UFadeWidget::NativeDestruct()
{
    Super::NativeDestruct();

    GetWorld()->GetTimerManager().ClearTimer(fadeTimerHandle);
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

