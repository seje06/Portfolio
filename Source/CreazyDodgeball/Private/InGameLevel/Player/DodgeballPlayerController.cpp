// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/Player/DodgeballPlayerController.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/Player/PlayerAbilityComponent.h"
#include "InGameLevel/AI/DodgeballAIController.h"
#include "InGameLevel/AI/DodgeballAIManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "InGameLevel/Ball/DodgeBall.h"
#include "InGameLevel/UI/EnergyBarWidget.h"
#include "InGameLevel/DodgeballGameState.h"
#include "Components/ProgressBar.h"

ADodgeballPlayerController::ADodgeballPlayerController()
{

}

void ADodgeballPlayerController::Init()
{
	if (energyBarWidgetClass) energyBarWidget = CreateWidget<UEnergyBarWidget>(this, energyBarWidgetClass);
	energyBarWidget->AddToViewport();

	//ó�� ������ ��
	currentPlayerCharacter = Cast<ADodgeballCharacter>(GetPawn());
	if(!currentPlayerCharacter->onEnergyChanged.IsBoundToObject(currentPlayerCharacter))
	currentPlayerCharacter->onEnergyChanged.BindUObject(this, &ADodgeballPlayerController::SetSkillKeyWidgetAndEnergyBar);


	//�������� �÷��̾� �ɷ� ������Ʈ ����
	playerAbilityComp = NewObject<UPlayerAbilityComponent>(currentPlayerCharacter, playerAbilityCompClass, TEXT("playerAbilityComp"));

	//���忡 ���
	playerAbilityComp->RegisterComponent();
	//�÷��̾� �ɷ�������Ʈ�� ĳ�� ��ȯ�� ���� �Լ��� ���
	playerAbilityComp->OnCheckingWatchingTeamMemberHavingBall.BindUObject(this, &ADodgeballPlayerController::SetCharacTurningWidget);
	playerAbilityComp->OnTryTurnChracter.BindUObject(this, &ADodgeballPlayerController::TurnCharacter);
	//�÷��̾��� ��ǲ ������ �۵��ϰ� �ٽ� ����
	//SetPawn(currentPlayerCharacter);
	Possess(currentPlayerCharacter);
}

void ADodgeballPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

