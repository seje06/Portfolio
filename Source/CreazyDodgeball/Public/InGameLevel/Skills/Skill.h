// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Characters/CharacterEnums.h"
#include "Skill.generated.h"

UCLASS(Abstract)
class CREAZYDODGEBALL_API ASkill : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkill();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/// <summary>
	/// 스킬 추상함수
	/// </summary>
	/// <param name="ball">제어할 볼</param>
	virtual void ActiveSkill(AActor* _ball) PURE_VIRTUAL(ASkill::ActiveSkill, ; ;);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/// <summary>
	/// 제어할 볼
	/// </summary>
	AActor* ball=nullptr;

};
