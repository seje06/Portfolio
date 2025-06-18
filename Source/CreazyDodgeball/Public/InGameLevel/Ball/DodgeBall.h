#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Characters/CharacterEnums.h"
#include "DodgeBall.generated.h"

class UStaticMeshComponent;
class UBallTrajectoryComponent;
class USphereComponent;



// 델리게이트 타입 선언
DECLARE_MULTICAST_DELEGATE(FDodgeBallDelegate)
//DECLARE_MULTICAST_DELEGATE_OneParam(FDodgeBallDelegate_OneParam, class ADodgeballCharacter*);

UCLASS()
class CREAZYDODGEBALL_API ADodgeBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADodgeBall();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//발사 함수
	UFUNCTION(BlueprintCallable)
	void LaunchBall(FVector _direction, float _power);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor,
		UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
		UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

private:
	void TraceFloorUnderBall(); // 트레이스 함수 선언

public:	
	//볼 매쉬 생성
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ballMeshComp;

	UPROPERTY(VisibleAnywhere)
	UBallTrajectoryComponent* ballTrajectortyComp;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* sphereComp;

	UPROPERTY()
	TArray<AActor*> hitCharacters;

	//최근에 공을 던진 액터
	AActor* recentThrownCharacter;

	// 발사 시작 지점 위치
	UPROPERTY()
	FVector launchStartLocation;

	ECharacterSkillType controllingSkillType = ECharacterSkillType::None;
	
	//델리게이트
	FDodgeBallDelegate OnCatched;
	FDodgeBallDelegate OnGrounded;

	bool bIsGrounded = false;
	bool bIsCahtched = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 currentFloorNum = -1; // 현재 공이 위치한 바닥 번호 (-1: 없음)

protected:
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* hitSound;
};
