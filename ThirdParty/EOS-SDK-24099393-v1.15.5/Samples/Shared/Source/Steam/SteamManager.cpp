// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"

#ifdef EOS_STEAM_ENABLED
#include "DebugLog.h"
#include "StringUtils.h"
#include "Game.h"
#include "GameEvent.h"
#include "Authentication.h"
#include "Users.h"
#include "SteamManager.h"
#include "CommandLine.h"

#include "steam/steam_api.h"

/**
 * Steam Manager Implementation
 */
class FSteamManager::FImpl
{
public:
	FImpl();
	~FImpl();

	void Init();
	void Update();
	void RetrieveAuthSessionTicket();
	void OnGetAuthSessionTicket(GetAuthSessionTicketResponse_t* pAuthSessionTicketResponse);

	void OnLoginComplete();
	void StartLogin();

private:

	void CleanupResources();

	CCallbackManual<FSteamManager::FImpl, GetAuthSessionTicketResponse_t> SteamCallbackGetAuthSessionTicket;
	/** Auth session ticket converted to Hex, to pass to the EOS SDK */
	std::wstring AuthSessionTicket;
	/** We need to keep hold of this and then call ISteamUser::CancelAuthTicket when done with the login */
	HAuthTicket AuthSessionTicketHandle;
	bool bIsInitialized = false;
};

FSteamManager::FImpl::FImpl()
{
	SteamCallbackGetAuthSessionTicket.Register(this, &FImpl::OnGetAuthSessionTicket);
}

FSteamManager::FImpl::~FImpl()
{
	SteamCallbackGetAuthSessionTicket.Unregister();
}

void FSteamManager::FImpl::Init()
{
	if (SteamAPI_Init())
	{
		FDebugLog::Log(L"Steam - Initialized");

		bIsInitialized = true;

		RetrieveAuthSessionTicket();
	}
	else
	{
		FDebugLog::LogError(L"Steam must be running to play this game (SteamAPI_Init() failed)");
	}
}

void FSteamManager::FImpl::Update()
{
	if (!bIsInitialized)
	{
		return;
	}

	// Run Steam client callbacks
	SteamAPI_RunCallbacks();
}

void FSteamManager::FImpl::RetrieveAuthSessionTicket()
{
	if (!bIsInitialized)
	{
		return;
	}

	FDebugLog::Log(L"Steam - Requesting Auth Session Ticket ...");

	// Steamworks (https://partner.steamgames.com/doc/api/ISteamUser#GetAuthSessionTicket) API says a size of 1024 is enough
	// unless there is a large amount of available DLC, so lets use a large enough buffer
	std::vector<uint8> RawAuthSessionTicket(4096);
	uint32 AuthSessionTicketSize = 0;
	AuthSessionTicketHandle = SteamUser()->GetAuthSessionTicket(RawAuthSessionTicket.data(), (int)RawAuthSessionTicket.size(), &AuthSessionTicketSize);
	if (AuthSessionTicketHandle == k_HAuthTicketInvalid || AuthSessionTicketSize == 0)
	{
		FDebugLog::LogError(L"Steam - Unable to get the auth session ticket.");
		return;
	}
	// Clamp to the right size
	RawAuthSessionTicket.resize(AuthSessionTicketSize);

	uint32 StringBufSize = ((uint32)RawAuthSessionTicket.size() * 2) + 1;
	std::vector<char> NarrowAuthSessionTicket(StringBufSize);
	uint32_t OutLen = StringBufSize;
	EOS_EResult ConvResult = EOS_ByteArray_ToString(RawAuthSessionTicket.data(), RawAuthSessionTicket.size(), NarrowAuthSessionTicket.data(), &OutLen);
	if (ConvResult != EOS_EResult::EOS_Success)
	{
		FDebugLog::LogError(L"Steam - RetrieveAuthSessionTicket - EOS_ByteArray_ToString failed - Result: %ls", FStringUtils::Widen(EOS_EResult_ToString(ConvResult)).c_str());
		CleanupResources();
		return;
	}

	assert(OutLen == StringBufSize);
	AuthSessionTicket = FStringUtils::Widen(std::string(NarrowAuthSessionTicket.data(), NarrowAuthSessionTicket.size()));

	// NOTE: Not starting the login straight away because according to Steamworks's documentation, we should only start using the
	// auth session ticket once we get the GetAuthSessionTicketResponse_t callback.
}

