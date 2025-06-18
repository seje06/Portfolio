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
	
	//���� ����
	void CreateInGameLevelWidget();

	// ���� UI�� ���� (GameMode���� ȣ��)
	void UpdateScoreText(int32 _teamAScore, int32 _teamBScore);

	// ��Ʈ ������ UI ���� (GameMode���� ȣ��)
	void UpdateSetScore(ECharacterTeamType _teamType, int32 _winSetCount);

	// Ÿ�̸� ����
	void StartGameTimer();

	//�Ͻ�����â ���
	void ShowPauseWidget();

	//����â ���
	void ShowSettingsWidget();
	
	//��������â ���
	void ShowExitConfirmWidget();
	
	//���۸޴�â ���
	void ShowReturnConfirmWidget();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//escŰ �Է½� ȣ��
	void OnPauseToggle();

	//fadeUI ���� �� fade out
	void FadeOut(float _fadeDuration);
	//in
	void FadeIn(float _fadeDuration);

	//��Ŭ�� ������Ʈ
	void UpdateShotClock(ECharacterTeamType _teamType, int32 _remainingTime);

	//���ӽ¸����� ���
	void ShowEndSetWidget(ECharacterTeamType _winnerTeam);

protected:
	virtual void BeginPlay() override;
	
private:
	//Ÿ�̸� ������Ʈ
	void UpdateTimer();

	void UpdateSkillEnergyBar(int32 _currentEnergy);

public:
	InGameUIDelegate OnUITimeOut;

private:
	// ��� UI ���� Ŭ���� �� �ν��Ͻ�
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> headerDisplayWidgetClass;

	UPROPERTY()
	UUserWidget* headerDisplayWidget = nullptr;

	// ��� UI ���� ���� ���
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

	// ��Ʈ ������ �̹���
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* activeSetIcon;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* inactiveSetIcon;

	// Ÿ�̸� ó�� ����
	FTimerHandle timerHandle;
	FTimerHandle timerHandle_fade;
	int32 currentTime = 60;
	int32 maxTime = 60;

	//�Ͻ����� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> pauseWidgetClass;

	//���� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> settingsWidgetClass;
	
	//�������� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> exitConfirmWidgetClass;

	// ReturnConfirm ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> returnConfirmWidgetClass;

	// ���̵� ���� Ŭ���� (BP_WBP_FadeWidget�� ������ ��)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UFadeWidget> fadeWidgetClass;

	// EndSetWidget Ŭ����
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UEndSetWidget> endSetWidgetClass;

	// TeamShotClock Ŭ����
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UTeamShotClockWidget> teamShotClockWidgetClass;

	// ������ ���� ���� �ν��Ͻ���
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

	//��ų��
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* skillEnergyBar;
};
