// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Characters/CharacterEnums.h"
#include "DodgeballAIManager.generated.h"

UCLASS()
class CREAZYDODGEBALL_API ADodgeballAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADodgeballAIManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init();

	void OnCharacterOut(class ADodgeballCharacter* charac);
	void OnCharacterIn(ECharacterTeamType getInTeamType);

	void UpdateTeamAAIFloorNum(float DeltaTime);
	void UpdateTeamBAIFloorNum(float DeltaTime);

	void DestroyAllAI();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	void TeamAAIInit();
	void TeamBAIInit();



	void DisappearAI(class ADodgeballGameState* gameState, class ADodgeballCharacter* charac);

	void MixFloorNum(TArray<class ADodgeballCharacter*>& _characters);
	void ChangeFloorNum(TArray<class ADodgeballCharacter*>& _characters, int indexA, int indexB);
protected:	
	//TMap<TSubclassOf<class ADodgeballCharacter>>
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> floorClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> floorClassB;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ADodgeballCharacter> dodgeballCharacterClass;
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInstance* aIMI = nullptr;
	UPROPERTY(EditDefaultsOnly)
	class UMaterialInstance* aICapMI = nullptr;
private:
	TArray<AActor*> floors;
	TArray<class ADodgeballCharacter*> characters;
	TArray<AActor*> floorsB;
	TArray<class ADodgeballCharacter*> charactersB;

	float floorNumMixingTimer = 5;
	float floorNumMixingTimerB = 5;

	class ADodgeballCharacter* currentPlayerCharacter=nullptr;
	int currentPlayerFloorNum = 1;
	int inFieldCount = 6;
	int inFieldCountB = 6;
};
