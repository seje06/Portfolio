// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BallManagerComponent.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(BallManagerDelegate)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CREAZYDODGEBALL_API UBallManagerComponent : public USphereComponent
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateSensedBallLocation();
	bool AttachBallToOtherActor(AActor* owner, const FName& socketName);

	void OnBallForceDetached();


	//void OnHittedOwnerToBall(class ACharacter* owenr);

	void ReadyThrowBall(float power, FVector dir);
	void EndReadyThrowBall();

	/// <summary>
	/// 볼을 파워와 방향으로 던지고, 던지는 볼 반환.
	/// </summary>
	/// <param name="owner">볼을 던질 캐릭터</param>
	/// <param name="power">파워</param>
	/// <param name="dir">방향</param>
	/// <returns></returns>
	AActor* ThrowBall(class ACharacter* owner, float power, FVector dir);

	/// <summary>
	/// 감지된 볼 액터 반환.
	/// </summary>
	/// <returns></returns>
	AActor* GetBallActor();

	bool CanObtainBall();

	void SetBallCatchingToEasy();

	inline FVector GetSensedBallRightSideLocation() { return rightLocationOfSensedBall; }
	inline FVector GetSensedBallLeftSideLocation() { return leftLocationOfSensedBall; }

	inline bool IsBallAttachedToOwnerActor() { return isBallAttachedToOwnerActor; }
	inline bool IsExistSensedBall() { return isExistSensedBall; }

	void InitBallInfo();
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	BallManagerDelegate OnBallEscapedFromManager;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SencedInfo")
	bool isExistSensedBall = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SencedInfo")
	FVector rightLocationOfSensedBall;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SencedInfo")
	FVector leftLocationOfSensedBall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SencedInfo")
	FVector boneHandLocationL_CPP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SencedInfo")
	FVector boneHandLocationR_CPP;

private:
	AActor* sensedBallActor = nullptr;

	USphereComponent* currentSensedBallComp = nullptr;

	bool isBallAttachedToOwnerActor = false;

	FTimerHandle timerHandle;
};
