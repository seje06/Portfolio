// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterEnums.h"
#include "DodgeballCharacter.generated.h"

DECLARE_DELEGATE_OneParam(DodgeballCharacterInputDelegate, class UInputComponent*)
DECLARE_DELEGATE_OneParam(DodgeballCharacterEnergyDelegate, bool)

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Boomerang,
	Giant,
	Clone,
	Fire
};

UCLASS()
class CREAZYDODGEBALL_API ADodgeballCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADodgeballCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor,
		UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor,
		UPrimitiveComponent* _otherComp, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Look(FVector2D moveVec);
	
	void Move(FVector2D moveVec);

	void OnMoveCompleted();

	void TurnMoveMode(bool canRun);

	void TryJump();

	void DoReadyArmAction();
	void DoEndReadyArmAction();

	void DoArmAction();

	UFUNCTION(BlueprintCallable)
	void InitMovement();
	void InitLook();

	UFUNCTION(BlueprintCallable)
	AActor* Throw();

	void Crouch();

	void Avoid();

	void TrySkill(ESkillType _skillType);

	void OnHitFromBall(ECharacterSkillType skillType, FVector hitDir);

	void OnGameEnded(bool isVictory);
	
	void WarmingUp();

	UFUNCTION(BlueprintCallable)
	void InitAvoidSetting();
	UFUNCTION(BlueprintCallable)
	void LaunchOnAvoid();

	void AddEnergy(float energyToAdd);

	void FixBallManagerPosition();

	void OnDisappeared();

	void OnAppeared();

	void ForceToDetachBall();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	inline FVector GetCurrentRotation() { return currentRotation; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	void SetHeadAndArmRotation();
	void SetFitOfCapsuleAndMesh();
	void MoveOnJumping();
	void UpdateBallManagerLocation();
	void TryObtainBall();
	void UpdateBallThrowingPower();
	void OnPreparingThrowing();
	void CheckUnderFoot();
public:

	UPROPERTY(BlueprintReadOnly)
	FVector2D lookAxisVector;
	UPROPERTY(BlueprintReadWrite)
	FVector2D movementVector;

	UPROPERTY(BlueprintReadOnly)
	FVector currentRotation;

	UPROPERTY(EditAnywhere)
	float bodyRotationSpeed = 8;
	UPROPERTY(EditAnywhere)
	float bodyRotationSpeedInAir = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECharacterTeamType teamType= ECharacterTeamType::A;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterType type = ECharacterType::AI;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterMoveSpeedType moveSpeedType = ECharacterMoveSpeedType::Walk;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ECharacterActionType characterActionType = ECharacterActionType::Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ECharacterArmActionType armActionType = ECharacterArmActionType::None;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ECharacterJumpingState jumpingState = ECharacterJumpingState::None;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterFieldPositionType currentPositionType = ECharacterFieldPositionType::In;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBallManagerComponent* ballManagerCom = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* capMesh = nullptr;

	DodgeballCharacterInputDelegate onCharacterDonePlayer;
	DodgeballCharacterEnergyDelegate onEnergyChanged;

	UPROPERTY(VisibleAnywhere)
	int currentFloorNum = 1;

	UPROPERTY(EditAnywhere)
	float maxEnergy = 3;
	UPROPERTY(EditAnywhere)
	float energy = 0;

	float throwingPower = 0;


protected:
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_ReadyCatchBall;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_TurnLeg;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_Throw;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_Avoid;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_Shrugging;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_Hit;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_EndGameJesture;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* aM_WarmingUp;

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* catchingSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* dieSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* inverseDieSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* screamSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* victorySound = nullptr;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* defeatSound = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class ASkill>> skillArray;
private:
	//FVector preBallSensorOffset;
	//FVector nonModifyBallSensorLocation;

	FVector ballManagerOffsetFromFixHead;

	FTransform fixHeadTrs;

	int recentHorizionMoveDir = 1;

	float disSolveValue=1.5f;
	class UMaterialInstanceDynamic* Mat = nullptr;
	class UMaterialInstanceDynamic* Mat2 = nullptr;
	class UMaterialInstanceDynamic* CapMat = nullptr;
	//죽을때 실행할 타이머에 대한 핸들
	FTimerHandle dissolveTick;
};
