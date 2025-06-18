// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/AI/DodgeballAIManager.h"
#include "Kismet/GameplayStatics.h"
#include "InGameLevel/Characters/DodgeballCharacter.h"
#include "InGameLevel/AI/DodgeballAIController.h"
#include "Components/CapsuleComponent.h"
#include "InGameLevel/DodgeballGameState.h"
#include "InGameLevel/Ball/DodgeBall.h"

// Sets default values
ADodgeballAIManager::ADodgeballAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADodgeballAIManager::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.TickGroup = TG_DuringPhysics;

	Init();
}

void ADodgeballAIManager::TeamAAIInit()
{
	ADodgeballGameState* gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	TArray<AActor*> _floors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), floorClass, _floors);

	characters.Init(nullptr, _floors.Num());
	floors.Init(nullptr, _floors.Num());
	inFieldCount = _floors.Num();

	for (auto& floor : _floors)
	{
		for (auto& tag : floor->Tags)
		{
			if (tag.ToString().Contains("Plane"))
			{
				//���ҵ� �ٴ��� ��ȣ�� �ε�������� �ٴ� ���͸� ������
				FString tagStr = tag.ToString();
				FString numberPart = tagStr.RightChop(5); // "Plane"�� 5����
				int32 floorNum = FCString::Atoi(*numberPart);
				//floorMap[floorNum]=floor;
				floors[floorNum - 1] = floor;

				//1�� �ٴڿ� �÷��̾ ������ AI�� �ְ� ��ġ�� �ٴ�����
				if (floorNum == 1)
				{
					characters[floorNum - 1] = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
					currentPlayerCharacter = characters[floorNum - 1];
					if (!characters[floorNum - 1])
					{
						UE_LOG(LogTemp, Warning, TEXT("player null"));
						return;
					}
					currentPlayerFloorNum = floorNum;
					characters[floorNum - 1]->currentFloorNum = floorNum;
					characters[floorNum - 1]->SetActorLocation(FVector(floor->GetActorLocation().X, floor->GetActorLocation().Y,
						floor->GetActorLocation().Z + characters[floorNum - 1]->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));
					//�ٶ� ����
					FVector goalLookDir = FVector(gameState->judgementInitLocation.X, gameState->judgementInitLocation.Y, 0) - FVector(characters[floorNum - 1]->GetActorLocation().X, characters[floorNum - 1]->GetActorLocation().Y, 0);
					if (gameState->currentSet && *gameState->currentSet == 1) characters[floorNum - 1]->SetActorRotation(FRotator(0, 0, 0));
					else characters[floorNum - 1]->SetActorRotation(goalLookDir.ToOrientationRotator());
				}
				else
				{
					characters[floorNum - 1] = GetWorld()->SpawnActor<ADodgeballCharacter>(dodgeballCharacterClass);
					characters[floorNum - 1]->SetActorLocation(FVector(floor->GetActorLocation().X, floor->GetActorLocation().Y,
						floor->GetActorLocation().Z + characters[floorNum - 1]->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));

					//�ٶ� ����
					FVector goalLookDir = FVector(gameState->judgementInitLocation.X, gameState->judgementInitLocation.Y, 0) - FVector(characters[floorNum - 1]->GetActorLocation().X, characters[floorNum - 1]->GetActorLocation().Y, 0);
					if (gameState->currentSet && *gameState->currentSet == 1) characters[floorNum - 1]->SetActorRotation(FRotator(0, 0, 0));
					else characters[floorNum - 1]->SetActorRotation(goalLookDir.ToOrientationRotator());

					//AIController ����
					auto aIController = GetWorld()->SpawnActor<ADodgeballAIController>(ADodgeballAIController::StaticClass());
					aIController->SetPawn(characters[floorNum - 1]);
					aIController->Possess(characters[floorNum - 1]);

					//��Ʈ�ѷ��� aiFloorNum ����
					//auto aIController = Cast<ADodgeballAIController>(characters[floorNum - 1]->GetController());
					if (!aIController)
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed AIManager begin"));

					}
					aIController->aIFloorNums.Add(floorNum);
					characters[floorNum - 1]->currentFloorNum = floorNum;
				}
			}
		}
	}
}

