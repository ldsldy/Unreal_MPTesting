// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MPTESTING_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// PostLogin은 새로운 플레이어가 게임에 접속할 때마다 호출되는 함수(플레이어 컨트롤러에 접근)
	virtual void PostLogin(APlayerController* NewPlayer) override;
	// Logout은 플레이어가 게임에서 나갈 때 호출되는 함수(세션에서 컨트롤러를 제거)
	virtual void Logout(AController* Exiting) override;
};
