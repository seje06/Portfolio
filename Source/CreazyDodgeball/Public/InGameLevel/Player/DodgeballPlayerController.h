// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DodgeBallPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CREAZYDODGEBALL_API ADodgeballPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ADodgeballPlayerController();

	void Init();
	
	UFUNCTION()
	void TurnCharacter(AActor* charac);
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void SetCharacTurningWidget(AActor* charac);
	
	UFUNCTION()
	void SetSkillKeyWidgetAndEnergyBar(bool isVisibility);

	UFUNCTION()
	void SetPlayerEnergyBar(float energy);
protected:

	//플레이어 능력 컴포넌트 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlayerAbilityComponent> playerAbilityCompClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWidgetComponent> characTurningWidgetCompClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> skillKeyWidgetClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADodgeballAIManager> aiManagerClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEnergyBarWidget> energyBarWidgetClass;

private:
	class ADodgeballCharacter* currentPlayerCharacter=nullptr;
	class UPlayerAbilityComponent* playerAbilityComp=nullptr;
	class UWidgetComponent* characTurningWidgetComp = nullptr;
	class UUserWidget* skillKeyWidget=nullptr;
	class ADodgeballAIManager* aIManager=nullptr;
	class UEnergyBarWidget* energyBarWidget = nullptr;
};
