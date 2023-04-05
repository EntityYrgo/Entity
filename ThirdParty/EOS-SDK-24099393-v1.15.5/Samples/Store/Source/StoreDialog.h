// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Dialog.h"
#include "Font.h"

/**
 * Forward declarations
 */
class FOfferListWidget;
class FGameEvent;

/**
 * Store dialog
 */
class FStoreDialog : public FDialog
{
public:
	/**
	 * Constructor
	 */
	FStoreDialog(Vector2 DialogPos,
		Vector2 DialogSize,
		UILayer DialogLayer,
		FontPtr DialogNormalFont,
		FontPtr DialogSmallFont,
		FontPtr DialogTinyFont);

	/**
	 * Destructor
	 */
	virtual ~FStoreDialog() {};

	/** IWidget */
	virtual void SetPosition(Vector2 Pos) override;
	virtual void SetSize(Vector2 NewSize) override;

	/**
	* Receives game event
	*
	* @param Event - Game event to act on
	*/
	void OnGameEvent(const FGameEvent& Event);

	/** Sets visibility of offer info */
	void SetOfferInfoVisible(bool bVisible);

	/** Reset */
	void Reset();

	/** Clear */
	void Clear();
	
private:
	/** Offer List */
	std::shared_ptr<FOfferListWidget> OfferListWidget;
};
