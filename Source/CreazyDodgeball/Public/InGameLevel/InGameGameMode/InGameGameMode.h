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

	//���� ����â
	void ShowControlGuide();
	UFUNCTION(BlueprintCallable)
	void OnControlGuideClosed();

	//��Ʈ ����
	UFUNCTION(BlueprintCallable)
	void ProceedSet(ECharacterTeamType _winnerTeam);
	UFUNCTION(BlueprintCallable)
	void StartSet();

	void InitSet();

	void GameStart();

	UFUNCTION()
	void OnTimeOut();

	//ĳ���� ���� ����
	void ReviveCharacter(ECharacterTeamType getInTeamType);
	void OutCharacter(AActor* _character);

	// ESC �Է� �� �Ͻ����� �޴� ó��
	UFUNCTION()
	void HandlePauseMenu();

	// InGame UI �ʱ�ȭ �Լ�
	void InitGameUI();

	void EndGame();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	inline int GetCurrentTeamASet() { return teamASetWin; }
	inline int GetCurrentTeamBSet() { return teamBSetWin; }


protected:
	//���� ����
	UFUNCTION(BlueprintCallable)
	void AddScore(ECharacterTeamType _teamType, int32 _amount);
	UFUNCTION(BlueprintCallable)
	void SubtractScore(ECharacterTeamType _teamType, int32 _amount);

public:
	InGameGameModeDelegate OnSetStarted;

	//UI ����� ������Ʈ(���� ����)
	UPROPERTY()
	class UInGameUIComponent* inGameUIComponent;

	//���� ��Ʈ
	int32 currentSet = 1;
protected:
	//���� ����â UI Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> controlGuideWidgetClass;

	//// UI ������Ʈ BP Ŭ���� ������
	//UPROPERTY(EditDefaultsOnly, Category = "UI")
	//TSubclassOf<class UInGameUIComponent> inGameUIComponentClass;

	//AI�Ŵ��� ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADodgeballAIManager> aiManagerClass;

	/*UPROPERTY(EditDefaultsOnly)
	class UBaseSound* chearingSound = nullptr;*/

	//�ΰ��� ��Ʈ�� ������
	UPROPERTY(EditAnywhere, Category = "Cinematic")
	class ALevelSequenceActor* levelSequenceActor;


private:

	// ���̵� ���� Ŭ���� (BP_WBP_FadeWidget�� ������ ��)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UFadeWidget> fadeWidgetClass;

	// ������ ���� ���̵� ���� �ν��Ͻ�
	UPROPERTY()
	UFadeWidget* fadeWidgetInstance;


	UPROPERTY()
	class ADodgeballAIManager* aIManager;

	//���ۼ���â ���� �ν��Ͻ�
	UPROPERTY()
	UUserWidget* controlGuideWidget;

	// ���� ����â ���� ����
	bool bControlGuideClosed = false;

	// �� ���� ����
	int32 teamAScore = 0;
	int32 teamBScore = 0;

	// 6�� ���� �� ��Ʈ �¸�
	UPROPERTY(EditAnywhere, Category = "GameRule")
	int32 scoreToWinSet = 6;  

	// �� ���� �̱� ��Ʈ ��
	int32 teamASetWin = 0;
	int32 teamBSetWin = 0;

	bool isTimeOut = false;

	FTimerHandle timerHandleFade;
	FTimerHandle ResumeHandle;
	FTimerHandle timerHandleEndGame;
	FTimerHandle firstSetHandleOnFadeInEnded;
	FTimerHandle handleOnCinematicEnded;
};
