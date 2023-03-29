// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "eos_sdk.h"
#include "eos_auth.h"
#include "eos_logging.h"

#include "AccountHelpers.h"

/** Login Mode */
enum class ELoginMode
{
	/** Login using a user id and password token */
	IDPassword,

	/** Login using an exchange code */
	ExchangeCode,

	/** Login using a device code */
	DeviceCode,

	/** Login using credentials from the EOS SDK Dev Auth Tool */
	DevAuth,

	/** Login using the account portal */
	AccountPortal,

	/** Login using persistent auth */
	PersistentAuth,

	/** Login using external auth */
	ExternalAuth
};

/** Login External Type */
enum class ELoginExternalType
{
	/** None */
	None,

	/** Login using Steam */
	Steam
};

/**
* Forward declarations
*/
struct FUserData;
class FGameEvent;
struct FConnectLoginContext;

/**
* Manages all user authentication
*/
class FAuthentication
{
public:
	/**
	* Constructor
	*/
	FAuthentication() noexcept(false);

	/**
	* No copying or copy assignment allowed for this class.
	*/
	FAuthentication(FAuthentication const&) = delete;
	FAuthentication& operator=(FAuthentication const&) = delete;

	/**
	* Destructor
	*/
	virtual ~FAuthentication();

	/**
	* Attempts to login user with given credentials
	*
	* @param LoginMode - Login mode
	* @param UserId - Id of the user logging in
	* @param UserToken - Credentials or token related to the user logging in
	* @returns True in case of success.
	*/
	bool Login(ELoginMode LoginMode, std::wstring UserId, std::wstring UserToken, ELoginExternalType ExternalType = ELoginExternalType::None);
		
	/**
	* Logs user out
	*
	* @param UserId - Account Id of the user to log out
	*/
	void Logout(FEpicAccountId UserId);

	/**
	* Continues previous login attempt using a previously stored continuance token
	*
	* @param ContinuanceToken - Continuance token used to link external account to continue login
	*/
	void ContinueLogin(EOS_ContinuanceToken ContinuanceToken);

	/**
	* Shuts down EOS SDK
	*/
	void Shutdown();

	/**
	* Receives game event
	*
	* @param Event - Game event to act on
	*/
	void OnGameEvent(const FGameEvent& Event);

private:
	/**
	* Checks command line for auto login and starts login if found
	*/
	void CheckAutoLogin();

	/**
	 * Login with Connect authentication
	 *
	 * @param UserId - UserId to login
	 */
	void ConnectLogin(FEpicAccountId UserId);

	/**
	 * Adds notification for login status change
	 */
	void AddNotifyLoginStatusChanged();

	/**
	 * Removes notification for login status change
	 */
	void RemoveNotifyLoginStatusChanged();

	/**
	 * Adds expiration notification for connect authentication
	 */
	void AddConnectAuthExpirationNotification();

	/**
	 * Removes expiration notification for connect authentication
	 */
	void RemoveConnectAuthExpirationNotification();

	/**
	* Callback that is fired when the login operation completes, either successfully or in error
	*
	* @param Data - Output parameters for the EOS_Auth_Login Function
	*/
	static void EOS_CALL LoginCompleteCallbackFn(const EOS_Auth_LoginCallbackInfo* Data);

	/**
	* Callback that is fired when the link account operation completes, either successfully or in error
	*
	* @param Data - Output parameters for the EOS_Auth_LinkAccount Function
	*/
	static void EOS_CALL LinkAccountCompleteCallbackFn(const EOS_Auth_LinkAccountCallbackInfo* Data);

	/**
	* Callback that is fired when the delete persistent auth operation completes, either successfully or in error
	*
	* @param Data - Output parameters for the EOS_Auth_DeletePersistentAuth Function
	*/
	static void EOS_CALL DeletePersistentAuthCompleteCallbackFn(const EOS_Auth_DeletePersistentAuthCallbackInfo* Data);

	/**
	 * Deletes any locally stored persistent auth credentials for the currently logged in user of the local device.
	 */
	void DeletePersistentAuth();

	/**
	* callback that is fired when the logout operation completes, either successfully or in error
	*
	* @param Data - Output parameters for the EOS_Auth_Logout Function
	*/
	static void EOS_CALL LogoutCompleteCallbackFn(const EOS_Auth_LogoutCallbackInfo* Data);

	/**
	* callback that is fired when the connect login operation completes, either successfully or in error
	*
	* @param Data - Output parameters for the EOS_Auth_Logout Function
	*/
	static void EOS_CALL ConnectLoginCompleteCb(const EOS_Connect_LoginCallbackInfo* Data);

	/**
	 * callback that is fired when the creation of a new connect user is complete
	 */
	static void EOS_CALL ConnectCreateUserCompleteCb(const EOS_Connect_CreateUserCallbackInfo* Data);

	/**
	 * callback that is fired when there is limited time left for the connect user auth token
	 * it is expected that the game will attempt to call EOS_Connect_Login again within the remaining time
	 * otherwise future calls to services will fail
	 */
	static void EOS_CALL ConnectAuthExpirationCb(const EOS_Connect_AuthExpirationCallbackInfo* Data);

	/**
	 * callback that is fired when auth login status has changed
	 */
	static void EOS_CALL LoginStatusChangedCb(const EOS_Auth_LoginStatusChangedCallbackInfo* Data);

	/**
	 * callback that is fired when an attempted login to connect auth is completed
	 * can end in two success state 
	 * - a user was found and auth token generated
	 * - a user was not found in which we will automatically create a user account in this sample
	 * -- typically the user will be prompted if they have other external credentials they'd like to login with
	 * -- this will allow the game to make a linkage between two different external accounts to one account here
	 */
	static void OnConnectLoginComplete(EOS_EResult Result, EOS_EpicAccountId UserId, EOS_ProductUserId LocalUserId);

	/**
	* Utility for printing auth token info
	*/
	static void PrintAuthToken(EOS_Auth_Token* InAuthToken);

	/**
	* Utility for printing Connect auth info
	*/
	static void PrintEOSAuthUsers();

	/**
	 * Send MFA Code user entered so login can continue
	 *
	 * @param MFAStr - String representing the MFA Code for login
	 */
	void SendMFACode(std::wstring MFAStr);

	/** Prints Auth Token Info for a valid auth token */
	void PrintAuthTokenInfo();

	/** Handle for Auth interface */
	EOS_HAuth AuthHandle;

	/** Handle for Connect interface */
	EOS_HConnect ConnectHandle;

	/** Notification ID used for Auth login status changed notification */
	EOS_NotificationId LoginStatusChangedId = EOS_INVALID_NOTIFICATIONID;

	/** Notification ID used for Connect Auth expiration notification */
	EOS_NotificationId ConnectAuthExpirationId = EOS_INVALID_NOTIFICATIONID;

	/** EOS Continuance Token which is used to link an external account and continue external auth login at a later time */
	EOS_ContinuanceToken PendingContinuanceToken = NULL;

	/** Login mode used for current EOS_Auth_Login */
	ELoginMode CurrentLoginMode;
};