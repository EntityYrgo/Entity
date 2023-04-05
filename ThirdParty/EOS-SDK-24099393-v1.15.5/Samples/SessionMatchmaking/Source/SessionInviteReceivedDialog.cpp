// Copyright Epic Games, Inc. All Rights Reserved.

#include "pch.h"
#include "Game.h"
#include "Menu.h"
#include "Sprite.h"
#include "TextLabel.h"
#include "Button.h"
#include "Checkbox.h"
#include "ProgressBar.h"
#include "SessionInviteReceivedDialog.h"
#include "StringUtils.h"

FSessionInviteReceivedDialog::FSessionInviteReceivedDialog(Vector2 InPos,
						 Vector2 InSize,
						 UILayer InLayer,
						 FontPtr InNormalFont,
						 FontPtr InSmallFont) :
	FDialog(InPos, InSize, InLayer)
{
	Background = std::make_shared<FSpriteWidget>(
		Position,
		InSize,
		InLayer - 1,
		L"Assets/textfield.dds",
		Color::Black);
	AddWidget(Background);

	Label = std::make_shared<FTextLabelWidget>(
		Vector2(Position.x + 30.0f, Position.y + 25.0f),
		Vector2(150.f, 30.f),
		InLayer - 1,
		L"Session invite received from: ",
		L"");
	Label->SetFont(InNormalFont);
	AddWidget(Label);

	PresenceSessionCheckbox = std::make_shared<FCheckboxWidget>(
		Label->GetPosition() + Vector2(Label->GetSize().x / 2.f + 5.f, Label->GetSize().y + 10.0f),
		Vector2(150.0f, 30.0f),
		Layer - 1,
		L"Presence?",
		L"",
		InNormalFont
		);
	AddWidget(PresenceSessionCheckbox);

	FColor AcceptButtonCol = FColor(0.f, 0.47f, 0.95f, 1.f);
	AcceptInviteButton = std::make_shared<FButtonWidget>(
		Position + Vector2(InSize.x / 2.0f - 105.0f, InSize.y - 45.f),
		Vector2(100.f, 30.f),
		InLayer - 1,
		L"Accept",
		assets::DefaultButtonAssets,
		InSmallFont,
		AcceptButtonCol);
	AcceptInviteButton->SetOnPressedCallback([this]()
	{
		const FSession& Session = GetSession();
		if (Session.IsValid())
		{
			FGame::Get().GetSessions()->JoinSession(FGame::Get().GetSessions()->GetInviteSessionHandle(), PresenceSessionCheckbox->IsTicked());
		}
		Hide();
	});
	AcceptInviteButton->SetBackgroundColors(assets::DefaultButtonColors);
	AddWidget(AcceptInviteButton);

	FColor DeclineButtonCol = FColor(0.f, 0.47f, 0.95f, 1.f);
	DeclineInviteButton = std::make_shared<FButtonWidget>(
		Vector2(Position.x + InSize.x / 2.0f + 5.0f, AcceptInviteButton->GetPosition().y),
		Vector2(100.f, 30.f),
		InLayer - 1,
		L"Decline",
		assets::DefaultButtonAssets,
		InSmallFont,
		DeclineButtonCol);
	DeclineInviteButton->SetOnPressedCallback([this]()
	{
		Hide();
	});
	DeclineInviteButton->SetBackgroundColors(assets::DefaultButtonColors);
	AddWidget(DeclineInviteButton);
}

void FSessionInviteReceivedDialog::SetPosition(Vector2 Pos)
{
	IWidget::SetPosition(Pos);

	Background->SetPosition(Position);
	Label->SetPosition(Position + Vector2(30.0f, 25.0f));
	PresenceSessionCheckbox->SetPosition(Label->GetPosition() + Vector2(Label->GetSize().x / 2.0f + 5.0f, Label->GetSize().y + 10.0f));
	AcceptInviteButton->SetPosition(Position + Vector2(GetSize().x / 2.0f - AcceptInviteButton->GetSize().x - 5.0f, GetSize().y - 45.0f));
	DeclineInviteButton->SetPosition(Position + Vector2(GetSize().x / 2.0f + 5.0f, GetSize().y - 45.0f));
}

void FSessionInviteReceivedDialog::SetSessionInfo(const std::wstring& InFriendName, const FSession& InSession)
{
	FriendName = InFriendName;
	Session = InSession;

	std::wstring LevelName;
	for (FSession::Attribute& Attr : Session.Attributes)
	{
		if (Attr.Key == "Level" || Attr.Key == "LEVEL")
		{
			LevelName = FStringUtils::Widen(Session.Attributes[0].AsString);
		}
	}

	if (Label)
	{
		Label->ClearText();
		Label->SetText(std::wstring(L"Session invite received from: ") + InFriendName + L" Level: " + LevelName);
	}

	if (FGame::Get().GetSessions()->HasPresenceSession())
	{
		PresenceSessionCheckbox->Disable();
		PresenceSessionCheckbox->SetTicked(false);
	}
	else
	{
		PresenceSessionCheckbox->Enable();
		PresenceSessionCheckbox->SetTicked(true);
	}
}

void FSessionInviteReceivedDialog::OnEscapePressed()
{
	Hide();
}