// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Ball/BallManagerComponent.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "GameFramework/Character.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "InGameLevel/DodgeballGameState.h"
#include "Components/CapsuleComponent.h"

//public
void UBallManagerComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//공인지 확인
	auto sphereCom = Cast<USphereComponent>(OtherComp);
	if (!sphereCom) return;
	if (isBallAttachedToOwnerActor) return;
	if (!OtherActor && !Cast<ADodgeBall>(OtherActor)) return;

	if (auto ballAttachComp = OtherActor->GetRootComponent()->GetAttachParent())
	{
		auto ownerCharacter = Cast<ACharacter>(ballAttachComp->GetOwner());
		if (ownerCharacter) return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Sense Ball!"));

	sensedBallActor = OtherActor;
	currentSensedBallComp = sphereCom;
	isExistSensedBall = true;

	UpdateSensedBallLocation();
}

void UBallManagerComponent::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto sphereCom = Cast<USphereComponent>(OtherComp);
	if (!sphereCom) return;
	if (isBallAttachedToOwnerActor) return;

	OnBallEscapedFromManager.ExecuteIfBound();
	OnBallEscapedFromManager.Unbind();

	sensedBallActor = nullptr;
	currentSensedBallComp = nullptr;
	isExistSensedBall = false;
}

void UBallManagerComponent::UpdateSensedBallLocation()
{
	if (!isExistSensedBall) return;
	if (!currentSensedBallComp) return;
	if (!sensedBallActor) return;

	auto ownerCharacter = Cast<ACharacter>(GetOwner());
	if (!ownerCharacter) return;

	auto ownerMesh = ownerCharacter->GetMesh();
	FTransform ownerMeshTrs = ownerMesh->GetComponentTransform();
	
	if (!isBallAttachedToOwnerActor)
	{
		rightLocationOfSensedBall = currentSensedBallComp->GetComponentLocation() + GetRightVector() * currentSensedBallComp->GetUnscaledSphereRadius() * 0.25f;
		leftLocationOfSensedBall = currentSensedBallComp->GetComponentLocation() + GetRightVector() * -1 * currentSensedBallComp->GetUnscaledSphereRadius() * 0.25f;
	}
	else
	{
		rightLocationOfSensedBall = GetComponentLocation() + GetRightVector() * currentSensedBallComp->GetUnscaledSphereRadius() * 0.25f;
		leftLocationOfSensedBall = GetComponentLocation() + GetRightVector() * -1 * currentSensedBallComp->GetUnscaledSphereRadius() * 0.25f;
	}

	boneHandLocationR_CPP = ownerMesh->GetBoneLocation(TEXT("hand_r"));
	boneHandLocationL_CPP = ownerMesh->GetBoneLocation(TEXT("hand_l"));


}

bool UBallManagerComponent::AttachBallToOtherActor(AActor* owner, const FName& socketName)
{
	if (!owner) return false;
	if (!sensedBallActor) return false;

	auto ballAttachComp = sensedBallActor->GetRootComponent()->GetAttachParent();
	if (ballAttachComp) return false;

	//스킬에 의해 제어 되고 있는중이면 리턴
	auto dodgeball=Cast<ADodgeBall>(sensedBallActor);
	if (!dodgeball) return false;
	if (dodgeball->controllingSkillType != ECharacterSkillType::None) return false;

	currentSensedBallComp->SetSimulatePhysics(false);

	FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, true);
	if (currentSensedBallComp->AttachToComponent(Cast<ACharacter>(owner)->GetMesh(), rules, socketName))
	{
		
		sensedBallActor->GetRootComponent()->AttachToComponent(Cast<ACharacter>(owner)->GetMesh(), rules, socketName);
		currentSensedBallComp->SetRelativeLocation(FVector(-2, currentSensedBallComp->GetUnscaledSphereRadius() * 0.25f, 3));
		UE_LOG(LogTemp, Warning, TEXT("succesed Attaching"));
		isBallAttachedToOwnerActor = true;

		auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
		gameState->currentCatchingBallCharac = Cast<ACharacter>(GetOwner());
;		dodgeball->bIsGrounded = false;
		dodgeball->bIsCahtched = true;
		if(dodgeball->OnCatched.IsBound()) dodgeball->OnCatched.Broadcast();

		

		UpdateSensedBallLocation();

		OnBallEscapedFromManager.Unbind();
		return true;
	}
	else
	{
		currentSensedBallComp->SetSimulatePhysics(true);
		return false;
	}

}

void UBallManagerComponent::OnBallForceDetached()
{
	if (!sensedBallActor) return;

	sensedBallActor->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Cast<ADodgeBall>(sensedBallActor)->sphereComp->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	Cast<ADodgeBall>(sensedBallActor)->sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	Cast<ADodgeBall>(sensedBallActor)->sphereComp->SetSimulatePhysics(true);
	Cast<ADodgeBall>(sensedBallActor)->bIsCahtched = false;

	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->recentAimedCharac = nullptr;
	InitBallInfo();
}

