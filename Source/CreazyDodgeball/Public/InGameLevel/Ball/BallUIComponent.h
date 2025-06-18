// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BallUIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CREAZYDODGEBALL_API UBallUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBallUIComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//궤도시각화 함수
	UFUNCTION()
	void DrawPredictedTrajectory(const FVector& _startLocation, const FVector& _velocity);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};
