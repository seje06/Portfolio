#include "InGameLevel/Skills/CloneSkill.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "InGameLevel/Ball/CloneBall.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ACloneSkill::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
}

void ACloneSkill::ActiveSkill(AActor* _ball)
{
	if (!_ball) return;
	this->ball = _ball;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(this->ball);
	if (!dodgeBall) return;

	dodgeBall->controllingSkillType = ECharacterSkillType::Clone;

	cloneState = ECloneState::PreClone;
}

void ACloneSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ball) return;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(ball);
	if (!dodgeBall || !dodgeBall->ballTrajectortyComp) return;

	switch (cloneState)
	{
	case ECloneState::PreClone:
	{
		//거리 계산
		float moveDistance = FVector::Dist(dodgeBall->launchStartLocation, dodgeBall->GetActorLocation());
		float totalDistance = FVector::Dist(dodgeBall->launchStartLocation, dodgeBall->ballTrajectortyComp->impactLocation);

		if (moveDistance >= totalDistance * 0.5f || dodgeBall->hitCharacters.Num() > 0)
		{
			cloneState = ECloneState::DoClone;
			SpawnClones();
		}
		else if (dodgeBall->bIsGrounded)
		{
			cloneState = ECloneState::DoClone;
		}
		break;
	}
	case ECloneState::DoClone:
	{
		cloneState = ECloneState::EndClone;
		break;
	}
	case ECloneState::EndClone:
	{
		if (dodgeBall->bIsGrounded|| dodgeBall->bIsCahtched)
		{
			dodgeBall->controllingSkillType = ECharacterSkillType::None;
			DestroyAllClones();
			Destroy();
		}
		break;
	}
	}
}

void ACloneSkill::SpawnClones()
{
	if (!ball)	return;

	ADodgeBall* originalBall = Cast<ADodgeBall>(ball);
	if (!originalBall)	return;
	if (!originalBall->sphereComp)	return;

	UWorld* world = GetWorld();
	if (!world)	return;

	FVector velocity = originalBall->sphereComp->GetPhysicsLinearVelocity();
	if (velocity.Size() < 10.0f) return;

	FVector direction = velocity.GetSafeNormal();
	FVector currentLocation = originalBall->GetActorLocation();

	FRotator leftRot = direction.Rotation() + FRotator(0, +20.0f, 0);
	FRotator rightRot = direction.Rotation() + FRotator(0, -20.0f, 0);
	FVector leftDir = leftRot.Vector();
	FVector rightDir = rightRot.Vector();

	FVector offset = FVector::CrossProduct(direction, FVector::UpVector).GetSafeNormal();
	float offsetAmount = 20.0f;

	FVector leftSpawn = currentLocation - offset * offsetAmount;
	FVector rightSpawn = currentLocation + offset * offsetAmount;

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;


	// 왼쪽 분신
	leftClone = world->SpawnActor<ACloneBall>(cloneBallClass, leftSpawn, leftRot, spawnParams);
	if (leftClone)
	{
		if (leftClone->sphereComp)
		{
			leftClone->sphereComp->SetSimulatePhysics(true);
			leftClone->sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			leftClone->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

			FVector launchVelocity = leftDir.GetSafeNormal() * velocity.Size();
			leftClone->sphereComp->AddImpulse(launchVelocity * 9.f);
			
		}

		ADodgeBall* dodgeBall = Cast<ADodgeBall>(ball);
		leftClone->sphereComp->OnComponentHit.AddDynamic(dodgeBall, &ADodgeBall::OnHit);

		cloneBalls.Add(leftClone);
		leftClone->SetLifeSpan(3.0f);
	}

	// 오른쪽 분신
	rightClone = world->SpawnActor<ACloneBall>(cloneBallClass, rightSpawn, rightRot, spawnParams);
	if (rightClone)
	{
		if (rightClone->sphereComp)
		{
			rightClone->sphereComp->SetSimulatePhysics(true);
			rightClone->sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			rightClone->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

			FVector launchVelocity = rightDir.GetSafeNormal() * velocity.Size();
			rightClone->sphereComp->AddImpulse(launchVelocity * 9.f);
		}
	
		ADodgeBall* dodgeBall = Cast<ADodgeBall>(ball);
		rightClone->sphereComp->OnComponentHit.AddDynamic(dodgeBall, &ADodgeBall::OnHit);

		cloneBalls.Add(rightClone);
		rightClone->SetLifeSpan(3.0f);
	}

}

void ACloneSkill::DestroyAllClones()
{
	if (leftClone)
	{
		leftClone->Destroy();
		leftClone = nullptr;
	}

	if (rightClone)
	{
		rightClone->Destroy();
		rightClone = nullptr;
	}
}
