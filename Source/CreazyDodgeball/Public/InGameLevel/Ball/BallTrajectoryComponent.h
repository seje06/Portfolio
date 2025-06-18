#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BallTrajectoryComponent.generated.h"


class USplineComponent;
class USplineMeshComponent;
class UStaticMesh;
class UMaterialInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CREAZYDODGEBALL_API UBallTrajectoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBallTrajectoryComponent();

	UFUNCTION()
	void DrawPredictedTrajectory(const FVector& _startLocation, const FVector& _velocity);

	UFUNCTION()
	void ClearTrajectory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	USplineComponent* trajectorySpline;

	UPROPERTY()
	TArray<USplineMeshComponent*> splineMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Trajectory")
	UStaticMesh* trajectoryMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Trajectory")
	UMaterialInterface* trajectoryMaterial = nullptr;

	// ���� ���� �޽�
	UPROPERTY(EditAnywhere, Category = "Trajectory|Impact")
	UStaticMesh* impactMesh = nullptr;

	// ���� ���� ��Ƽ����
	UPROPERTY(EditAnywhere, Category = "Trajectory|Impact")
	UMaterialInterface* impactMaterial = nullptr;

	// ���� ���� �޽�������Ʈ
	UPROPERTY()
	UStaticMeshComponent* impactMarker = nullptr;

	// ��������
	UPROPERTY()
	FVector impactLocation;
};