void FSteamManager::FImpl::CleanupResources()
{
	AuthSessionTicket.clear();
	// Once login finished(successfully or not), or the actual steam callback returned an error  we need to call Steamwork's ISteamUser::CancelAuthTicket
	if (AuthSessionTicketHandle != k_HAuthTicketInvalid)
	{
		SteamUser()->CancelAuthTicket(AuthSessionTicketHandle);
		AuthSessionTicketHandle = k_HAuthTicketInvalid;
	}
}

void FSteamManager::FImpl::OnGetAuthSessionTicket(GetAuthSessionTicketResponse_t* pAuthSessionTicketResponse)
{
	// GetAuthSessionTicketResponse_t is broadcast to all listeners, so if we get the wrong handle, it doesn't necessarily mean it's an error,
	// since maybe some other code is also processing auth session tickets.
	if (pAuthSessionTicketResponse->m_hAuthTicket != AuthSessionTicketHandle)
	{
		FDebugLog::LogWarning(L"Steam - Ignoring unexpected GetAuthSessionTicketResponse_t callback");
		return;
	}

	if (pAuthSessionTicketResponse->m_eResult != k_EResultOK)
	{
		FDebugLog::LogError(L"Steam - GetAuthSessionTicketResponse_t callback failed. Error %d", (int)pAuthSessionTicketResponse->m_eResult);
		CleanupResources();
		return;
	}

	StartLogin();
}

void FSteamManager::FImpl::StartLogin()
{
	if (!AuthSessionTicket.empty())
	{
		FDebugLog::Log(L"Steam - StartLogin - Auth Session Ticket: %ls", AuthSessionTicket.c_str());

		FGameEvent Event(EGameEventType::StartUserLogin, AuthSessionTicket, (int)ELoginMode::ExternalAuth, (int)ELoginExternalType::Steam);
		FGame::Get().OnGameEvent(Event);
	}
	else
	{
		FDebugLog::LogError(L"Steam - StartLogin - Invalid Steam Auth Session Ticket");
	}
}

void FSteamManager::FImpl::OnLoginComplete()
{
	CleanupResources();
}

std::unique_ptr<FSteamManager> FSteamManager::Instance;

FSteamManager::FSteamManager()
	: Impl(new FImpl())
{

}

FSteamManager::~FSteamManager()
{

}

FSteamManager& FSteamManager::GetInstance()
{
	if (!Instance)
	{
		Instance = std::unique_ptr<FSteamManager>(new FSteamManager());
	}

	return *Instance;
}

void FSteamManager::ClearInstance()
{
	Instance.reset();
}

void FSteamManager::Init()
{
	Impl->Init();
}

void FSteamManager::Update()
{
	Impl->Update();
}

void FSteamManager::RetrieveAuthSessionTicket()
{
	Impl->RetrieveAuthSessionTicket();
}

void FSteamManager::StartLogin()
{
	Impl->StartLogin();
}

void FSteamManager::OnGameEvent(const FGameEvent& Event)
{
	if (Event.GetType() == EGameEventType::UserInfoRetrieved)
	{
		FEpicAccountId UserId = Event.GetUserId();

		// Log Steam Display Name
		std::wstring DisplayName = FGame::Get().GetUsers()->GetExternalAccountDisplayName(UserId, UserId, EOS_EExternalAccountType::EOS_EAT_STEAM);
		if (!DisplayName.empty())
		{
			FDebugLog::Log(L"[EOS SDK] External Account Display Name: %ls", DisplayName.c_str());
		}
		else
		{
			FDebugLog::LogError(L"[EOS SDK] External Account Display Name Not Found");
		}
	}
	else if ((Event.GetType() == EGameEventType::UserLoggedIn) || (Event.GetType() == EGameEventType::UserLoginFailed))
	{
		Impl->OnLoginComplete();
	}
}

#endif //EOS_STEAM_ENABLED