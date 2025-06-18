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
    // ���̵� ��
    UFUNCTION(BlueprintCallable)
    void StartFadeIn(float _fadeDuration);

    // ���̵� �ƿ�
    UFUNCTION(BlueprintCallable)
    void StartFadeOut(float _fadeDuration);

    virtual void NativeDestruct() override;

    // ���� ���̵尡 ���� ������ üũ
    UFUNCTION(BlueprintPure, Category = "Fade")
    bool IsFading() const { return bIsFading; }


protected:
    virtual void NativeConstruct() override;

private:
    
    // Tickó�� �ֱ������� ���ĸ� ������Ʈ�� �Լ�
    void UpdateFade();

    // ���ε��� ���̵�� �̹���
    UPROPERTY(meta = (BindWidget))
    UImage* fadeImage;

    // ���� ���İ�
    float currentAlpha;

    // ���İ� ��ȭ �ӵ�
    float fadeSpeed;

    // ���̵� Ÿ�� (0.0 = ����, 1.0 = ������)
    float targetAlpha;

    // ���̵� �� ����
    bool bIsFading;

    // ���̵� ���� ���� (true = In, false = Out)
    bool bFadeIn;
 
    // ���� Ÿ�̸� �ڵ�
    FTimerHandle fadeTimerHandle;
};
