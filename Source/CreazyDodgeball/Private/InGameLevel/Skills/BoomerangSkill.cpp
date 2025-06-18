// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Skills/BoomerangSkill.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "Components/SphereComponent.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/Ball/BallManagerComponent.h"

void ABoomerangSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!this->ball) return;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(this->ball);
	if (!dodgeBall) return;

	float goalOffsetDis;
	float movingDis;

	auto character = Cast<ADodgeballCharacter>(dodgeBall->recentThrownCharacter);
	if (!character) return;

	FVector goalLocation;
	
	switch (state)
	{
	case EBoomerangState::PreBoomerang:
		goalOffsetDis = FVector::Dist(dodgeBall->GetActorLocation(), dodgeBall->ballTrajectortyComp->impactLocation);
		movingDis = FVector::Dist(dodgeBall->launchStartLocation, dodgeBall->ballTrajectortyComp->impactLocation);

		if (/*goalOffsetDis < movingDis / 5 ||*/ dodgeBall->bIsGrounded || dodgeBall->hitCharacters.Num()>0)
		{
			if (dodgeBall->bIsGrounded) dodgeBall->bIsGrounded = false;
			state = EBoomerangState::Boomerang;
			dodgeBall->sphereComp->SetSimulatePhysics(false);
			dodgeBall->sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			dodgeBall->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
		break;
	case EBoomerangState::Boomerang:
		goalOffsetDis = FVector::Dist(dodgeBall->GetActorLocation(), character->ballManagerCom->GetComponentLocation());
		if (goalOffsetDis > 10)
		{
			goalLocation = FMath::VInterpTo(dodgeBall->GetActorLocation(), character->ballManagerCom->GetComponentLocation(), DeltaTime, 5);
			dodgeBall->SetActorLocation(goalLocation);
		}
		else
		{
			state = EBoomerangState::EndBoomerang;
		}
		break;
	case EBoomerangState::EndBoomerang:
		dodgeBall->sphereComp->SetSimulatePhysics(true);
		dodgeBall->sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		dodgeBall->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		dodgeBall->controllingSkillType = ECharacterSkillType::None;

		Destroy();
		break;
	}
}

void ABoomerangSkill::ActiveSkill(AActor* _ball)
{

	this->ball = _ball;
	if (!this->ball) return;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(this->ball);
	if (!dodgeBall) return;
	
	dodgeBall->controllingSkillType = ECharacterSkillType::Boomerang;
	//dodgeBall->OnCatched.AddUObject()
	//dodgeBall->OnCatched.AddUObject()
}

void ABoomerangSkill::BeginPlay()
{
	Super::BeginPlay();
}
