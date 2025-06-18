#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartLevelManager.generated.h"

class UUserWidget;
class USettingsWidget;
class UInputMappingContext;
class UInputAction;
class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;
class UFadeWidget;

UCLASS()
class CREAZYDODGEBALL_API AStartLevelManager : public AActor
{
	GENERATED_BODY()
	
public:	

	AStartLevelManager();

	//버튼 처리 함수들
	UFUNCTION()
	void OnStartGame();

	UFUNCTION()
	void OnQuitGame();

	UFUNCTION()
	void OnOpenSettings();
		
	//esc 입력 제거
	void DisableEscInput();

protected:

	virtual void BeginPlay() override;

private:
	//시네마틱 재생
	void PlayOpeningCinematic();

	// ESC 키 입력 처리 함수
	void OnToggleSettings(const struct FInputActionInstance& Instance);


	//시네마틱 종료 후 시작 메뉴 출력
	UFUNCTION()
	void ShowStartMenu();
	
	//설정창 출력
	void ShowSettingsWidget();
	//설정창 닫기
	void HideSettingsWidget();
	//종료경고창 출력
	void ShowExitConfirmWidget();
	//종료경고창 닫기
	void HideExitConfirmWidget();

	//InGameLevel로 이동
	void MoveToInGameLevel();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	//오프닝 시네마틱
	UPROPERTY(EditAnywhere, Category = "Cinematic")
	ULevelSequence* openingSequence;

	//시네마틱 플레이어 캐시
	UPROPERTY()
	ULevelSequencePlayer* sequencePlayer;

	//시작메뉴 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> startMenuWidgetClass;
	
	//설정 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> settingsWidgetClass;

	// ExitConfirm 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> exitConfirmWidgetClass;

	// 페이드 위젯 클래스 (BP_WBP_FadeWidget을 세팅할 것)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UFadeWidget> fadeWidgetClass;

	// 로딩창 위젯클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> loadingWidgetClass;

	//로딩창 인스턴스
	UPROPERTY()
	UUserWidget* loadingWidgetInstance;

	// 생성된 실제 페이드 위젯 인스턴스
	UPROPERTY()
	UFadeWidget* fadeWidgetInstance;

	//현재 표시 중인 위젯 (시작메뉴 or 설정)
	UPROPERTY()
	UUserWidget* currentWidget;

	UPROPERTY()
	UUserWidget* startMenuWidget;

	UPROPERTY()
	UUserWidget* settingsWidget;

	UPROPERTY()
	UUserWidget* exitConfirmWidget;

	// Enhanced Input
	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputMappingContext * inputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input")
	UInputAction* inputActionToggleSettings;

	FTimerHandle fadeOutTimerHandle; // 페이드 아웃 타이머
	FTimerHandle delayHandle;
};
