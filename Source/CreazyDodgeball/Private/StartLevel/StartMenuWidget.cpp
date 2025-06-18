#include "StartLevel/StartMenuWidget.h"
#include "Components/Button.h"
#include "StartLevel/StartLevelManager.h"



void UStartMenuWidget::SetManager(AStartLevelManager* _manager)
{
	manager = _manager;
}

void UStartMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (startButton)
	{
		startButton->OnClicked.AddDynamic(this, & UStartMenuWidget::HandleStartClicked);
	}

	if (settingsButton)
	{
		settingsButton->OnClicked.AddDynamic(this, & UStartMenuWidget::HandleSettingsClicked);
	}

	if (quitButton)
	{
		quitButton->OnClicked.AddDynamic(this, & UStartMenuWidget::HandleQuitClicked);
	}
}

void UStartMenuWidget::HandleStartClicked()
{
	if (manager)
	{
		// StartLevelManager���� ���� �̵� ó��
		manager->OnStartGame(); 
	}
}

void UStartMenuWidget::HandleSettingsClicked()
{
	if (manager)
	{
		// StartLevelManager���� ����â ���
		manager->OnOpenSettings(); 
	}
}

void UStartMenuWidget::HandleQuitClicked()
{
	if (manager)
	{
		// ���� ����
		manager->OnQuitGame(); 
	}
}