void ADodgeballAIManager::TeamBAIInit()
{
	ADodgeballGameState* gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	TArray<AActor*> _floorsB;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), floorClassB, _floorsB);

	charactersB.Init(nullptr, _floorsB.Num());
	floorsB.Init(nullptr, _floorsB.Num());
	inFieldCountB = _floorsB.Num();

	for (auto& floor : _floorsB)
	{
		for (auto& tag : floor->Tags)
		{
			if (tag.ToString().Contains("Plane"))
			{
				//���ҵ� �ٴ��� ��ȣ�� �ε�������� �ٴ� ���͸� ������
				FString tagStr = tag.ToString();
				FString numberPart = tagStr.RightChop(5); // "Plane"�� 5����
				int32 floorNum = FCString::Atoi(*numberPart);
				floorNum -= _floorsB.Num();
				//floorMap[floorNum]=floor;
				floorsB[floorNum - 1] = floor;

				//b���� ��� ai��
				charactersB[floorNum - 1] = GetWorld()->SpawnActor<ADodgeballCharacter>(dodgeballCharacterClass);
				charactersB[floorNum - 1]->teamType = ECharacterTeamType::B;
				charactersB[floorNum - 1]->SetActorLocation(FVector(floor->GetActorLocation().X, floor->GetActorLocation().Y,
					floor->GetActorLocation().Z + charactersB[floorNum - 1]->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));

				FVector goalLookDir = FVector(gameState->judgementInitLocation.X, gameState->judgementInitLocation.Y, 0) - FVector(charactersB[floorNum - 1]->GetActorLocation().X, charactersB[floorNum - 1]->GetActorLocation().Y, 0);
				
				if (gameState->currentSet && *gameState->currentSet == 1) charactersB[floorNum - 1]->SetActorRotation(FRotator(0, 180, 0));
				else charactersB[floorNum - 1]->SetActorRotation(goalLookDir.ToOrientationRotator());
				
				if(aIMI) charactersB[floorNum - 1]->GetMesh()->SetMaterial(1, aIMI);
				if(aICapMI)charactersB[floorNum - 1]->capMesh->SetMaterial(0, aICapMI);

				//AIController ����
				auto aIController = GetWorld()->SpawnActor<ADodgeballAIController>(ADodgeballAIController::StaticClass());
				aIController->SetPawn(charactersB[floorNum - 1]);
				aIController->Possess(charactersB[floorNum - 1]);

				//��Ʈ�ѷ��� aiFloorNum ����
				//auto aIController = Cast<ADodgeballAIController>(characters[floorNum - 1]->GetController());
				if (!aIController)
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed AIManager begin"));
				}
				aIController->aIFloorNums.Add(floorNum);
				charactersB[floorNum - 1]->currentFloorNum = floorNum;

			}
		}
	}
}

void ADodgeballAIManager::UpdateTeamAAIFloorNum(float DeltaTime)
{
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!player) return;


	//�÷��̾ �ִ� �� A
	//�÷��̾ �ٸ� �ٴ����� �� ������ ���� �÷��̾ �ִ����� ��ȣ�� AI���� ����
	int playerCharacFloorNum = player->currentFloorNum;

	if (currentPlayerCharacter == player && currentPlayerFloorNum != playerCharacFloorNum)
	{
		if (currentPlayerFloorNum != playerCharacFloorNum)
		{
			ChangeFloorNum(characters, currentPlayerFloorNum - 1, playerCharacFloorNum - 1);

			currentPlayerFloorNum = playerCharacFloorNum;
			currentPlayerCharacter = player;
		}
	}
	else if (currentPlayerCharacter != player)
	{
		for (int i = 0; i < characters.Num(); i++)
		{
			if (characters[i] == player && i != playerCharacFloorNum - 1)
			{
				ChangeFloorNum(characters, playerCharacFloorNum - 1, i);
				break;
			}
		}
		currentPlayerFloorNum = playerCharacFloorNum;
		currentPlayerCharacter = player;
	}

	//���� �ֱ⸶�� � �� ai�� �־���� �ٴڹ�ȣ�� �ٸ� AI�� ��ȯ.
	floorNumMixingTimer -= DeltaTime;
	if (floorNumMixingTimer <= 0)
	{
		//MixFloorNum();
		floorNumMixingTimer = 5;
	}

	//ai�鿡�� �ο��� �ٴ� ��ȣ ����
	for (int i = 0; i < characters.Num(); i++)
	{
		if (!characters[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("character%d null"), i);
			continue;
		}

		if (characters[i]->type == ECharacterType::AI)
		{
			auto dodgeballAIController = Cast<ADodgeballAIController>(characters[i]->GetController());
			if (!dodgeballAIController)
			{
				UE_LOG(LogTemp, Warning, TEXT("DodgeballAIController Null"));
				return;
			}
			dodgeballAIController->aIFloorNums.Empty();
		}
	}

	for (int i = 0; i < characters.Num(); i++)
	{
		if (!characters[i]) continue;

		//AI��Ʈ�ѷ����� AI�� �ٴ� ��ȣ ����
		if (characters[i]->type == ECharacterType::Player)
		{

		}
		else
		{
			auto dodgeballAIController = Cast<ADodgeballAIController>(characters[i]->GetController());
			dodgeballAIController->aIFloorNums.Add(i + 1);
			dodgeballAIController->floors = floors;
		}
	}
}

