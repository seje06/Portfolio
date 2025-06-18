#include "InGameLevel/Skills/FireballSkill.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
 
void AFireballSkill::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
}

void AFireballSkill::ActiveSkill(AActor* _ball)
{
	if (!_ball) return;
	this->ball = _ball;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(this->ball);
	if (!dodgeBall) return;

	dodgeBall->controllingSkillType = ECharacterSkillType::Fire;

	fireState = EFireState::BeforeBoost;
}


void AFireballSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ball)return;

	ADodgeBall* dodgeBall = Cast<ADodgeBall>(ball);
	if (!dodgeBall) return;

	//거리 계산
	float goalOffsetDis = FVector::Dist(dodgeBall->GetActorLocation(), dodgeBall->ballTrajectortyComp->impactLocation);
	float totalDistance = FVector::Dist(dodgeBall->launchStartLocation, dodgeBall->ballTrajectortyComp->impactLocation);


	switch (fireState)
	{
	case EFireState::BeforeBoost:
	{
		if (goalOffsetDis < totalDistance * 0.8f || dodgeBall->hitCharacters.Num() > 0 || dodgeBall->bIsGrounded)
		{
			fireState = EFireState::Boosting;
			// 캐릭터와는 Overlap으로 처리
			dodgeBall->sphereComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

			UE_LOG(LogTemp, Warning, TEXT("Character Response at Fireball start: %d"),
				(int32)dodgeBall->sphereComp->GetCollisionResponseToChannel(ECC_PhysicsBody));
			
			//nC_Fire->SetRelativeRotation(nC_Fire->GetRelativeRotation() + FRotator(180, 50, 180));
			UE_LOG(LogTemp, Warning, TEXT("FireBall Boost Start"));
		}
		break;
	}
	case EFireState::Boosting:
	{
		// 현재 속도 기준 가속 적용
		//FVector currentVelocity = dodgeBall->sphereComp->GetPhysicsLinearVelocity();

		//if (currentVelocity.Size() > 1.0f)
		
		FVector boostedVelocity = (dodgeBall->ballTrajectortyComp->impactLocation-dodgeBall->GetActorLocation()).GetSafeNormal() * 2500.0f;
		//dodgeBall->sphereComp->SetAllPhysicsLinearVelocity(dodgeBall->sphereComp->GetPhysicsLinearVelocity()/10);
		dodgeBall->sphereComp->SetAllPhysicsLinearVelocity(FVector::Zero());
		dodgeBall->sphereComp->SetEnableGravity(false);
		preMass = dodgeBall->sphereComp->GetMassScale();
		//dodgeBall->sphereComp->SetMassScale(NAME_None,0.001f);
		dodgeBall->sphereComp->SetPhysicsLinearVelocity(boostedVelocity);
		
		//이펙트 시작
		FRotator fireRotator = (dodgeBall->sphereComp->GetPhysicsLinearVelocity().GetSafeNormal()).ToOrientationRotator();
		FTransform fireTrs;
		fireTrs.SetRotation(FQuat(fireRotator));
		fireRotator = fireTrs.TransformRotation(FQuat(FRotator(0,0,45))).Rotator();
		nC_Fire = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), nS_Fire, dodgeBall->GetActorLocation(), fireRotator, FVector(1, 1, 1), false);
		fireState = EFireState::EndBoost;
		
		//사운드
		UGameplayStatics::PlaySoundAtLocation(this, fireSound, ball->GetActorLocation());

		break;
	}
	case EFireState::EndBoost:
		if (dodgeBall->bIsGrounded || dodgeBall->bIsCahtched)
		{
			if (nC_Fire)nC_Fire->DestroyComponent();
			UE_LOG(LogTemp, Warning, TEXT("fire is destroy"));
			//dodgeBall->sphereComp->SetMassScale(NAME_None,preMass);
			dodgeBall->sphereComp->SetEnableGravity(true);
			/*dodgeBall->sphereComp->SetAllPhysicsLinearVelocity(FVector::Zero());
			dodgeBall->sphereComp->SetAllPhysicsAngularVelocityInRadians(FVector::Zero());
			dodgeBall->sphereComp->SetAllPhysicsAngularVelocityInDegrees(FVector::Zero());
			dodgeBall->sphereComp->SetPhysicsLinearVelocity(FVector(0, 0, -1), true);*/
			dodgeBall->controllingSkillType = ECharacterSkillType::None;
			dodgeBall->sphereComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block); // 복구
			Destroy();//스킬 종료
		}
		else
		{
			if (nC_Fire)
			{
				nC_Fire->SetWorldLocation(dodgeBall->GetActorLocation());
				//nC_Fire->SetRelativeRotation(nC_Fire->GetRelativeRotation() + FRotator(180, 50, 180));
			}
		}
		break;
	}

}

