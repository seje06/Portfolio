#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InGameLevel/Characters/CharacterEnums.h"
#include "InGameLevel/InGameGameMode/TeamShotClockWidget.h"
#include "InGameUIComponent.generated.h"

class UTextBlock;
class UHorizontalBox;
class UImage;
class UUserWidget;
class UProgressBar;

DECLARE_DELEGATE(InGameUIDelegate)

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CREAZYDODGEBALL_API UInGameUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInGameUIComponent();
	
	//위젯 생성
	void CreateInGameLevelWidget();

	// 점수 UI만 갱신 (GameMode에서 호출)
	void UpdateScoreText(int32 _teamAScore, int32 _teamBScore);

	// 세트 아이콘 UI 갱신 (GameMode에서 호출)
	void UpdateSetScore(ECharacterTeamType _teamType, int32 _winSetCount);

	// 타이머 시작
	void StartGameTimer();

	//일시정지창 출력
	void ShowPauseWidget();

	//설정창 출력
	void ShowSettingsWidget();
	
	//게임종료창 출력
	void ShowExitConfirmWidget();
	
	//시작메뉴창 출력
	void ShowReturnConfirmWidget();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//esc키 입력시 호출
	void OnPauseToggle();

	//fadeUI 생성 및 fade out
	void FadeOut(float _fadeDuration);
	//in
	void FadeIn(float _fadeDuration);

	//샷클락 업데이트
	void UpdateShotClock(ECharacterTeamType _teamType, int32 _remainingTime);

	//게임승리위젯 출력
	void ShowEndSetWidget(ECharacterTeamType _winnerTeam);

protected:
	virtual void BeginPlay() override;
	
private:
	//타이머 업데이트
	void UpdateTimer();

	void UpdateSkillEnergyBar(int32 _currentEnergy);

public:
	InGameUIDelegate OnUITimeOut;

private:
	// 헤더 UI 위젯 클래스 및 인스턴스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> headerDisplayWidgetClass;

	UPROPERTY()
	UUserWidget* headerDisplayWidget = nullptr;

	// 헤더 UI 내부 구성 요소
	UPROPERTY()
	UTextBlock* timerText;

	UPROPERTY()
	UTextBlock* teamAScoreText;

	UPROPERTY()
	UTextBlock* teamBScoreText;

	UPROPERTY()
	UHorizontalBox* teamASetBox;

	UPROPERTY()
	UHorizontalBox* teamBSetBox;

	// 세트 아이콘 이미지
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* activeSetIcon;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* inactiveSetIcon;

	// 타이머 처리 관련
	FTimerHandle timerHandle;
	FTimerHandle timerHandle_fade;
	int32 currentTime = 60;
	int32 maxTime = 60;

	//일시정지 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> pauseWidgetClass;

	//설정 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> settingsWidgetClass;
	
	//게임종료 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> exitConfirmWidgetClass;

	// ReturnConfirm 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> returnConfirmWidgetClass;

	// 페이드 위젯 클래스 (BP_WBP_FadeWidget을 세팅할 것)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UFadeWidget> fadeWidgetClass;

	// EndSetWidget 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UEndSetWidget> endSetWidgetClass;

	// TeamShotClock 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UTeamShotClockWidget> teamShotClockWidgetClass;

	// 생성된 실제 위젯 인스턴스들
	UPROPERTY()
	class UTeamShotClockWidget* teamShotClockWidget = nullptr;

	UPROPERTY()
	class UEndSetWidget* endSetWidget = nullptr;

	UPROPERTY()
	class UFadeWidget* fadeWidgetInstance = nullptr;

	UPROPERTY()
	UUserWidget* pauseWidget = nullptr;

	UPROPERTY()
	UUserWidget* settingsWidget = nullptr;

	UPROPERTY()
	UUserWidget* exitConfirmWidget = nullptr;

	UPROPERTY()
	UUserWidget* returnConfirmWidget = nullptr;

	//스킬바
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* skillEnergyBar;
};
