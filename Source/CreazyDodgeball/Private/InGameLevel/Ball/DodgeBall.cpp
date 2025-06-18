
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

	//스피어 생성 & 루트 설정
	sphereComp= CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = sphereComp;
	//크기 & 물리적용
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

	// 공메쉬 생성
	ballMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMeshComp"));
	
	ballMeshComp->SetSimulatePhysics(false); // ProjectileMovement 쓸 거면 false
	ballMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ballMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	ballMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	/*ballMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ballMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel1, ECollisionResponse::ECR_Ignore);*/
	//ballMeshComp->SetNotifyRigidBodyCollision(true); // OnHit을 위해
	ballMeshComp->SetupAttachment(sphereComp);
	ballMeshComp->SetRelativeScale3D(FVector(1.2, 1.2, 1.2));
	ballMeshComp->SetRelativeLocation(FVector(0, 0, sphereComp->GetUnscaledSphereRadius()));

	ballTrajectortyComp = CreateDefaultSubobject<UBallTrajectoryComponent>(TEXT("BallUIComp"));
}

void ADodgeBall::LaunchBall(FVector _direction, float _power)
{
	//현재 공잡은 캐릭터 초기화
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	gameState->currentCatchingBallCharac = nullptr;

	//시작위치 저장
	launchStartLocation = GetActorLocation();

	//hitCharacters 배열 초기화, 추후 심판캐릭터 생성시 삭제 예정
	hitCharacters.Empty();

	if (!sphereComp)
	{
		return;
	}

	//물리적용
	sphereComp->SetSimulatePhysics(true);

	// 기존 물리 속도 제거
	sphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	FVector _velocity = _direction.GetSafeNormal() * _power;
	sphereComp->AddImpulse(_velocity*7.5f); // 발사

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

	//땅에 닿았을 때, 그리고 복제 볼이 아닌 원본 볼일 때
	if (_otherActor->ActorHasTag("Ground")&& hitCompOwner)
	{
		sphereComp->SetPhysicsLinearVelocity(FVector(sphereComp->GetPhysicsLinearVelocity().X, sphereComp->GetPhysicsLinearVelocity().Y, 0));
		bIsGrounded = true;
		TraceFloorUnderBall();
		if(OnGrounded.IsBound()) OnGrounded.Broadcast();
	}
	//if (_otherActor->ActorHasTag("Ground")) UE_LOG(LogTemp, Warning, TEXT("ground!!!!!!!!!!!!!"));

	//캐릭터에 닿았을 때
	auto character = Cast<ADodgeballCharacter>(_otherActor);
	if (character)
	{
		//땅에 있지 않다면
		if (!bIsGrounded)
		{
			//중복피격 방지
			if (!hitCharacters.Contains(_otherActor))
			{
				//던진애가 있고, 맞은애랑 같지 않다면 추가
				if (recentThrownCharacter && Cast<ADodgeballCharacter>(recentThrownCharacter)->teamType != character->teamType)
				{
					hitCharacters.Add(_otherActor);
					character->OnHitFromBall(controllingSkillType, (character->GetActorLocation() - GetActorLocation()).GetSafeNormal());
	
					OnCatched.RemoveAll(character->GetController());
					OnGrounded.RemoveAll(character->GetController());
				}
			}
		}
		//땅에 있다면
		else
		{
			//현재속도 가져오기
			FVector currentVelocity = sphereComp->GetPhysicsLinearVelocity();

			//x,y 만 0으로 만들고 z는 유지
			FVector adjustedVelocity = FVector(0.0f, 0.0f, -100.f);

			sphereComp->SetPhysicsLinearVelocity(adjustedVelocity);
			sphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector); // 회전 정지
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
	// 트레이스 시작 위치 = 공 위치
	FVector start = GetActorLocation();
	FVector end = start + FVector(0, 0, -sphereComp->GetUnscaledSphereRadius()*0.25f -100); // 아래로 충분히 긴 거리

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this); // 자기 자신 무시

	// 감지 대상: WorldStatic만 설정
	FCollisionObjectQueryParams objectQueryParams;
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); // 일반적인 바닥이나 벽 오브젝트는 WorldStatic

	// 트레이스 실행
	bool bHit = GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, objectQueryParams, queryParams);

	if (bHit && hitResult.GetActor())
	{
		auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

		if (hitResult.GetActor()->Tags.Num() == 2)
		{
			for (auto& tag : hitResult.GetActor()->Tags)
			{
				if (tag.ToString().Contains(TEXT("Plane"))) //내야일떄
				{
					FString tagStr = tag.ToString();
					FString numberPart = tagStr.RightChop(5); // Plane1 → 1
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
		else if (hitResult.GetActor()->Tags.Num() == 1 && hitResult.GetActor()->Tags[0].ToString().Contains(TEXT("Ground")) && !hitResult.GetActor()->Tags[0].ToString().Contains(TEXT("Plane")))//바깥으로 나갔을때
		{
			//floor랑 외부 땅이랑 겹치는거 때문인지 얘도 계속 호출함. 겹치지않게 맵 구성해야할듯
			//UE_LOG(LogTemp, Warning, TEXT("team type none!"));
			gameState->currentHavingTurnTeamType = ECharacterTeamType::None;
			gameState->currentBallPositionType = EBallPositionType::Notting;
		}
		
	}

	// 아무 것도 감지 못한 경우
	currentFloorNum = -1;
}

// Called every frame
void ADodgeBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceFloorUnderBall();
}