void ADodgeballAIManager::UpdateTeamBAIFloorNum(float DeltaTime)
{

	//b�� �ڸ� ����
	//���� �ֱ⸶�� � �� ai�� �־���� �ٴڹ�ȣ�� �ٸ� AI�� ��ȯ.
	floorNumMixingTimerB -= DeltaTime;
	if (floorNumMixingTimerB <= 0)
	{
		//MixFloorNum();
		floorNumMixingTimerB = 5;
	}

	//ai�鿡�� �ο��� �ٴ� ��ȣ ����
	for (int i = 0; i < charactersB.Num(); i++)
	{
		if (!charactersB[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("characterB%d null"), i);
			continue;
		}

		if (charactersB[i]->type == ECharacterType::AI)
		{
			auto dodgeballAIController = Cast<ADodgeballAIController>(charactersB[i]->GetController());
			if (!dodgeballAIController)
			{
				UE_LOG(LogTemp, Warning, TEXT("DodgeballAIController Null"));
				return;
			}
			dodgeballAIController->aIFloorNums.Empty();
		}
	}

	for (int i = 0; i < charactersB.Num(); i++)
	{
		if (!charactersB[i]) continue;

		//AI��Ʈ�ѷ����� AI�� �ٴ� ��ȣ ����
		if (charactersB[i]->type == ECharacterType::Player)
		{

		}
		else
		{
			auto dodgeballAIController = Cast<ADodgeballAIController>(charactersB[i]->GetController());
			dodgeballAIController->aIFloorNums.Add(i + 1);
			dodgeballAIController->floors = floorsB;
		}
	}
}

void ADodgeballAIManager::DestroyAllAI()
{
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	for (int i = 0; i < characters.Num(); i++)
	{
		if (!characters[i]) continue;
		if (characters[i] == player) continue;

		if (auto aIController = characters[i]->GetController())
		{
			aIController->Destroy();
		}
		characters[i]->Destroy();
		characters[i] = nullptr;
	}

	for (int i = 0; i < charactersB.Num(); i++)
	{
		if (!charactersB[i]) continue;
		if (charactersB[i] == player) continue;

		if (auto aIController = charactersB[i]->GetController())
		{
			aIController->Destroy();
		}
		charactersB[i]->Destroy();
		charactersB[i] = nullptr;
	}
}

void ADodgeballAIManager::DisappearAI(ADodgeballGameState* gameState,ADodgeballCharacter* charac)
{
	UpdateTeamAAIFloorNum(GetWorld()->DeltaTimeSeconds);
	UpdateTeamBAIFloorNum(GetWorld()->DeltaTimeSeconds);

	Cast<ADodgeBall>(gameState->ball)->OnCatched.RemoveAll(charac->GetController());
	Cast<ADodgeBall>(gameState->ball)->OnGrounded.RemoveAll(charac->GetController());
	charac->GetController()->Destroy();
	charac->OnDisappeared();
}

