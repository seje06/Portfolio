#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Characters/CharacterEnums.h"
#include "DodgeBall.generated.h"

class UStaticMeshComponent;
class UBallTrajectoryComponent;
class USphereComponent;



// ��������Ʈ Ÿ�� ����
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

	//�߻� �Լ�
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
	void TraceFloorUnderBall(); // Ʈ���̽� �Լ� ����

public:	
	//�� �Ž� ����
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ballMeshComp;

	UPROPERTY(VisibleAnywhere)
	UBallTrajectoryComponent* ballTrajectortyComp;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* sphereComp;

	UPROPERTY()
	TArray<AActor*> hitCharacters;

	//�ֱٿ� ���� ���� ����
	AActor* recentThrownCharacter;

	// �߻� ���� ���� ��ġ
	UPROPERTY()
	FVector launchStartLocation;

	ECharacterSkillType controllingSkillType = ECharacterSkillType::None;
	
	//��������Ʈ
	FDodgeBallDelegate OnCatched;
	FDodgeBallDelegate OnGrounded;

	bool bIsGrounded = false;
	bool bIsCahtched = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 currentFloorNum = -1; // ���� ���� ��ġ�� �ٴ� ��ȣ (-1: ����)

protected:
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* hitSound;
};
