#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloneBall.generated.h"

class USphereComponent;

// ĳ���͸� ������ �� ȣ��� ��������Ʈ
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
	// �нŰ��� ĳ���͸� ������ �� �ܺο� �˸��� ��������Ʈ
	FOnCloneBallHit OnCloneBallHit;

	// �浹�� ����ϴ� ���Ǿ� �ݸ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* sphereComp;

	// �ð� ǥ���� �޽� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* meshComp;

	bool bIsGrounded = false;

};
