
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/Ball/BallTrajectoryComponent.h"
#include "InGameLevel/DodgeballGameState.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/DodgeballGameState.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADodgeBall::ADodgeBall()
{
 	//Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���Ǿ� ���� & ��Ʈ ����
	sphereComp= CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = sphereComp;
	//ũ�� & ��������
	sphereComp->SetSphereRadius(60);
	sphereComp->SetRelativeScale3D(FVector(0.25, 0.25, 0.25));
	sphereComp->SetSimulatePhysics(true);
	sphereComp->SetNotifyRigidBodyCollision(true);
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	sphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	/*sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block); 
	sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel1, ECollisionResponse::ECR_Block);*/

	//OnHit
	sphereComp->OnComponentHit.AddDynamic(this, &ADodgeBall::OnHit);

	//OnBeginOverlap
	sphereComp->OnComponentBeginOverlap.AddDynamic(this, &ADodgeBall::OnBeginOverlap);

	// ���޽� ����
	ballMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMeshComp"));
	
	ballMeshComp->SetSimulatePhysics(false); // ProjectileMovement �� �Ÿ� false
	ballMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ballMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	ballMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	/*ballMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ballMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel1, ECollisionResponse::ECR_Ignore);*/
	//ballMeshComp->SetNotifyRigidBodyCollision(true); // OnHit�� ����
	ballMeshComp->SetupAttachment(sphereComp);
	ballMeshComp->SetRelativeScale3D(FVector(1.2, 1.2, 1.2));
	ballMeshComp->SetRelativeLocation(FVector(0, 0, sphereComp->GetUnscaledSphereRadius()));

	ballTrajectortyComp = CreateDefaultSubobject<UBallTrajectoryComponent>(TEXT("BallUIComp"));
}

void ADodgeBall::LaunchBall(FVector _direction, float _power)
{
	//���� ������ ĳ���� �ʱ�ȭ
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->currentCatchingBallCharac = nullptr;

	//������ġ ����
	launchStartLocation = GetActorLocation();

	//hitCharacters �迭 �ʱ�ȭ, ���� ����ĳ���� ������ ���� ����
	hitCharacters.Empty();

	if (!sphereComp)
	{
		return;
	}

	//��������
	sphereComp->SetSimulatePhysics(true);

	// ���� ���� �ӵ� ����
	sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	FVector _velocity = _direction.GetSafeNormal() * _power;
	sphereComp->AddImpulse(_velocity*7.5f); // �߻�

	UE_LOG(LogTemp, Warning, TEXT("Ball Impulse Fired!"));
}

// Called when the game starts or when spawned
void ADodgeBall::BeginPlay()
{
	Super::BeginPlay();
	
}
void ADodgeBall::OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor,
	UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit)
{
	auto hitCompOwner = Cast<ADodgeBall>(_hitComp->GetOwner());

	if(!bIsGrounded && !bIsCahtched && hitCompOwner)
	UGameplayStatics::PlaySoundAtLocation(this, hitSound, GetActorLocation(), 0.001f * sphereComp->GetPhysicsLinearVelocity().Size());

	if (!_otherActor || _otherActor == this) return;
	if (bIsCahtched) return;

	sphereComp->SetEnableGravity(true);

	//���� ����� ��, �׸��� ���� ���� �ƴ� ���� ���� ��
	if (_otherActor->ActorHasTag("Ground")&& hitCompOwner)
	{
		sphereComp->SetPhysicsLinearVelocity(FVector(sphereComp->GetPhysicsLinearVelocity().X, sphereComp->GetPhysicsLinearVelocity().Y, 0));
		bIsGrounded = true;
		TraceFloorUnderBall();
		if(OnGrounded.IsBound()) OnGrounded.Broadcast();
	}
	//if (_otherActor->ActorHasTag("Ground")) UE_LOG(LogTemp, Warning, TEXT("ground!!!!!!!!!!!!!"));

	//ĳ���Ϳ� ����� ��
	auto character = Cast<ADodgeballCharacter>(_otherActor);
	if (character)
	{
		//���� ���� �ʴٸ�
		if (!bIsGrounded)
		{
			//�ߺ��ǰ� ����
			if (!hitCharacters.Contains(_otherActor))
			{
				//�����ְ� �ְ�, �����ֶ� ���� �ʴٸ� �߰�
				if (recentThrownCharacter && Cast<ADodgeballCharacter>(recentThrownCharacter)->teamType != character->teamType)
				{
					hitCharacters.Add(_otherActor);
					character->OnHitFromBall(controllingSkillType, (character->GetActorLocation() - GetActorLocation()).GetSafeNormal());
	
					OnCatched.RemoveAll(character->GetController());
					OnGrounded.RemoveAll(character->GetController());
				}
			}
		}
		//���� �ִٸ�
		else
		{
			//����ӵ� ��������
			FVector currentVelocity = sphereComp->GetPhysicsLinearVelocity();

			//x,y �� 0���� ����� z�� ����
			FVector adjustedVelocity = FVector(0.0f, 0.0f, -100.f);

			sphereComp->SetPhysicsLinearVelocity(adjustedVelocity);
			sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector); // ȸ�� ����
			sphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
		}
	}
}

