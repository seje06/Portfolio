#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "InGameLevel/DodgeballGameState.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"

// Sets default values for this component's properties
UBallTrajectoryComponent::UBallTrajectoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	trajectorySpline = nullptr;
}


void UBallTrajectoryComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UBallTrajectoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UBallTrajectoryComponent::DrawPredictedTrajectory(const FVector& _startLocation, const FVector& _velocity)
{
	ClearTrajectory();

	FPredictProjectilePathParams params;
	params.StartLocation = _startLocation;
	params.LaunchVelocity = _velocity;
	params.ProjectileRadius = 5.0f;
	params.SimFrequency = 15.0f;
	params.MaxSimTime = 3.0f;
	params.bTraceWithCollision = true;
	params.TraceChannel = ECC_Visibility;
	params.ActorsToIgnore.Add(GetOwner());

	//같은 팀 캐릭들은 무시
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if (gameState->currentHavingTurnTeamType == ECharacterTeamType::A)
	{
		for(auto& charac : *gameState->charactersA) if(charac) params.ActorsToIgnore.Add(charac);
	}
	else
	{
		for (auto& charac : *gameState->charactersB) if (charac) params.ActorsToIgnore.Add(charac);
	}
	FPredictProjectilePathResult result;
	if (!UGameplayStatics::PredictProjectilePath(GetWorld(), params, result))
	{
		return;
	}

	//splineComp 생성 및 설정
	if (!trajectorySpline)
	{
		trajectorySpline = NewObject<USplineComponent>(this, TEXT("TrajectorySplineComp"));
		trajectorySpline->RegisterComponent();
		trajectorySpline->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	}

	trajectorySpline->ClearSplinePoints(false);

	for (const auto& Point : result.PathData)
	{
		trajectorySpline->AddSplinePoint(Point.Location, ESplineCoordinateSpace::World, false);
	}
	trajectorySpline->UpdateSpline();
	
	//볼과 궤도의 분리
	if (trajectorySpline)
	{
		trajectorySpline->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	//splineMeshComponent 생성, player일때만
	if (!gameState->currentCatchingBallCharac || !IsValid(gameState->currentCatchingBallCharac)) return;

	auto charac = Cast<ADodgeballCharacter>(gameState->currentCatchingBallCharac);
	if (charac && charac->type == ECharacterType::Player)
	for (int i = 0; i < trajectorySpline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* mesh = NewObject<USplineMeshComponent>(this);
		mesh->SetMobility(EComponentMobility::Movable);
		mesh->SetStaticMesh(trajectoryMesh);
		mesh->SetMaterial(0, trajectoryMaterial);
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mesh->RegisterComponentWithWorld(GetWorld());
		mesh->AttachToComponent(trajectorySpline, FAttachmentTransformRules::KeepRelativeTransform);

		FVector startPos;
		FVector startTangent;
		FVector endPos;
		FVector endTangent;

		trajectorySpline->GetLocationAndTangentAtSplinePoint(i, startPos, startTangent, ESplineCoordinateSpace::World);
		trajectorySpline->GetLocationAndTangentAtSplinePoint(i + 1, endPos, endTangent, ESplineCoordinateSpace::World);

		mesh->SetStartAndEnd(startPos, startTangent, endPos, endTangent);
		mesh->SetStartScale(FVector2D(0.05f, 0.05f));
		mesh->SetEndScale(FVector2D(0.05f, 0.05f));
		splineMeshes.Add(mesh);
	}

	//도착 지점 설정
	impactLocation = result.HitResult.Location;

	// 도착 지점 표시 컴포넌트 생성, 플레이어 일때만
	if (charac && charac->type == ECharacterType::Player)
	{
		impactMarker = NewObject<UStaticMeshComponent>(this);
		impactMarker->RegisterComponent();
		impactMarker->SetWorldLocation(impactLocation);
		impactMarker->SetWorldRotation(FRotator::ZeroRotator);
		impactMarker->SetStaticMesh(impactMesh);
		impactMarker->SetMaterial(0, impactMaterial);
		impactMarker->SetWorldScale3D(FVector(0.3f));
		impactMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		impactMarker->SetSimulatePhysics(false);
		impactMarker->SetMobility(EComponentMobility::Movable);
	}
}

void UBallTrajectoryComponent::ClearTrajectory()
{
	for (auto* Mesh : splineMeshes)
	{
		if (Mesh)
		{
			Mesh->DestroyComponent();
		}
	}

	splineMeshes.Empty();

	if (trajectorySpline)
	{
		trajectorySpline->ClearSplinePoints();
		trajectorySpline->UpdateSpline();
	}

	if (impactMarker)
	{
		impactMarker->DestroyComponent();
		impactMarker = nullptr;
	}
}