//void UBallManagerComponent::OnHittedOwnerToBall(ACharacter* owner)
//{
//	if (!owner) return;
//
//	auto dodgeball=Cast<ADodgeBall>(Cast<ADodgeballGameState>(GetWorld()->GetGameState())->ball);
//	dodgeball->OnCatched.RemoveAll(owner->GetController());
//	dodgeball->OnGrounded.RemoveAll(owner->GetController());
//}

void UBallManagerComponent::ReadyThrowBall(float power, FVector dir)
{
	if (currentSensedBallComp == nullptr) return;
	if (auto ball = Cast<ADodgeBall>(sensedBallActor))
	{
		//궤도 설정
		ball->ballTrajectortyComp->ClearTrajectory();
		ball->ballTrajectortyComp->DrawPredictedTrajectory(currentSensedBallComp->GetComponentLocation(), power * dir);
	}
}

void UBallManagerComponent::EndReadyThrowBall()
{
	if (currentSensedBallComp == nullptr) return;
	if (auto ball = Cast<ADodgeBall>(sensedBallActor))
	{
		ball->ballTrajectortyComp->ClearTrajectory();
	}
}

AActor* UBallManagerComponent::ThrowBall(ACharacter* owner, float power, FVector dir)
{
	if (!owner) return nullptr;
	if (GetOwner() != owner) return nullptr;
	if (currentSensedBallComp == nullptr) return nullptr;

	sensedBallActor->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	currentSensedBallComp->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	currentSensedBallComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
	/*currentSensedBallComp->SetSimulatePhysics(true);
	currentSensedBallComp->AddImpulse(dir * power);*/

	if (auto ball = Cast<ADodgeBall>(sensedBallActor))
	{
		ball->recentThrownCharacter = owner;
		ball->bIsCahtched = false;
		//auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
		
		ball->LaunchBall(dir, power);
		ball->ballTrajectortyComp->DrawPredictedTrajectory(currentSensedBallComp->GetComponentLocation(), power * dir);
		
		ball->GetWorldTimerManager().SetTimer(timerHandle, ball->ballTrajectortyComp ,&UBallTrajectoryComponent::ClearTrajectory, 1,false);
	}

	AActor* launchedBall = sensedBallActor;

	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->currentCatchingBallCharac =nullptr;
	
	auto aimedCharac=gameState->GetProximateEnemyToLocation(Cast<ACharacter>(GetOwner()), Cast<ADodgeBall>(sensedBallActor)->ballTrajectortyComp->impactLocation);
	if (FVector::Dist(Cast<ADodgeBall>(sensedBallActor)->ballTrajectortyComp->impactLocation, aimedCharac->GetActorLocation()) <
		owner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2)
	{
		gameState->recentAimedCharac = aimedCharac;
	}
	else
	{
		gameState->recentAimedCharac = nullptr;
	}
	//gameState->recentAimedCharac = aimedCharac;

	isExistSensedBall = false;
	sensedBallActor = nullptr;
	currentSensedBallComp = nullptr;
	isBallAttachedToOwnerActor = false;

	

	return launchedBall;
}

AActor* UBallManagerComponent::GetBallActor()
{
	return sensedBallActor;
}
bool UBallManagerComponent::CanObtainBall()
{
	if (!sensedBallActor) return false;
	if (!isExistSensedBall) return false;
	if (isBallAttachedToOwnerActor) return false;
	if (auto dodgeball = Cast<ADodgeBall>(sensedBallActor))
	{
		if (dodgeball->controllingSkillType != ECharacterSkillType::None) return false;
	}
	else return false;


	return true;
}
void UBallManagerComponent::SetBallCatchingToEasy()
{
	if (!CanObtainBall()) return;

	//currentSensedBallComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	currentSensedBallComp->SetPhysicsLinearVelocity(FVector(currentSensedBallComp->GetPhysicsLinearVelocity().X / 5.f, currentSensedBallComp->GetPhysicsLinearVelocity().Y / 5.f, currentSensedBallComp->GetPhysicsLinearVelocity().Z / 2.f));
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, FString::Printf(TEXT("ballSlowSlow")));
}
//

void UBallManagerComponent::InitBallInfo()
{
	isExistSensedBall = false;
	sensedBallActor = nullptr;
	currentSensedBallComp = nullptr;
	isBallAttachedToOwnerActor = false;
}

void UBallManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	//InitSphereRadius(16);
	SetGenerateOverlapEvents(true);

	OnComponentBeginOverlap.AddDynamic(this, &UBallManagerComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBallManagerComponent::OnSphereEndOverlap);
}


void UBallManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}