// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

//
// 메뉴에서 바인딩할 델리게이트 선언
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSesssionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionsSubsystem();

	//
	// 세션 함수를 위한 핸들러 선언
	//
	
	// 서브시스템에 접근하여 세션 생성 함수 호출(세션 생성 정보, 플레이어 수를 지정)
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	// 세션을 찾아서 어떤 세션을 조인할지 결정
	void FindSessions(int32 MaxSerachResults);
	// 검색 결과를 가져오고
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	// 현재 세션을 파괴
	void DestroySession();
	// 세션 시작
	void StartSession();

	//
	// 메뉴에서 바인딩할 델리게이트들
	//
	FMultiplayerOnCreateSessionComplete MultiPlayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete MultiPlayerOnFindSessionsComplete;
	FMultiplayerOnJoinSesssionComplete MultiplayerOnJoinSesssionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

protected: 
	//
	// 델리게이트 리스트를 위한 콜백 함수들
	// 클래스 밖에서 호출될 일 없음
	//
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bwasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName InSessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings; // 나중에 세팅을 바꿀 수 있도록

	//
	// Online Session Interface 델리게이트 리스트
	// 여기에 있는 함수들을 MultiplayerSessionSubsystem 콜백 함수로 바인딩
	// 
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;  // 델리게이트 핸들러 : 델리게이트를 바인딩/언바인딩 할 때 사용하는 핸들러
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

};
