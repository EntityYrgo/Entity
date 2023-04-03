// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"
#include "DebugLog.h"
#include "Console.h"
#include "Menu.h"
#include "Level.h"
#include "GameEvent.h"
#include "Player.h"
#include "Main.h"
#include "Game.h"
#include "SessionMatchmaking.h"

const double MaxTimeToShutdown = 7.0; //7 seconds

FGame::FGame() noexcept(false)
{
	Menu = std::make_unique<FMenu>(Console);
	Level = std::make_unique<FLevel>();
	SessionMatchmaking = std::make_unique<FSessionMatchmaking>();

	CreateConsoleCommands();
}

FGame::~FGame()
{
}

void FGame::Update()
{
	SessionMatchmaking->Update();
	FBaseGame::Update();
}

void FGame::Create()
{
	FBaseGame::Create();

	SessionMatchmaking->SubscribeToGameInvites();
}

void FGame::OnGameEvent(const FGameEvent& Event)
{
	FBaseGame::OnGameEvent(Event);

	SessionMatchmaking->OnGameEvent(Event);
}

void FGame::OnShutdown()
{
	//SessionMatchmaking must be cleared before we destroy SDK platform.
	if (SessionMatchmaking)
	{
		SessionMatchmaking->OnShutdown();
	}

	FBaseGame::OnShutdown();

	ShutdownTriggeredTimestamp = Main->GetTimer().GetTotalSeconds();
}

bool FGame::IsShutdownDelayed()
{
	if (SessionMatchmaking && (Main->GetTimer().GetTotalSeconds() - ShutdownTriggeredTimestamp) < MaxTimeToShutdown)
	{
		return SessionMatchmaking->HasActiveLocalSessions();
	}

	return false;
}

const std::unique_ptr<FSessionMatchmaking>& FGame::GetSessions()
{
	return SessionMatchmaking;
}
