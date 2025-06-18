// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Ball/BallUIComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UBallUIComponent::UBallUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBallUIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBallUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBallUIComponent::DrawPredictedTrajectory(const FVector& _startLocation, const FVector& _velocity)
{
	const UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}

	FPredictProjectilePathParams params;
	params.StartLocation = _startLocation;
	params.LaunchVelocity = _velocity;
	params.ProjectileRadius = 5.0f;
	params.SimFrequency = 15.0f;
	params.MaxSimTime = 3.0f;
	params.bTraceWithCollision = true;
	params.TraceChannel = ECC_Visibility;
	params.ActorsToIgnore.Add(GetOwner());
	params.DrawDebugType = EDrawDebugTrace::ForDuration;
	params.DrawDebugTime = 1.0f;

	FPredictProjectilePathResult result;

	if (UGameplayStatics::PredictProjectilePath(world, params, result))
	{
		//도착지점에 원 그리기
		DrawDebugSphere(world, result.HitResult.Location, 15.0f, 16, FColor::Red, false, 1.0f);
	}
}