void ADodgeBall::OnBeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (controllingSkillType != ECharacterSkillType::Fire) return;
	if (bIsCahtched) return;

	auto* character = Cast<ADodgeballCharacter>(_otherActor);
	if (!character) return;

	if (hitCharacters.Contains(character)) return;

	if (recentThrownCharacter && Cast<ADodgeballCharacter>(recentThrownCharacter)->teamType!= character->teamType)
	{
		hitCharacters.Add(_otherActor);
		character->OnHitFromBall(controllingSkillType, (character->GetActorLocation() - GetActorLocation()).GetSafeNormal());
		UE_LOG(LogTemp, Warning, TEXT("Fireball pierced: %s"), *_otherActor->GetName());
	}
}

void ADodgeBall::TraceFloorUnderBall()
{
	if (!bIsGrounded) return;
	if (bIsCahtched) return;

	FHitResult hitResult;
	// Ʈ���̽� ���� ��ġ = �� ��ġ
	FVector start = GetActorLocation();
	FVector end = start + FVector(0, 0, -sphereComp->GetUnscaledSphereRadius()*0.25f -100); // �Ʒ��� ����� �� �Ÿ�

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this); // �ڱ� �ڽ� ����

	// ���� ���: WorldStatic�� ����
	FCollisionObjectQueryParams objectQueryParams;
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); // �Ϲ����� �ٴ��̳� �� ������Ʈ�� WorldStatic

	// Ʈ���̽� ����
	bool bHit = GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, objectQueryParams, queryParams);

	if (bHit && hitResult.GetActor())
	{
		auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

		if (hitResult.GetActor()->Tags.Num() == 2)
		{
			for (auto& tag : hitResult.GetActor()->Tags)
			{
				if (tag.ToString().Contains(TEXT("Plane"))) //�����ϋ�
				{
					FString tagStr = tag.ToString();
					FString numberPart = tagStr.RightChop(5); // Plane1 �� 1
					currentFloorNum = FCString::Atoi(*numberPart);

					if (currentFloorNum > gameState->floorCountOfOneTeam)
					{
						//gameState->currentHavingBallTeamType = ECharacterTeamType::B;
						gameState->currentHavingTurnTeamType = ECharacterTeamType::B;
						gameState->currentBallPositionType = EBallPositionType::In;
					}
					else
					{
						//gameState->currentHavingBallTeamType = ECharacterTeamType::A;
						gameState->currentHavingTurnTeamType = ECharacterTeamType::A;
						gameState->currentBallPositionType = EBallPositionType::In;
					}

					//UE_LOG(LogTemp, Warning, TEXT("[Ball Floor Trace] Floor: %d (Hit Actor: %s)"), currentFloorNum, *hitResult.GetActor()->GetName());
					return;
				}
			}
		}
		else if (hitResult.GetActor()->Tags.Num() == 1 && hitResult.GetActor()->Tags[0].ToString().Contains(TEXT("Ground")) && !hitResult.GetActor()->Tags[0].ToString().Contains(TEXT("Plane")))//�ٱ����� ��������
		{
			//floor�� �ܺ� ���̶� ��ġ�°� �������� �굵 ��� ȣ����. ��ġ���ʰ� �� �����ؾ��ҵ�
			//UE_LOG(LogTemp, Warning, TEXT("team type none!"));
			gameState->currentHavingTurnTeamType = ECharacterTeamType::None;
			gameState->currentBallPositionType = EBallPositionType::Notting;
		}
		
	}

	// �ƹ� �͵� ���� ���� ���
	currentFloorNum = -1;
}

// Called every frame
void ADodgeBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceFloorUnderBall();
}

