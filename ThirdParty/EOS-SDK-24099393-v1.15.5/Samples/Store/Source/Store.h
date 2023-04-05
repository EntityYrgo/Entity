// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "eos_sdk.h"

struct FEntitlementData
{
	/** User associated with this entitlement */
	FEpicAccountId UserId;
	/** The EOS_Ecom_EntitlementName */
	std::string Name;
	/** The EOS_Ecom_EntitlementInstanceId */
	std::string InstanceId;
	/** If true then this entitlement has been retrieved */
	bool bRedeemed;
};

struct FOfferData
{
	/** User associated with this offer */
	FEpicAccountId UserId;
	/** The EOS_Ecom_CatalogOfferId */
	std::string Id;
	/** The title of the offer */
	std::wstring Title;
	/** True if the price was properly retrieved */
	bool bPriceValid;
	/** The current price of the offer (includes discounts) */
	uint64_t CurrentPrice;
	/** The original price of the offer */
	uint64_t OriginalPrice;
	/** The decimal point for given price. */
	uint32_t DecimalPoint;
};

/** Provides access to EOS_Ecom with an emphasis on the Catalog */
class FStore
{
public:
	FStore() noexcept(false);
	FStore(FStore const&) = delete;
	FStore& operator=(FStore const&) = delete;

	virtual ~FStore();

	/** Query the store for offer info and entitlement info */
	void Update();

	/** Buy an offer */
	void Checkout(const std::string& OfferId);

	/** Redeem an entitlement */
	// void RedeemEntitlement(const FEntitlementData& Entitlement);

	/** Retrieves the catalog cache */
	const std::vector<FOfferData>& GetCatalog() const { return Catalog; }
	/** Retrieves the entitlements cache */
	const std::vector<FEntitlementData>& GetEntitlements() const { return Entitlements; }

	/** Has the catalog data updated? */
	bool IsDirty() { return bDirty; };
	/** Change the dirty flag directly */
	void SetDirty(bool bNewIsDirty) { bDirty = bNewIsDirty; };

	/** Set the user currently associated with the store */
	void SetCurrentUser(FEpicAccountId UserId) { CurrentUserId = UserId; };
	/** Get the user currently associated with the store */
	FEpicAccountId GetCurrentUser() { return CurrentUserId; };

	/** Notify the store of game events. */
	void OnGameEvent(const FGameEvent& Event);

private:
	/** Respond to the logged in game event */
	void OnLoggedIn(FEpicAccountId UserId);
	/** Respond to the logged out game event */
	void OnLoggedOut(FEpicAccountId UserId);

	/** Start a store query for the user */
	void QueryStore(EOS_EpicAccountId LocalUserId);
	/** Start a entitlement query for the user */
	void QueryEntitlements(EOS_EpicAccountId LocalUser);

	/** Set the catalog data */
	void SetCatalog(FEpicAccountId InUserId, std::vector<FOfferData>&& InCatalog);
	/** Set the entitlement data */
	void SetEntitlements(FEpicAccountId InUserId, std::vector<FEntitlementData>&& InEntitlements);

	/** The cached data dirty flag */
	bool bDirty = false;
	/** How long until the next update */
	float UpdateStoreTimer = 0.f;
	/** The current user associated with the store */
	FEpicAccountId CurrentUserId;

	/** The current catalog cache */
	std::vector<FOfferData> Catalog;
	/** The current entitlement cache */
	std::vector<FEntitlementData> Entitlements;

	/** The desired store update time */
	static constexpr float UpdateStoreTimeoutSeconds = 300.f;

	/** Static callback handler for Checkout complete */
	static void EOS_CALL CheckoutCompleteCallbackFn(const EOS_Ecom_CheckoutCallbackInfo* CheckoutData);

	/** Static callback handler for Query Offer complete */
	static void EOS_CALL QueryStoreCompleteCallbackFn(const EOS_Ecom_QueryOffersCallbackInfo* OfferData);

	/** Static callback handler for Query Entitlement complete */
	static void EOS_CALL QueryEntitlementsCompleteCallbackFn(const EOS_Ecom_QueryEntitlementsCallbackInfo* EntitlementData);
};