void ADodgeballAIManager::OnCharacterOut(ADodgeballCharacter* charac)
{
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	//if (player == charac) return;

	//�÷��̾�(��� ��Ʈ ����) �Ǵ� �÷��̾� + �ƿ��Ǵ� ĳ����(��� �� ó����) �� ������� ������ �����ؾ��ϴ� ���� �־�ߵ�!!
	auto& _characters = charac->teamType == ECharacterTeamType::A ? characters : charactersB;
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	if (gameState->currentAliveTeamACharacterCount == 0 || gameState->currentAliveTeamBCharacterCount==0) return;

	if (charac->teamType == ECharacterTeamType::A)
	{
		if (gameState->currentAliveTeamACharacterCount==2)
		{
			gameState->currentAliveTeamACharacterCount--;
			for (int i = 0; i < _characters.Num(); i++)
			{
				if (_characters[i] == charac)_characters[i] = nullptr;
			}
			DisappearAI(gameState, charac);
			//charac->Destroy();
			return;
		}
		else if (gameState->currentAliveTeamACharacterCount == 1)
		{
			//���� ���� ��Ű��
			gameState->currentAliveTeamACharacterCount--;
			DisappearAI(gameState, charac);
			return;
		}
	}
	else
	{
		if (gameState->currentAliveTeamBCharacterCount == 1)
		{
			gameState->currentAliveTeamBCharacterCount--;
			//���� ���� ��Ű��. ��B
			DisappearAI(gameState, charac);
			//charac->Destroy();
			return;
		}
	}
	
	// B���� 2�� �̻��̾����� A�� 3�� �̻��̾����� �����ϰ� �����ִ� �ٸ�ai���� �ڸ��� ������
	auto aIController = Cast<ADodgeballAIController>(charac->GetController());
	if (!aIController || !IsValid(aIController)) return;

	for (int i = 0; i < aIController->aIFloorNums.Num(); i++)
	{
		int randomCharacIndex = FMath::RandRange(0, _characters.Num() - 1);
		while (_characters[randomCharacIndex] == charac || _characters[randomCharacIndex] == player)
		{
			randomCharacIndex= FMath::RandRange(0, _characters.Num() - 1);
		}
		_characters[aIController->aIFloorNums[i] - 1] = _characters[randomCharacIndex];
	}

	if(charac->teamType==ECharacterTeamType::A) gameState->currentAliveTeamACharacterCount--;
	else gameState->currentAliveTeamBCharacterCount--;
	UE_LOG(LogTemp, Warning, TEXT("disappearAI!"));

	DisappearAI(gameState, charac);
	//charac->Destroy();
}

void ADodgeballAIManager::OnCharacterIn(ECharacterTeamType getInTeamType)
{
	TArray<ADodgeballCharacter*>& _characters = getInTeamType == ECharacterTeamType::A ? characters : charactersB;
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	auto player = Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (getInTeamType == ECharacterTeamType::A && gameState->currentAliveTeamACharacterCount >= _characters.Num()) return;
	if (getInTeamType == ECharacterTeamType::B && gameState->currentAliveTeamBCharacterCount >= _characters.Num()) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto getInCharac = GetWorld()->SpawnActor<ADodgeballCharacter>(dodgeballCharacterClass, Params);

	getInCharac->teamType = getInTeamType;
	if (getInTeamType == ECharacterTeamType::B)
	{
		getInCharac->GetMesh()->SetMaterial(1, aIMI);
		getInCharac->capMesh->SetMaterial(0, aICapMI);
	}
	auto newAIController = GetWorld()->SpawnActor<ADodgeballAIController>(ADodgeballAIController::StaticClass());
	newAIController->SetPawn(getInCharac);
	newAIController->Possess(getInCharac);


	//a���� ������ 1���� �������, b���̾������� �������
	if (getInTeamType == ECharacterTeamType::A)
	{
		if (gameState->currentAliveTeamACharacterCount == 1)
		{
			for (int i = 0; i < _characters.Num() ; i++)
			{
				if (_characters[i] != player) _characters[i] = getInCharac;
			}
			gameState->currentAliveTeamACharacterCount++;
			UpdateTeamAAIFloorNum(GetWorld()->DeltaTimeSeconds);

			int randSpawnFloorIndex = FMath::RandRange(0, _characters.Num() - 1);
			while (_characters[randSpawnFloorIndex] == player)randSpawnFloorIndex = FMath::RandRange(0, _characters.Num() - 1);

			getInCharac->SetActorLocation(floors[randSpawnFloorIndex]->GetActorLocation() + FVector(0, 0, getInCharac->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));
			getInCharac->OnAppeared();
			return;
		}
	}
	else
	{
		/*if (gameState->currentAliveTeamBCharacterCount == 1)
		{
			for (int i = 0; i < _characters.Num(); i++)
			{
				if (_characters[i] != Cast<ADodgeballCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())) _characters[i] = getInCharac;
			}
			gameState->currentAliveTeamBCharacterCount++;
			UpdateTeamBAIFloorNum(GetWorld()->DeltaTimeSeconds);
			return;
		}*/
	}

	//a���̸� 2���̻��̾�����, b���̸� 1�� �̻��̾����� ������ ĳ�� �ֱ�
	for (int i = 0; i < _characters.Num(); i++)
	{
		if (_characters[i] != player)
		{
			auto aIController = Cast<ADodgeballAIController>(_characters[i]->GetController());
			if (aIController && aIController->aIFloorNums.Num() >= 2)
			{
				_characters[i] = getInCharac;

				UpdateTeamAAIFloorNum(GetWorld()->DeltaTimeSeconds);
				UpdateTeamBAIFloorNum(GetWorld()->DeltaTimeSeconds);

				//int randSpawnFloorIndex = FMath::RandRange(0, aIController->aIFloorNums.Num() - 1);

				/*if (getInTeamType == ECharacterTeamType::A) getInCharac->SetActorLocation(floors[aIController->aIFloorNums[randSpawnFloorIndex] - 1]->GetActorLocation() + FVector(0, 0, getInCharac->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));
				else getInCharac->SetActorLocation(floorsB[aIController->aIFloorNums[randSpawnFloorIndex] - 1]->GetActorLocation() + FVector(0, 0, getInCharac->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));*/
				
				if (getInTeamType == ECharacterTeamType::A)
				{
					getInCharac->SetActorLocation(floors[i]->GetActorLocation() + FVector(0, 0, getInCharac->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));
					gameState->currentAliveTeamACharacterCount++;
				}
				else
				{
					getInCharac->SetActorLocation(floorsB[i]->GetActorLocation() + FVector(0, 0, getInCharac->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 30));
					gameState->currentAliveTeamBCharacterCount++;
				}
				getInCharac->OnAppeared();

				return;
			}
		}
	}


}

