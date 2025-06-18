// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Skills/GigantSkill.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "Components/SphereComponent.h"

void AGigantSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!this->ball) return;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(this->ball);
	if (!dodgeBall) return;
	if (!IsValid(dodgeBall->sphereComp)) return;

	float goalOffsetDis = FVector::Dist(dodgeBall->GetActorLocation(), dodgeBall->ballTrajectortyComp->impactLocation);
	float movingDis = FVector::Dist(dodgeBall->launchStartLocation, dodgeBall->ballTrajectortyComp->impactLocation);

	switch (state)
	{
	case EGiantState::PreGiant:

		
		if (goalOffsetDis < movingDis / 10.f || dodgeBall->bIsGrounded || dodgeBall->hitCharacters.Num()>0)
		{
			state = EGiantState::Giant;
			dodgeBall->sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			dodgeBall->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			dodgeBall->sphereComp->SetSimulatePhysics(false);
			dodgeBall->sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		}

		break;
	case EGiantState::Giant:

		if (dodgeBall->sphereComp->GetRelativeScale3D().X < ballGoalScale.X)
		{
			dodgeBall->sphereComp->SetRelativeScale3D(dodgeBall->sphereComp->GetRelativeScale3D() + ballGoalScale * DeltaTime);
		}
		else
		{
			timer += DeltaTime;
		}

		if (timer > 1) state = EGiantState::EndGiant;

		break;
	case EGiantState::EndGiant:

		if (dodgeBall->sphereComp->GetRelativeScale3D().X > ballInitScale.X)
		{
			dodgeBall->sphereComp->SetRelativeScale3D(dodgeBall->sphereComp->GetRelativeScale3D() - ballGoalScale * DeltaTime);
		}
		else if(!dodgeBall->sphereComp->IsSimulatingPhysics())
		{
			dodgeBall->sphereComp->SetRelativeScale3D(ballInitScale);
			// 2. 물리 값 초기화
			dodgeBall->sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			dodgeBall->sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			dodgeBall->sphereComp->SetSimulatePhysics(true);
			dodgeBall->sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		}
		else if(dodgeBall->bIsGrounded)
		{
			dodgeBall->controllingSkillType = ECharacterSkillType::None;
			Destroy();
		}

		break;
	}

	
}

void AGigantSkill::ActiveSkill(AActor* _ball)
{
	if (!_ball) return;

	//Super::ActiveSkill(_ball);

	this->ball = _ball;
	
	ADodgeBall* dodgeBall = Cast<ADodgeBall>(this->ball);
	if (!dodgeBall) return;

	dodgeBall->controllingSkillType = ECharacterSkillType::Giant;

	state = EGiantState::PreGiant;
	ballInitScale = dodgeBall->sphereComp->GetRelativeScale3D();
}

void AGigantSkill::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
}
