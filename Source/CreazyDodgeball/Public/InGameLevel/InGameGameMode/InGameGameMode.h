#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameLevel/Characters/CharacterEnums.h"
#include "InGameGameMode.generated.h"

class UInGameUIComponent;
class UFadeWidget;

DECLARE_DELEGATE(InGameGameModeDelegate)

UCLASS()
class CREAZYDODGEBALL_API AInGameGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AInGameGameMode();

	virtual void BeginPlay() override;

	//조작 설명창
	void ShowControlGuide();
	UFUNCTION(BlueprintCallable)
	void OnControlGuideClosed();

	//세트 관련
	UFUNCTION(BlueprintCallable)
	void ProceedSet(ECharacterTeamType _winnerTeam);
	UFUNCTION(BlueprintCallable)
	void StartSet();

	void InitSet();

	void GameStart();

	UFUNCTION()
	void OnTimeOut();

	//캐릭터 상태 관련
	void ReviveCharacter(ECharacterTeamType getInTeamType);
	void OutCharacter(AActor* _character);

	// ESC 입력 시 일시정지 메뉴 처리
	UFUNCTION()
	void HandlePauseMenu();

	// InGame UI 초기화 함수
	void InitGameUI();

	void EndGame();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	inline int GetCurrentTeamASet() { return teamASetWin; }
	inline int GetCurrentTeamBSet() { return teamBSetWin; }


protected:
	//점수 관련
	UFUNCTION(BlueprintCallable)
	void AddScore(ECharacterTeamType _teamType, int32 _amount);
	UFUNCTION(BlueprintCallable)
	void SubtractScore(ECharacterTeamType _teamType, int32 _amount);

public:
	InGameGameModeDelegate OnSetStarted;

	//UI 연결용 컴포넌트(위젯 제어)
	UPROPERTY()
	class UInGameUIComponent* inGameUIComponent;

	//현재 세트
	int32 currentSet = 1;
protected:
	//조작 설명창 UI 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> controlGuideWidgetClass;

	//// UI 컴포넌트 BP 클래스 지정용
	//UPROPERTY(EditDefaultsOnly, Category = "UI")
	//TSubclassOf<class UInGameUIComponent> inGameUIComponentClass;

	//AI매니저 생성
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADodgeballAIManager> aiManagerClass;

	/*UPROPERTY(EditDefaultsOnly)
	class UBaseSound* chearingSound = nullptr;*/

	//인게임 인트로 시퀀스
	UPROPERTY(EditAnywhere, Category = "Cinematic")
	class ALevelSequenceActor* levelSequenceActor;


private:

	// 페이드 위젯 클래스 (BP_WBP_FadeWidget을 세팅할 것)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UFadeWidget> fadeWidgetClass;

	// 생성된 실제 페이드 위젯 인스턴스
	UPROPERTY()
	UFadeWidget* fadeWidgetInstance;


	UPROPERTY()
	class ADodgeballAIManager* aIManager;

	//조작설명창 위젯 인스턴스
	UPROPERTY()
	UUserWidget* controlGuideWidget;

	// 조작 설명창 닫힘 여부
	bool bControlGuideClosed = false;

	// 각 팀의 점수
	int32 teamAScore = 0;
	int32 teamBScore = 0;

	// 6점 선취 시 세트 승리
	UPROPERTY(EditAnywhere, Category = "GameRule")
	int32 scoreToWinSet = 6;  

	// 각 팀이 이긴 세트 수
	int32 teamASetWin = 0;
	int32 teamBSetWin = 0;

	bool isTimeOut = false;

	FTimerHandle timerHandleFade;
	FTimerHandle ResumeHandle;
	FTimerHandle timerHandleEndGame;
	FTimerHandle firstSetHandleOnFadeInEnded;
	FTimerHandle handleOnCinematicEnded;
};
