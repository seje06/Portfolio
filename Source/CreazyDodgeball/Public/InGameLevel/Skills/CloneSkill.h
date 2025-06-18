#pragma once

#include "CoreMinimal.h"
#include "InGameLevel/Skills/Skill.h"
#include "CloneSkill.generated.h"

class ACloneBall;

UENUM(BlueprintType)
enum class ECloneState : uint8
{
	PreClone,
	DoClone,
	EndClone
};

UCLASS()
class CREAZYDODGEBALL_API ACloneSkill : public ASkill
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;
	virtual void ActiveSkill(AActor* ball) override;

protected:
	virtual void BeginPlay() override;


private:
	// 실제 분신을 생성하는 함수
	void SpawnClones();

	// 모든 분신공 삭제
	void DestroyAllClones();

	UPROPERTY(EditDefaultsOnly, Category = "Clone")
	TSubclassOf<class AActor> cloneBallClass;

	ECloneState cloneState = ECloneState::PreClone;

	// 분신공 리스트
	UPROPERTY()
	TArray<ACloneBall*> cloneBalls;

	ACloneBall* leftClone = nullptr;
	ACloneBall* rightClone = nullptr;
};
