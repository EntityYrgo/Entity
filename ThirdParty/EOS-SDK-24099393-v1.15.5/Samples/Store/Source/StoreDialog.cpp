// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"
#include "Game.h"
#include "TextLabel.h"
#include "Button.h"
#include "UIEvent.h"
#include "GameEvent.h"
#include "AccountHelpers.h"
#include "Player.h"
#include "OfferList.h"
#include "StoreDialog.h"

FStoreDialog::FStoreDialog(
	Vector2 DialogPos,
	Vector2 DialogSize,
	UILayer DialogLayer,
	FontPtr DialogNormalFont,
	FontPtr DialogSmallFont,
	FontPtr DialogTinyFont) :
	FDialog(DialogPos, DialogSize, DialogLayer)
{
	OfferListWidget = std::make_shared<FOfferListWidget>(
		Position,
		Size,
		DialogLayer,
		DialogNormalFont,
		DialogSmallFont,
		DialogSmallFont,
		DialogTinyFont);
#if defined(EOS_SAMPLE_SESSIONS) || defined(EOS_SAMPLE_P2P)
	OfferListWidget->SetBottomOffset(0.0f);
#else
	OfferListWidget->SetBottomOffset(100.0f);
#endif

	OfferListWidget->SetBorderColor(Color::UIBorderGrey);

	OfferListWidget->Create();
	AddWidget(OfferListWidget);
}

void FStoreDialog::SetPosition(Vector2 Pos)
{
	IWidget::SetPosition(Pos);

	if (OfferListWidget)
	{
		OfferListWidget->SetPosition(Pos);
	}
}

void FStoreDialog::SetSize(Vector2 NewSize)
{
	IWidget::SetSize(NewSize);

	if (OfferListWidget)
	{
		OfferListWidget->SetSize(NewSize);
	}
}

void FStoreDialog::OnGameEvent(const FGameEvent& Event)
{
	if (Event.GetType() == EGameEventType::UserLoggedIn)
	{
		SetOfferInfoVisible(true);
	}
	else if (Event.GetType() == EGameEventType::UserLoginRequiresMFA)
	{
		SetOfferInfoVisible(false);
		SetFocused(false);
	}
	else if (Event.GetType() == EGameEventType::UserLoginEnteredMFA)
	{
		SetOfferInfoVisible(true);
	}
	else if (Event.GetType() == EGameEventType::UserLoggedOut)
	{
		if (FPlayerManager::Get().GetNumPlayers() == 0)
		{
			Clear();
			SetOfferInfoVisible(false);
		}
	}
	else if (Event.GetType() == EGameEventType::ShowPrevUser)
	{
		Clear();
		Reset();
	}
	else if (Event.GetType() == EGameEventType::ShowNextUser)
	{
		Clear();
		Reset();
	}
	else if (Event.GetType() == EGameEventType::NewUserLogin)
	{
		SetOfferInfoVisible(false);
		Clear();
	}
	else if (Event.GetType() == EGameEventType::CancelLogin)
	{
		Clear();
		Reset();
	}
}

void FStoreDialog::SetOfferInfoVisible(bool bVisible)
{
	if (OfferListWidget)
	{
		OfferListWidget->SetOfferInfoVisible(bVisible);
	}
}

void FStoreDialog::Reset()
{
	if (OfferListWidget)
	{
		OfferListWidget->SetOfferInfoVisible(true);
		OfferListWidget->Reset();
	}
}

void FStoreDialog::Clear()
{
	if (OfferListWidget)
	{
		OfferListWidget->RefreshOfferData(std::vector<FOfferData>());
		OfferListWidget->ClearFilter();
		OfferListWidget->Reset();
	}
}
