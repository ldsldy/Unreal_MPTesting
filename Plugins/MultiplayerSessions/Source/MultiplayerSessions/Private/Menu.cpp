// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible); // 메뉴를 보이도록 설정
	bIsFocusable = true;					  // 메뉴가 포커스를 받을 수 있도록 설정

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			// 입력모드를 UI로만 설정 => 입력이 다른 어떤 폰에도 전달되지 않음
			// 마우스 커서는 잠기지 않고 뷰포트 내에서 자유롭게 움직일 수 있음.
			// 마우스 커서를 보이도록 설정
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// 게임 인스턴스 서브시스템은 게임 인스턴스가 존재할 때만 존재
	// 따라서 게임 인스턴스를 가져와서 서브시스템에 접근
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::HostButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Button Clicked"));
	
	if (MultiplayerSessionsSubsystem)
	{
		// 세션 생성 함수 호출
		MultiplayerSessionsSubsystem->CreateSession(4, FString("FreeForAll"));
	}
}

void UMenu::JoinButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Button Clicked"));
}
