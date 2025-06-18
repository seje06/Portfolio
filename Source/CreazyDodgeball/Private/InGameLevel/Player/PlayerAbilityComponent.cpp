// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Player/PlayerAbilityComponent.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "InGameLevel/InGameGameMode/InGameGameMode.h"
#include "InGameLevel/InGameGameMode/InGameUIComponent.h"

// Sets default values for this component's properties
UPlayerAbilityComponent::UPlayerAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	if (GetOuter())
	{
		ownerCharacter = Cast<ADodgeballCharacter>(GetOuter());
		if (ownerCharacter)
		{
			ownerCharacter->onCharacterDonePlayer.BindUObject(this, &UPlayerAbilityComponent::BindPlayerActions);
			ownerCharacter->type = ECharacterType::Player;;

			//카메라 붙힘
			//springArmComp = NewObject<USpringArmComponent>(this,TEXT("springArmComp"));
			//springArmComp->RegisterComponent();
			FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
			//springArmComp->AttachToComponent(ownerCharacter->GetMesh(), rules, TEXT("HeadSocket"));
			cameraComp = NewObject<UCameraComponent>(ownerCharacter, TEXT("cameraComp"));
			cameraComp->RegisterComponent();
			//cameraComp->SetupAttachment(ownerCharacter->GetMesh());
			//cameraComp->AttachToComponent(ownerCharacter->GetMesh(), rules, TEXT("HeadSocket"));
			//UE_LOG(LogTemp, Warning, TEXT("Success Attach Camera"));
		}
	}
	// ...
}

void UPlayerAbilityComponent::OnComponentDestroyed(bool beDestroyingHierarchy)
{
	Super::OnComponentDestroyed(beDestroyingHierarchy);

	ownerCharacter->onCharacterDonePlayer.Unbind();
	cameraComp->DestroyComponent();
}


// Called when the game starts
void UPlayerAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
	cameraComp->AttachToComponent(ownerCharacter->GetMesh(), rules, TEXT("HeadSocket"));
	cameraComp->SetRelativeLocation(FVector(4, 0, 10));
	ownerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2,ECollisionResponse::ECR_Block);
	// ...
	
}

void UPlayerAbilityComponent::BindPlayerActions(UInputComponent* PlayerInputComponent)
{
	auto enhacedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!enhacedInputComp) return;

	if (auto playerController = Cast<APlayerController>(ownerCharacter->GetController()))
	{
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer())->AddMappingContext(iMC_Player, 0);
		UE_LOG(LogTemp, Warning, TEXT("Success Bind Player Actions"));
	}

	enhacedInputComp->BindAction(iA_Look, ETriggerEvent::Triggered, this, &UPlayerAbilityComponent::OnMouseMoving);
	enhacedInputComp->BindAction(iA_Look, ETriggerEvent::Completed, this, &UPlayerAbilityComponent::OnMouseMoveCompleted);
	enhacedInputComp->BindAction(iA_Move, ETriggerEvent::Triggered, this, &UPlayerAbilityComponent::OnMoveKeyPressed);
	enhacedInputComp->BindAction(iA_Move, ETriggerEvent::Completed, this, &UPlayerAbilityComponent::OnMoveKeyPressUp);
	enhacedInputComp->BindAction(iA_FastMove, ETriggerEvent::Triggered, this, &UPlayerAbilityComponent::OnShiftKey);
	enhacedInputComp->BindAction(iA_FastMove, ETriggerEvent::Completed, this, &UPlayerAbilityComponent::OnShiftKey);
	enhacedInputComp->BindAction(iA_Jump, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnSpaceKeyPressed);
	enhacedInputComp->BindAction(iA_ArmActionReady, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnMouseRButton);
	enhacedInputComp->BindAction(iA_ArmActionReady, ETriggerEvent::Completed, this, &UPlayerAbilityComponent::OnMouseRButton);
	enhacedInputComp->BindAction(iA_ArmAction, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnMouseLButton);
	//enhacedInputComp->BindAction(iA_Avoid, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnAvoidKeyPressed);
	enhacedInputComp->BindAction(iA_Crouch, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnCrouchKeyPressed);

	enhacedInputComp->BindAction(iA_CharacTurn, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnCharacTrunKeyPressed);
	enhacedInputComp->BindAction(iA_Esc, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnEscKeyPressed);
	
	enhacedInputComp->BindAction(iA_Skills[(int)ESkillType::Boomerang], ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnBoomerangSkillKeyPressed);
	enhacedInputComp->BindAction(iA_Skills[(int)ESkillType::Giant], ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnGiantSkillKeyPressed);
	enhacedInputComp->BindAction(iA_Skills[(int)ESkillType::Clone], ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnCloneSkillKeyPressed);
	enhacedInputComp->BindAction(iA_Skills[(int)ESkillType::Fire], ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnFireSkillKeyPressed);

	enhacedInputComp->BindAction(iA_Debug, ETriggerEvent::Started, this, &UPlayerAbilityComponent::OnDebugCameraKeyPressed);
}


// Called every frame
void UPlayerAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* charac;
	TraceCharacterInFront(charac);
	// ...
}

void UPlayerAbilityComponent::OnMouseMoving(const FInputActionValue& value)
{
	ownerCharacter->Look(value.Get<FVector2D>());
}

