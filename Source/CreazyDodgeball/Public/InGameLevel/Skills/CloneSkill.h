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
	// ���� �н��� �����ϴ� �Լ�
	void SpawnClones();

	// ��� �нŰ� ����
	void DestroyAllClones();

	UPROPERTY(EditDefaultsOnly, Category = "Clone")
	TSubclassOf<class AActor> cloneBallClass;

	ECloneState cloneState = ECloneState::PreClone;

	// �нŰ� ����Ʈ
	UPROPERTY()
	TArray<ACloneBall*> cloneBalls;

	ACloneBall* leftClone = nullptr;
	ACloneBall* rightClone = nullptr;
};