/// <summary>
/// ĳ���� �����ϸ� ��ȯ���� ����. ���ٸ� ����
/// </summary>
/// <param name="charac">��ȯ ������ ����, ���ŵ� ����</param>
void ADodgeballPlayerController::SetCharacTurningWidget(AActor* charac)
{
	if (characTurningWidgetComp == nullptr)
	{
		if (!charac) return;
		
		characTurningWidgetComp = NewObject<UWidgetComponent>(charac, characTurningWidgetCompClass, TEXT("CharacTurningWidgetComp"));
		characTurningWidgetComp->RegisterComponent();
		characTurningWidgetComp->AttachToComponent(Cast<ACharacter>(charac)->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
		characTurningWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		characTurningWidgetComp->SetDrawSize(FVector2D(100, 33));
		characTurningWidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
		characTurningWidgetComp->SetRelativeLocation(FVector(0, 0, 200));
	}
	else
	{
		if (charac) return;

		characTurningWidgetComp->DestroyComponent();
		characTurningWidgetComp = nullptr;
	}
}

void ADodgeballPlayerController::TurnCharacter(AActor* charac)
{
	auto dodgeballCharac = Cast<ADodgeballCharacter>(charac);
	if (!dodgeballCharac) return;

	//���� ĳ������ �÷��̾� �ɷ� ������Ʈ ����
	playerAbilityComp->DestroyComponent();
	playerAbilityComp = nullptr;

	//aIManager->OnPlayerCharacterTurned(currentPlayerCharacter->currentFloorNum, Cast<ADodgeballAIController>(dodgeballCharac->GetController())->selectedFloorIndex);

	//��ȯ�� ĳ������ �ӷ� ����
	FVector savedAfterCharacVelocity = dodgeballCharac->GetCharacterMovement()->Velocity;

	//��ȯ�� ĳ������ ��Ʈ�ѷ� ����
	if (dodgeballCharac->GetController() && Cast<ADodgeballAIController>(dodgeballCharac->GetController()))
	{
		auto aIContoller = dodgeballCharac->GetController();

		auto ball = Cast<ADodgeballGameState>(GetWorld()->GetGameState())->ball;
		Cast<ADodgeBall>(ball)->OnCatched.RemoveAll(aIContoller);
		Cast<ADodgeBall>(ball)->OnGrounded.RemoveAll(aIContoller);

		aIContoller->UnPossess();
		aIContoller->Destroy(); // AIController ����
		UE_LOG(LogTemp, Warning, TEXT("playerController - exist AIController"));
	}

	//�������� �÷��̾� �ɷ� ������Ʈ ����. �׸��� ��ȯ�� ĳ���Ϳ� �ٿ���.
	playerAbilityComp = NewObject<UPlayerAbilityComponent>(dodgeballCharac, playerAbilityCompClass, TEXT("playerAbilityComp"));

	//���忡 ���
	playerAbilityComp->RegisterComponent();

	//���Ŀ� �ٽ� ĳ���� ��ȯ�Ҷ� ����� �Լ� ���ε�
	playerAbilityComp->OnCheckingWatchingTeamMemberHavingBall.BindUObject(this, &ADodgeballPlayerController::SetCharacTurningWidget);
	playerAbilityComp->OnTryTurnChracter.BindUObject(this, &ADodgeballPlayerController::TurnCharacter);

	//���� ĳ���� ���ν�Ƽ ����
	FVector savedPreCharacVelocity = currentPlayerCharacter->GetCharacterMovement()->Velocity;

	//���� ĳ������ �÷��̾���Ʈ�ѷ� ����
	UnPossess();

	//��Ȱ�� ĳ���Ϳ� player��Ʈ�ѷ��� �ٿ���.
	SetPawn(dodgeballCharac);
	Possess(dodgeballCharac);

	//��ȯ�� ĳ���Ϳ��� �ӷ� ���� �� �ü� �ʱ�ȭ
	if (dodgeballCharac->characterActionType == ECharacterActionType::Jump || dodgeballCharac->characterActionType == ECharacterActionType::Avoid)
	{
		dodgeballCharac->LaunchCharacter(savedAfterCharacVelocity, true, true);
	}
	dodgeballCharac->OnMoveCompleted();
	dodgeballCharac->InitMovement();
	dodgeballCharac->Look(FVector2D::Zero());
	dodgeballCharac->onEnergyChanged.BindUObject(this, &ADodgeballPlayerController::SetSkillKeyWidgetAndEnergyBar);
	SetPlayerEnergyBar(dodgeballCharac->energy);
	if (dodgeballCharac->energy < dodgeballCharac->maxEnergy)SetSkillKeyWidgetAndEnergyBar(false);
	else SetSkillKeyWidgetAndEnergyBar(true);


	//���� ĳ���Ϳ��� ai ��Ʈ�ѷ� �ٿ���.
	auto aiController = GetWorld()->SpawnActor<ADodgeballAIController>(ADodgeballAIController::StaticClass());
	aiController->SetPawn(currentPlayerCharacter);
	aiController->Possess(currentPlayerCharacter);
	//���� ĳ���� �ӷ� ���� �� �ü� �ʱ�ȭ
	
	if (currentPlayerCharacter->characterActionType == ECharacterActionType::Jump || currentPlayerCharacter->characterActionType == ECharacterActionType::Avoid)
	{
		currentPlayerCharacter->LaunchCharacter(savedPreCharacVelocity, true, true);
	}
	currentPlayerCharacter->type = ECharacterType::AI;
	currentPlayerCharacter->OnMoveCompleted();
	currentPlayerCharacter->Look(FVector2D::Zero());

	//�����÷��̾� ĳ���� ���� ����
	currentPlayerCharacter = dodgeballCharac;

	//UE_LOG(LogTemp, Warning, TEXT("Sussecced Turn Character"));
}

void ADodgeballPlayerController::SetSkillKeyWidgetAndEnergyBar(bool isVisibility)
{
	if (!skillKeyWidget)
	{
		skillKeyWidget = CreateWidget<UUserWidget>(this, skillKeyWidgetClass);
		skillKeyWidget->AddToViewport();
	}
	if (isVisibility) skillKeyWidget->SetVisibility(ESlateVisibility::Visible);
	else skillKeyWidget->SetVisibility(ESlateVisibility::Hidden);

	SetPlayerEnergyBar(Cast<ADodgeballCharacter>(GetPawn())->energy);
}

void ADodgeballPlayerController::SetPlayerEnergyBar(float energy)
{
	if (!energyBarWidget) return;
	
	energyBarWidget->energyBar->SetPercent(energy / 3.f);
	//// ������ ���� 0~3 ����
	//float percent = FMath::Clamp(static_cast<float>(_currentEnergy) / 3.0f, 0.0f, 1.0f);
	//skillEnergyBar->SetPercent(percent);
}
