// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Skills/Skill.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
// Sets default values
ASkill::ASkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkill::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

