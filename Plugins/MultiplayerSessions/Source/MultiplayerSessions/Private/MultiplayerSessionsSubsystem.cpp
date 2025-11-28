// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	// 현재 사용중인 온라인 서브시스템(Steam ...)을 가져옴
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		// 온라인 게임 세션 인터페이스(세션 생성, 관리 및 소멸 처리)를 가져옴
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// 세션 생성이 완료되면 델리게이트를 델리게이트 핸들러를 이용하여 제거
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// "세션 생성 완료" 델리게이트를 브로드캐스트
	MultiPlayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bwasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCancelFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// 추가적인 검사를 통해 다시 한번 세션 검색 결과를 확인
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiPlayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	MultiPlayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bwasSuccessful);
}
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	MultiplayerOnJoinSesssionComplete.Broadcast(Result);
}
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

}
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName InSessionName, bool bWasSuccessful)
{

}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// 이미 존재하는 세션이 있으면 삭제
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;
		
		DestroySession();
	}

	// 델리게이트를 델리게이트 핸들에 저장하여 나중에 델리게이트를 제거할 수 있도록 함
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	// 세션 설정 구성
	// LAN 매치 여부 (온라인 서브 시스템이 NULL인 경우 LAN 매치로 간주)
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;	// 공개 플레이어 수
	LastSessionSettings->bAllowJoinInProgress = true;					// 진행 중인 참가 허용
	LastSessionSettings->bAllowJoinViaPresence = true;					// 참가 조건
	LastSessionSettings->bShouldAdvertise = true;						// 세션 광고 여부
	LastSessionSettings->bUsesPresence = true;							// 프레즌스(사용자의 현재 상태 정보) 사용 여부
	LastSessionSettings->bUseLobbiesIfAvailable = true;					// 로비 사용 여부
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // 연결을 위한 매치 타입 설정
	LastSessionSettings->BuildUniqueId = 1;								// 고유 ID 빌드

	// 로컬 플레이어의 네트워크 ID와 세션 세팅을 사용하여 세션 생성 시도
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// 세션 생성 실패 시에 델리게이트를 델리게이트 핸들를 이용하여 제거
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		
		// "세션 생성 실패" 델리게이트를 브로드캐스트
		MultiPlayerOnCreateSessionComplete.Broadcast(false);
	}
}
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		return;
	}
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		
		// "세션 찾기 실패" 델리게이트를 빈 세션 검색 결과 배열과 함께 브로드캐스트
		MultiPlayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	
}
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		// 온라인 세션 인터페이스가 유효하지 않으면
		// "알 수 없는 오류로 세션 참가 실패" 델리게이트 브로드캐스트
		MultiplayerOnJoinSesssionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		// "알 수 없는 오류로 세션 참가 실패" 델리게이트 브로드캐스트		
		MultiplayerOnJoinSesssionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}

}
void UMultiplayerSessionsSubsystem::DestroySession()
{
	if(!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		//파괴 실패
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
}
void UMultiplayerSessionsSubsystem::StartSession()
{

}