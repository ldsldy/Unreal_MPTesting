// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		// 게임 스테이트에 접근하여 현재 세션에 접속한 플레이어 수를 획득
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if(GEngine)
		{
			// 디버그 메시지로 현재 접속한 플레이어 수를 출력
			GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Green,
				FString::Printf(TEXT("Number of Players in Lobby: %d"), NumberOfPlayers));

			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			if (PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Green,
					FString::Printf(TEXT("%s has joined the game!"), *PlayerName));
			}

			
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		// 디버그 메시지로 현재 접속한 플레이어 수를 출력
		// -1을 하는 이유는 아직 플레이어의 로그아웃이 GameState의 PlayerArray에서 반영되지 않았기 때문
		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Green,
			FString::Printf(TEXT("Number of Players in Lobby: %d"), NumberOfPlayers - 1));

		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Yellow,
			FString::Printf(TEXT("%s has existed the game!"), *PlayerName));


	}
}
