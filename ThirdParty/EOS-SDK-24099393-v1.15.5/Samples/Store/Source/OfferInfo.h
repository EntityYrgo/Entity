// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widget.h"
#include "Font.h"

/**
* Forward declarations
*/
class FUIEvent;
class FButtonWidget;
class FTextLabelWidget;
class FSpriteWidget;

/**
* Offers info widget
*/
class FOfferInfoWidget : public IWidget
{
public:
	/**
	 * Constructor
	 */
	FOfferInfoWidget(Vector2 InfoPos,
		Vector2 InfoSize,
		UILayer InfoLayer,
		FOfferData OfferData,
		FontPtr InfoLargeFont,
		FontPtr InfoSmallFont);

	/**
	* No copying or copy assignment allowed for this class.
	*/
	FOfferInfoWidget(const FOfferInfoWidget&) = delete;
	FOfferInfoWidget& operator=(const FOfferInfoWidget&) = delete;

	/**
	 * Destructor
	 */
	virtual ~FOfferInfoWidget() {};

	/** IGfxComponent */
	virtual void Create() override;
	virtual void Release() override;
	virtual void Update() override;
	virtual void Render(FSpriteBatchPtr& Batch) override;
#ifdef _DEBUG
	virtual void DebugRender() override;
#endif

	/** IWidget */
	virtual void OnUIEvent(const FUIEvent& Event) override;
	virtual void SetPosition(Vector2 Pos) override;
	virtual void SetSize(Vector2 NewSize) override;
	virtual void SetFocused(bool bFocused) override;

	/**
	 * Sets Offer data
	 *
	 * @param Data - Offer data to copy
	 */
	void SetOfferData(const FOfferData& Data);

private:
	/** Offer Data */
	FOfferData OfferData;

	/** Background Image */
	std::shared_ptr<FSpriteWidget> BackgroundImage;

	/** Name Label */
	std::shared_ptr<FTextLabelWidget> NameLabel;

	/** Button one */
	std::shared_ptr<FButtonWidget> Button1;

	/** Button two */
	std::shared_ptr<FButtonWidget> Button2;

	/** Large Font */
	FontPtr LargeFont;

	/** Small Font */
	FontPtr SmallFont;
};
