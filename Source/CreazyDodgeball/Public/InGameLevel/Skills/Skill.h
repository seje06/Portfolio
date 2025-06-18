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
	/// ��ų �߻��Լ�
	/// </summary>
	/// <param name="ball">������ ��</param>
	virtual void ActiveSkill(AActor* _ball) PURE_VIRTUAL(ASkill::ActiveSkill, ; ;);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/// <summary>
	/// ������ ��
	/// </summary>
	AActor* ball=nullptr;

};
