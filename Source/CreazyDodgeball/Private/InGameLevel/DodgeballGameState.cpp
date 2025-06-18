// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/DodgeballGameState.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"

void ADodgeballGameState::UpdateProximateCharacterToBall(ECharacterTeamType includedTeam, TArray<ADodgeballCharacter*>& characters)
{
	if (characters.Num() == 0) return;
	if (!ball || !IsValid(ball))
	{
		ball = nullptr;
		return;
	}

	for (int i = 0; i < characters.Num(); i++)
	{
		if (!characters[i] || !IsValid(characters[i]))
		{
			characters[i] = nullptr;
			continue;
		}//볼과 가장 가까운 캐릭터 게임 스테이트에 저장.
		if (currentHavingTurnTeamType == characters[i]->teamType && !currentCatchingBallCharac)
		{
			if (!proximateCharacterToBall || !IsValid(proximateCharacterToBall)) proximateCharacterToBall = characters[i];
			else
			{
				float currentProximateDis = FVector::Dist2D(proximateCharacterToBall->GetActorLocation(), ball->GetActorLocation());
				float dis = FVector::Dist2D(characters[i]->GetActorLocation(), ball->GetActorLocation());
				if (dis < currentProximateDis - 20)
				{
					proximateCharacterToBall = characters[i];
				}
			}
		}
	}
	if (!IsValid(proximateCharacterToBall)) proximateCharacterToBall = nullptr;
}

ACharacter* ADodgeballGameState::GetProximateEnemyToLocation(ACharacter* charac, FVector standardLocation)
{
	auto dodgeballCharac = Cast<ADodgeballCharacter>(charac);
	if (!dodgeballCharac) return nullptr;
	if (dodgeballCharac->teamType ==ECharacterTeamType::None) return nullptr;
	
	ADodgeballCharacter* proximateEnemy = nullptr;

	switch (dodgeballCharac->teamType)
	{
	case ECharacterTeamType::A:
		for (auto& enemy : *charactersB)
		{
			if(enemy)
			if (!proximateEnemy) proximateEnemy = enemy;
			else
			{
				float currentProximateDis=FVector::Dist2D(proximateEnemy->GetActorLocation(), standardLocation);
				float dis= FVector::Dist2D(enemy->GetActorLocation(), standardLocation);

				if (dis < currentProximateDis) proximateEnemy = enemy;
			}
		}
		break;
	case ECharacterTeamType::B:
		for (auto& enemy : *charactersA)
		{
			if(enemy)
			if (!proximateEnemy) proximateEnemy = enemy;
			else
			{
				float currentProximateDis = FVector::Dist2D(proximateEnemy->GetActorLocation(), standardLocation);
				float dis = FVector::Dist2D(enemy->GetActorLocation(), standardLocation);

				if (dis < currentProximateDis) proximateEnemy = enemy;
			}
		}
		break;
	case ECharacterTeamType::None:
		break;
	}

	return proximateEnemy;
}