void UPlayerAbilityComponent::OnMouseMoveCompleted(const FInputActionValue& value)
{
	ownerCharacter->InitLook();
}

void UPlayerAbilityComponent::OnMoveKeyPressed(const FInputActionValue& value)
{
	ownerCharacter->Move(value.Get<FVector2D>());
}

void UPlayerAbilityComponent::OnMoveKeyPressUp(const FInputActionValue& value)
{
	ownerCharacter->OnMoveCompleted();
}

void UPlayerAbilityComponent::OnSpaceKeyPressed(const FInputActionValue& value)
{
	ownerCharacter->TryJump();
}

void UPlayerAbilityComponent::OnShiftKey(const FInputActionValue& value)
{
	ownerCharacter->TurnMoveMode(value.Get<bool>());
}

void UPlayerAbilityComponent::OnMouseRButton(const FInputActionValue& value)
{
	if (value.Get<bool>()) ownerCharacter-> DoReadyArmAction();
	else ownerCharacter-> DoEndReadyArmAction();
}

void UPlayerAbilityComponent::OnMouseLButton(const FInputActionValue& value)
{
	if (value.Get<bool>()) ownerCharacter-> DoArmAction();
}

void UPlayerAbilityComponent::OnAvoidKeyPressed(const FInputActionValue& value)
{
	if (value.Get<bool>()) ownerCharacter-> Avoid();
}

void UPlayerAbilityComponent::OnCrouchKeyPressed(const FInputActionValue& value)
{
	if (value.Get<bool>()) ownerCharacter-> Crouch();
}

void UPlayerAbilityComponent::OnCharacTrunKeyPressed(const FInputActionValue& value)
{
	AActor* charac;
	if (!TraceCharacterInFront(charac)) return;
	
	OnTryTurnChracter.ExecuteIfBound(charac);
}

void UPlayerAbilityComponent::OnEscKeyPressed(const FInputActionValue& value)
{
	auto gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	if (!gameMode) return;

	gameMode->inGameUIComponent->OnPauseToggle();
}

void UPlayerAbilityComponent::OnBoomerangSkillKeyPressed(const FInputActionValue& value)
{
	if (!value.Get<bool>()) return;

	ownerCharacter->TrySkill(ESkillType::Boomerang);
}

void UPlayerAbilityComponent::OnGiantSkillKeyPressed(const FInputActionValue& value)
{
	if (!value.Get<bool>()) return;
	
	ownerCharacter->TrySkill(ESkillType::Giant);
}

void UPlayerAbilityComponent::OnCloneSkillKeyPressed(const FInputActionValue& value)
{
	if (!value.Get<bool>()) return;

	ownerCharacter->TrySkill(ESkillType::Clone);
}

void UPlayerAbilityComponent::OnFireSkillKeyPressed(const FInputActionValue& value)
{
	if (!value.Get<bool>()) return;

	ownerCharacter->TrySkill(ESkillType::Fire);
}

void UPlayerAbilityComponent::OnDebugCameraKeyPressed(const FInputActionValue& value)
{
	TArray<AActor*> cameraActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), cameraActors);
	debugCamera = Cast<ACameraActor>(cameraActors[0]);

	if (!debugCamera) return;
	if (Cast<APlayerController>(ownerCharacter->GetController())->GetViewTarget() == ownerCharacter)
	{
		Cast<APlayerController>(ownerCharacter->GetController())->SetViewTarget(debugCamera);
	}
	else
	{
		Cast<APlayerController>(ownerCharacter->GetController())->SetViewTarget(ownerCharacter);
	}
}

bool UPlayerAbilityComponent::TraceCharacterInFront(AActor*& out_SeenCharacter)
{
	out_SeenCharacter = nullptr;

	FHitResult hitResult;
	FVector dir =ownerCharacter->GetMesh()->GetSocketTransform(TEXT("HeadSocket")).GetUnitAxis(EAxis::X);
	FVector startPos = ownerCharacter->GetMesh()->GetSocketLocation(TEXT("HeadSocket")) + dir * 50;

	if (GetWorld()->LineTraceSingleByObjectType(hitResult, startPos, startPos + dir * 1500, ECollisionChannel::ECC_Pawn))
	{
		/*TArray<AActor*, FDefaultAllocator> ignoreArray;
		ignoreArray.Add(ownerCharacter);*/
		/*UKismetSystemLibrary::LineTraceSingle(ownerCharacter, startPos, startPos + dir * 1000, ETraceTypeQuery::TraceTypeQuery5, false, ignoreArray,
			EDrawDebugTrace::ForOneFrame, hitResult, true);*/
		if (auto checkedCharac = Cast<ADodgeballCharacter>(hitResult.GetActor()))
		{
			if (checkedCharac != ownerCharacter && checkedCharac->teamType == ownerCharacter->teamType)
			{
				//보고 있는 캐릭터 매개변수로 전달 실행
				OnCheckingWatchingTeamMemberHavingBall.ExecuteIfBound(checkedCharac);
				out_SeenCharacter = checkedCharac;
				return true;
			}
		}
	}

	OnCheckingWatchingTeamMemberHavingBall.ExecuteIfBound(nullptr);

	return false;
}
