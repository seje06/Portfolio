#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloneBall.generated.h"

class USphereComponent;

// 캐릭터를 맞췄을 때 호출될 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCloneBallHit, AActor*);

UCLASS()
class CREAZYDODGEBALL_API ACloneBall : public AActor
{
	GENERATED_BODY()
	
public:	
	ACloneBall();

	virtual void Tick(float DeltaTime) override;


protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* _hitComp, AActor* _otherActor,
		UPrimitiveComponent* _otherComp, FVector _normalImpulse, const FHitResult& _hit);
public:	
	// 분신공이 캐릭터를 맞췄을 때 외부에 알리는 델리게이트
	FOnCloneBallHit OnCloneBallHit;

	// 충돌을 담당하는 스피어 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* sphereComp;

	// 시각 표현용 메쉬 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* meshComp;

	bool bIsGrounded = false;

};
