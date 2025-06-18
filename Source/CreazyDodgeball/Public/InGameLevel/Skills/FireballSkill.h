#pragma once

#include "CoreMinimal.h"
#include "InGameLevel/Skills/Skill.h"
#include "FireballSkill.generated.h"

UENUM(BlueprintType)
enum class EFireState : uint8
{
	BeforeBoost,
	Boosting,
	EndBoost
};

UCLASS()
class CREAZYDODGEBALL_API AFireballSkill : public ASkill
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime)	override;
	virtual void ActiveSkill(AActor* _ball)	override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* nS_Fire;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* fireSound;
private:
	EFireState fireState = EFireState::BeforeBoost;
	
	class UNiagaraComponent* nC_Fire;
	float preMass;
};
