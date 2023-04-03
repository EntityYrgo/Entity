// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"
#include "DebugLog.h"
#include "Console.h"
#include "Menu.h"
#include "Level.h"
#include "GameEvent.h"
#include "Platform.h"
#include "Main.h"
#include "Game.h"
#include "Lobbies.h"

const double MaxTimeToShutdown = 7.0; //7 seconds


FGame::FGame() noexcept(false)
{
	Menu = std::make_unique<FMenu>(Console);
	Level = std::make_unique<FLevel>();
	Lobbies = std::make_unique<FLobbies>();

	CreateConsoleCommands();
}

FGame::~FGame()
{
	
}

void FGame::CreateConsoleCommands()
{
	FBaseGame::CreateConsoleCommands();

	if (Console)
	{
		const std::vector<const wchar_t*> ExtraHelpMessageLines =
		{
			L" CURRENTLOBBY - to print out current lobby info;",
			L" FINDLOBBY - to perform a lobby search;",
		};
		AppendHelpMessageLines(ExtraHelpMessageLines);

		Console->AddCommand(L"CURRENTLOBBY", [](const std::vector<std::wstring>&)
		{
			if (FPlatform::IsInitialized())
			{
				if (FGame::Get().GetLobbies())
				{
					if (FGame::Get().GetLobbies()->GetCurrentLobby().IsValid())
					{
						FDebugLog::Log(L"Current lobby id: %ls", FStringUtils::Widen(FGame::Get().GetLobbies()->GetCurrentLobby().Id).c_str());
					}
					else
					{
						FDebugLog::LogError(L"No current lobby.");
					}
				}
				else
				{
					FDebugLog::LogError(L"EOS SDK Lobbies are not initialized!");
				}
			}
			else
			{
				FDebugLog::LogError(L"EOS SDK is not initialized!");
			}
		});
		Console->AddCommand(L"FINDLOBBY", [](const std::vector<std::wstring>& args)
		{
			if (FPlatform::IsInitialized())
			{
				if (FGame::Get().GetLobbies())
				{
					if (args.size() == 1)
					{
						FGame::Get().GetLobbies()->Search(FStringUtils::Narrow(args[0]), 1);
					}
					else
					{
						FDebugLog::LogError(L"Lobby id is required as the only argument.");
					}
				}
				else
				{
					FDebugLog::LogError(L"EOS SDK Lobbies are not initialized!");
				}
			}
			else
			{
				FDebugLog::LogError(L"EOS SDK is not initialized!");
			}
		});

	}
}

void FGame::Update()
{
	FBaseGame::Update();

	Lobbies->Update();
}



void FGame::Create()
{
	FBaseGame::Create();

	Lobbies->SubscribeToLobbyInvites();
	Lobbies->SubscribeToLobbyUpdates();
}

void FGame::OnGameEvent(const FGameEvent& Event)
{
	FBaseGame::OnGameEvent(Event);
	Lobbies->OnGameEvent(Event);
}

void FGame::OnShutdown()
{
	//Lobbies must be cleared before we destroy SDK platform.
	if (Lobbies)
	{
		Lobbies->OnShutdown();
	}

	FBaseGame::OnShutdown();

	ShutdownTriggeredTimestamp = Main->GetTimer().GetTotalSeconds();
}

bool FGame::IsShutdownDelayed()
{
	if (Lobbies && (Main->GetTimer().GetTotalSeconds() - ShutdownTriggeredTimestamp) < MaxTimeToShutdown)
	{
		return !Lobbies->IsReadyToShutdown();
	}

	return false;
}

const std::unique_ptr<FLobbies>& FGame::GetLobbies()
{
	return Lobbies;
}
