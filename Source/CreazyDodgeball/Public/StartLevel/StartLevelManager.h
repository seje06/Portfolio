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

	//��ư ó�� �Լ���
	UFUNCTION()
	void OnStartGame();

	UFUNCTION()
	void OnQuitGame();

	UFUNCTION()
	void OnOpenSettings();
		
	//esc �Է� ����
	void DisableEscInput();

protected:

	virtual void BeginPlay() override;

private:
	//�ó׸�ƽ ���
	void PlayOpeningCinematic();

	// ESC Ű �Է� ó�� �Լ�
	void OnToggleSettings(const struct FInputActionInstance& Instance);


	//�ó׸�ƽ ���� �� ���� �޴� ���
	UFUNCTION()
	void ShowStartMenu();
	
	//����â ���
	void ShowSettingsWidget();
	//����â �ݱ�
	void HideSettingsWidget();
	//������â ���
	void ShowExitConfirmWidget();
	//������â �ݱ�
	void HideExitConfirmWidget();

	//InGameLevel�� �̵�
	void MoveToInGameLevel();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	//������ �ó׸�ƽ
	UPROPERTY(EditAnywhere, Category = "Cinematic")
	ULevelSequence* openingSequence;

	//�ó׸�ƽ �÷��̾� ĳ��
	UPROPERTY()
	ULevelSequencePlayer* sequencePlayer;

	//���۸޴� ���� Ŭ����
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> startMenuWidgetClass;
	
	//���� ���� Ŭ����
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> settingsWidgetClass;

	// ExitConfirm ���� Ŭ����
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> exitConfirmWidgetClass;

	// ���̵� ���� Ŭ���� (BP_WBP_FadeWidget�� ������ ��)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UFadeWidget> fadeWidgetClass;

	// �ε�â ����Ŭ����
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> loadingWidgetClass;

	//�ε�â �ν��Ͻ�
	UPROPERTY()
	UUserWidget* loadingWidgetInstance;

	// ������ ���� ���̵� ���� �ν��Ͻ�
	UPROPERTY()
	UFadeWidget* fadeWidgetInstance;

	//���� ǥ�� ���� ���� (���۸޴� or ����)
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

	FTimerHandle fadeOutTimerHandle; // ���̵� �ƿ� Ÿ�̸�
	FTimerHandle delayHandle;
};
