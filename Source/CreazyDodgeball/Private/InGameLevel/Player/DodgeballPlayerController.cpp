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

	//처음 지정된 폰
	currentPlayerCharacter = Cast<ADodgeballCharacter>(GetPawn());
	if(!currentPlayerCharacter->onEnergyChanged.IsBoundToObject(currentPlayerCharacter))
	currentPlayerCharacter->onEnergyChanged.BindUObject(this, &ADodgeballPlayerController::SetSkillKeyWidgetAndEnergyBar);


	//동적으로 플레이어 능력 컴포넌트 생성
	playerAbilityComp = NewObject<UPlayerAbilityComponent>(currentPlayerCharacter, playerAbilityCompClass, TEXT("playerAbilityComp"));

	//월드에 등록
	playerAbilityComp->RegisterComponent();
	//플레이어 능력컴포넌트에 캐릭 전환에 대한 함수들 등록
	playerAbilityComp->OnCheckingWatchingTeamMemberHavingBall.BindUObject(this, &ADodgeballPlayerController::SetCharacTurningWidget);
	playerAbilityComp->OnTryTurnChracter.BindUObject(this, &ADodgeballPlayerController::TurnCharacter);
	//플레이어의 인풋 맵핑이 작동하게 다시 설정
	//SetPawn(currentPlayerCharacter);
	Possess(currentPlayerCharacter);
}

void ADodgeballPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

/// <summary>
/// 캐릭이 존재하면 전환위젯 생성. 없다면 제거
/// </summary>
/// <param name="charac">전환 위젯이 생성, 제거될 액터</param>
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

	//기존 캐릭터의 플레이어 능력 컴포넌트 제거
	playerAbilityComp->DestroyComponent();
	playerAbilityComp = nullptr;

	//aIManager->OnPlayerCharacterTurned(currentPlayerCharacter->currentFloorNum, Cast<ADodgeballAIController>(dodgeballCharac->GetController())->selectedFloorIndex);

	//전환할 캐릭터의 속력 저장
	FVector savedAfterCharacVelocity = dodgeballCharac->GetCharacterMovement()->Velocity;

	//전환할 캐릭터의 컨트롤러 제거
	if (dodgeballCharac->GetController() && Cast<ADodgeballAIController>(dodgeballCharac->GetController()))
	{
		auto aIContoller = dodgeballCharac->GetController();

		auto ball = Cast<ADodgeballGameState>(GetWorld()->GetGameState())->ball;
		Cast<ADodgeBall>(ball)->OnCatched.RemoveAll(aIContoller);
		Cast<ADodgeBall>(ball)->OnGrounded.RemoveAll(aIContoller);

		aIContoller->UnPossess();
		aIContoller->Destroy(); // AIController 제거
		UE_LOG(LogTemp, Warning, TEXT("playerController - exist AIController"));
	}

	//동적으로 플레이어 능력 컴포넌트 생성. 그리고 전환할 캐릭터에 붙여줌.
	playerAbilityComp = NewObject<UPlayerAbilityComponent>(dodgeballCharac, playerAbilityCompClass, TEXT("playerAbilityComp"));

	//월드에 등록
	playerAbilityComp->RegisterComponent();

	//이후에 다시 캐릭터 전환할때 실행될 함수 바인드
	playerAbilityComp->OnCheckingWatchingTeamMemberHavingBall.BindUObject(this, &ADodgeballPlayerController::SetCharacTurningWidget);
	playerAbilityComp->OnTryTurnChracter.BindUObject(this, &ADodgeballPlayerController::TurnCharacter);

	//기존 캐릭터 벨로시티 저장
	FVector savedPreCharacVelocity = currentPlayerCharacter->GetCharacterMovement()->Velocity;

	//기존 캐릭터의 플레이어컨트롤러 해제
	UnPossess();

	//전활할 캐릭터에 player컨트롤러를 붙여줌.
	SetPawn(dodgeballCharac);
	Possess(dodgeballCharac);

	//전환한 캐릭터에게 속력 설정 및 시선 초기화
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


	//기존 캐릭터에는 ai 컨트롤러 붙여줌.
	auto aiController = GetWorld()->SpawnActor<ADodgeballAIController>(ADodgeballAIController::StaticClass());
	aiController->SetPawn(currentPlayerCharacter);
	aiController->Possess(currentPlayerCharacter);
	//기존 캐릭터 속력 설정 및 시선 초기화
	
	if (currentPlayerCharacter->characterActionType == ECharacterActionType::Jump || currentPlayerCharacter->characterActionType == ECharacterActionType::Avoid)
	{
		currentPlayerCharacter->LaunchCharacter(savedPreCharacVelocity, true, true);
	}
	currentPlayerCharacter->type = ECharacterType::AI;
	currentPlayerCharacter->OnMoveCompleted();
	currentPlayerCharacter->Look(FVector2D::Zero());

	//현재플레이어 캐릭터 변수 갱신
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
	//// 에너지 값은 0~3 범위
	//float percent = FMath::Clamp(static_cast<float>(_currentEnergy) / 3.0f, 0.0f, 1.0f);
	//skillEnergyBar->SetPercent(percent);
}
