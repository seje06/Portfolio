// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAbilityComponent.generated.h"

DECLARE_DELEGATE_OneParam(PlayerAbilityCompDelegate, AActor*)

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CREAZYDODGEBALL_API UPlayerAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAbilityComponent();
	virtual void OnComponentDestroyed(bool beDestroyingHierarchy) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void OnMouseMoving(const struct FInputActionValue& value);
	void OnMouseMoveCompleted(const struct FInputActionValue& value);
	void OnMoveKeyPressed(const struct FInputActionValue& value);
	void OnMoveKeyPressUp(const struct FInputActionValue& value);

	void OnSpaceKeyPressed(const struct FInputActionValue& value);

	void OnShiftKey(const struct FInputActionValue& value);

	void OnMouseRButton(const struct FInputActionValue& value);
	void OnMouseLButton(const struct FInputActionValue& value);

	void OnAvoidKeyPressed(const struct FInputActionValue& value);

	void OnCrouchKeyPressed(const struct FInputActionValue& value);

	void OnCharacTrunKeyPressed(const struct FInputActionValue& value);

	void OnEscKeyPressed(const struct FInputActionValue& value);

	//스킬 키 누를때 1,2,3,4번
	void OnBoomerangSkillKeyPressed(const struct FInputActionValue& value);
	void OnGiantSkillKeyPressed(const struct FInputActionValue& value);
	void OnCloneSkillKeyPressed(const struct FInputActionValue& value);
	void OnFireSkillKeyPressed(const struct FInputActionValue& value);

	void OnDebugCameraKeyPressed(const struct FInputActionValue& value);
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void BindPlayerActions(class UInputComponent* PlayerInputComponent);

	bool TraceCharacterInFront(AActor*& out_SeenCharacter);
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* cameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USpringArmComponent* springArmComp;

	PlayerAbilityCompDelegate OnCheckingWatchingTeamMemberHavingBall;
	PlayerAbilityCompDelegate OnTryTurnChracter;

	UPROPERTY(EditAnywhere)
	class ACameraActor* debugCamera = nullptr;
protected:
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* iMC_Player;
	//InputActions
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Look;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Move;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_FastMove;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Jump;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_ArmActionReady;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_ArmAction;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Avoid;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Crouch;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_CharacTurn;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Esc;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* iA_Debug;

	UPROPERTY(EditDefaultsOnly)
	TArray<class UInputAction*> iA_Skills;

	class ADodgeballCharacter* ownerCharacter;
};