void ADodgeballAIManager::MixFloorNum(TArray<ADodgeballCharacter*>& _characters)
{
	//���߿� ĳ���� 2�����ϸ� ����
	if (inFieldCount <= 2) return;

	int count = 0;
	int floorIndexA = FMath::RandRange(0, 5);
	int floorIndexB = FMath::RandRange(0, 5);
	while (floorIndexA == floorIndexB|| currentPlayerFloorNum==floorIndexA||currentPlayerFloorNum==floorIndexB)
	{
		floorIndexA = FMath::RandRange(0, 5);
		floorIndexB = FMath::RandRange(0, 5);
		if (count++ > 10000) break;
	}

	ChangeFloorNum(_characters,floorIndexA, floorIndexB);
}

void ADodgeballAIManager::ChangeFloorNum(TArray<ADodgeballCharacter*>& _characters,int indexA, int indexB)
{
	auto tempCharac = _characters[indexA];
	_characters[indexA] = _characters[indexB];
	_characters[indexB] = tempCharac;
}

// Called every frame
void ADodgeballAIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateTeamAAIFloorNum(DeltaTime);

	//b�� �ڸ� ����
	//���� �ֱ⸶�� � �� ai�� �־���� �ٴڹ�ȣ�� �ٸ� AI�� ��ȯ.
	UpdateTeamBAIFloorNum(DeltaTime);

	//���� ���� ������ �ִ� ĳ�� ������Ʈ
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());
	if(gameState->currentHavingTurnTeamType==ECharacterTeamType::A) gameState->UpdateProximateCharacterToBall(ECharacterTeamType::A,characters);
	else if(gameState->currentHavingTurnTeamType == ECharacterTeamType::B) gameState->UpdateProximateCharacterToBall(ECharacterTeamType::B, charactersB);
}

void ADodgeballAIManager::Init()
{
	auto gameState = Cast<ADodgeballGameState>(GetWorld()->GetGameState());

	//teamA ����
	TeamAAIInit();

	//teamB����
	TeamBAIInit();

	gameState->charactersA = &characters;
	gameState->charactersB = &charactersB;
	//gameState->currentAliveTeamACharacterCount = characters.Num();
	//gameState->currentAliveTeamBCharacterCount = charactersB.Num();
}


