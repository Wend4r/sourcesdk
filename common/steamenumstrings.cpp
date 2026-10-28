//=============================================================================//
//
// Purpose: Text descriptions of Steam result enums
//
//=============================================================================//

#include "steamenumstrings.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EResult, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EResultToString( EResult eResult )
{
	switch ( eResult )
	{
	case k_EResultNone:										return "No result.";
	case k_EResultOK:										return "Success.";
	case k_EResultFail:										return "Generic failure.";
	case k_EResultNoConnection:								return "Your Steam client doesn't have a connection to the back-end.";
	case k_EResultInvalidPassword:							return "Password/ticket is invalid.";
	case k_EResultLoggedInElsewhere:						return "The user is logged in elsewhere.";
	case k_EResultInvalidProtocolVer:						return "Protocol version is incorrect.";
	case k_EResultInvalidParam:								return "A parameter is incorrect.";
	case k_EResultFileNotFound:								return "File was not found.";
	case k_EResultBusy:										return "Called method is busy - action not taken.";
	case k_EResultInvalidState:								return "Called object was in an invalid state.";
	case k_EResultInvalidName:								return "The name was invalid.";
	case k_EResultInvalidEmail:								return "The email was invalid.";
	case k_EResultDuplicateName:							return "The name is not unique.";
	case k_EResultAccessDenied:								return "Access is denied.";
	case k_EResultTimeout:									return "Operation timed out.";
	case k_EResultBanned:									return "The user is VAC2 banned.";
	case k_EResultAccountNotFound:							return "Account not found.";
	case k_EResultInvalidSteamID:							return "The Steam ID was invalid.";
	case k_EResultServiceUnavailable:						return "The requested service is currently unavailable.";
	case k_EResultNotLoggedOn:								return "The user is not logged on.";
	case k_EResultPending:									return "Request is pending, it may be in process or waiting on third party.";
	case k_EResultEncryptionFailure:						return "Encryption or Decryption failed.";
	case k_EResultInsufficientPrivilege:					return "Insufficient privilege.";
	case k_EResultLimitExceeded:							return "Too much of a good thing.";
	case k_EResultRevoked:									return "Access has been revoked (used for revoked guest passes.)";
	case k_EResultExpired:									return "License/Guest pass the user is trying to access is expired.";
	case k_EResultAlreadyRedeemed:							return "Guest pass has already been redeemed by account, cannot be used again.";
	case k_EResultDuplicateRequest:							return "The request is a duplicate and the action has already occurred in the past, ignored this time.";
	case k_EResultAlreadyOwned:								return "All the games in this guest pass redemption request are already owned by the user.";
	case k_EResultIPNotFound:								return "IP address not found.";
	case k_EResultPersistFailed:							return "Failed to write change to the data store.";
	case k_EResultLockingFailed:							return "Failed to acquire access lock for this operation.";
	case k_EResultLogonSessionReplaced:						return "The logon session has been replaced.";
	case k_EResultConnectFailed:							return "Failed to connect.";
	case k_EResultHandshakeFailed:							return "The authentication handshake has failed.";
	case k_EResultIOFailure:								return "There has been a generic IO failure.";
	case k_EResultRemoteDisconnect:							return "The remote server has disconnected.";
	case k_EResultShoppingCartNotFound:						return "Failed to find the shopping cart requested.";
	case k_EResultBlocked:									return "A user blocked the action.";
	case k_EResultIgnored:									return "The target is ignoring sender.";
	case k_EResultNoMatch:									return "Nothing matching the request found.";
	case k_EResultAccountDisabled:							return "The account is disabled.";
	case k_EResultServiceReadOnly:							return "This service is not accepting content changes right now.";
	case k_EResultAccountNotFeatured:						return "Account doesn't have value, so this feature isn't available.";
	case k_EResultAdministratorOK:							return "Allowed to take this action, but only because requester is admin.";
	case k_EResultContentVersion:							return "A Version mismatch in content transmitted within the Steam protocol.";
	case k_EResultTryAnotherCM:								return "The current CM can't service the user making a request, user should try another.";
	case k_EResultPasswordRequiredToKickSession:			return "You are already logged in elsewhere, this cached credential login has failed.";
	case k_EResultAlreadyLoggedInElsewhere:					return "The user is logged in elsewhere.";
	case k_EResultSuspended:								return "Long running operation has suspended/paused. (eg. content download.)";
	case k_EResultCancelled:								return "Operation has been canceled, typically by user. (eg. a content download.)";
	case k_EResultDataCorruption:							return "Operation canceled because data is ill formed or unrecoverable.";
	case k_EResultDiskFull:									return "Operation canceled - not enough disk space.";
	case k_EResultRemoteCallFailed:							return "The remote or IPC call has failed.";
	case k_EResultPasswordUnset:							return "Password could not be verified as it's unset server side.";
	case k_EResultExternalAccountUnlinked:					return "External account (PSN, Facebook...) is not linked to a Steam account.";
	case k_EResultPSNTicketInvalid:							return "PSN ticket was invalid.";
	case k_EResultExternalAccountAlreadyLinked:				return "External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first.";
	case k_EResultRemoteFileConflict:						return "The sync cannot resume due to a conflict between the local and remote files.";
	case k_EResultIllegalPassword:							return "The requested new password is not allowed.";
	case k_EResultSameAsPreviousValue:						return "New value is the same as the old one. This is used for secret question and answer.";
	case k_EResultAccountLogonDenied:						return "Account login denied due to 2nd factor authentication failure.";
	case k_EResultCannotUseOldPassword:						return "The requested new password is not legal.";
	case k_EResultInvalidLoginAuthCode:						return "Account login denied due to auth code invalid.";
	case k_EResultAccountLogonDeniedNoMail:					return "Account login denied due to 2nd factor auth failure - and no mail has been sent.";
	case k_EResultHardwareNotCapableOfIPT:					return "The users hardware does not support Intel's Identity Protection Technology (IPT).";
	case k_EResultIPTInitError:								return "Intel's Identity Protection Technology (IPT) has failed to initialize.";
	case k_EResultParentalControlRestricted:				return "Operation failed due to parental control restrictions for current user.";
	case k_EResultFacebookQueryError:						return "Facebook query returned an error.";
	case k_EResultExpiredLoginAuthCode:						return "Account login denied due to an expired auth code.";
	case k_EResultIPLoginRestrictionFailed:					return "The login failed due to an IP restriction.";
	case k_EResultAccountLockedDown:						return "The current users account is currently locked for use. This is likely due to a hijacking and pending ownership verification.";
	case k_EResultAccountLogonDeniedVerifiedEmailRequired:	return "The logon failed because the accounts email is not verified.";
	case k_EResultNoMatchingURL:							return "There is no URL matching the provided values.";
	case k_EResultBadResponse:								return "Bad Response due to a Parse failure, missing field, etc.";
	case k_EResultRequirePasswordReEntry:					return "The user cannot complete the action until they re-enter their password.";
	case k_EResultValueOutOfRange:							return "The value entered is outside the acceptable range.";
	case k_EResultUnexpectedError:							return "Something happened that we didn't expect to ever happen.";
	case k_EResultDisabled:									return "The requested service has been configured to be unavailable.";
	case k_EResultInvalidCEGSubmission:						return "The files submitted to the CEG server are not valid.";
	case k_EResultRestrictedDevice:							return "The device being used is not allowed to perform this action.";
	case k_EResultRegionLocked:								return "The action could not be complete because it is region restricted.";
	case k_EResultRateLimitExceeded:						return "Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent.";
	case k_EResultAccountLoginDeniedNeedTwoFactor:			return "Need two-factor code to login.";
	case k_EResultItemDeleted:								return "The thing we're trying to access has been deleted.";
	case k_EResultAccountLoginDeniedThrottle:				return "Login attempt failed, try to throttle response to possible attacker.";
	case k_EResultTwoFactorCodeMismatch:					return "Two factor authentication (Steam Guard) code is incorrect.";
	case k_EResultTwoFactorActivationCodeMismatch:			return "The activation code for two-factor authentication (Steam Guard) didn't match.";
	case k_EResultAccountAssociatedToMultiplePartners:		return "The current account has been associated with multiple partners.";
	case k_EResultNotModified:								return "The data has not been modified.";
	case k_EResultNoMobileDevice:							return "The account does not have a mobile device associated with it.";
	case k_EResultTimeNotSynced:							return "The time presented is out of range or tolerance.";
	case k_EResultSmsCodeFailed:							return "SMS code failure - no match, none pending, etc.";
	case k_EResultAccountLimitExceeded:						return "Too many accounts access this resource.";
	case k_EResultAccountActivityLimitExceeded:				return "Too many changes to this account.";
	case k_EResultPhoneActivityLimitExceeded:				return "Too many changes to this phone.";
	case k_EResultRefundToWallet:							return "Cannot refund to payment method, must use wallet.";
	case k_EResultEmailSendFailure:							return "Cannot send an email.";
	case k_EResultNotSettled:								return "Can't perform operation until payment has settled.";
	case k_EResultNeedCaptcha:								return "The user needs to provide a valid captcha.";
	case k_EResultGSLTDenied:								return "A game server login token owned by this token's owner has been banned.";
	case k_EResultGSOwnerDenied:							return "Game server owner is denied for some other reason such as account locked, community ban, vac ban, missing phone, etc.";
	case k_EResultInvalidItemType:							return "The type of thing we were requested to act on is invalid.";
	case k_EResultIPBanned:									return "The IP address has been banned from taking this action.";
	case k_EResultGSLTExpired:								return "This Game Server Login Token (GSLT) has expired from disuse; it can be reset for use.";
	case k_EResultInsufficientFunds:						return "User doesn't have enough wallet funds to complete the action.";
	case k_EResultTooManyPending:							return "There are too many of this thing pending already.";
	case k_EResultNoSiteLicensesFound:						return "No site licenses found.";
	case k_EResultWGNetworkSendExceeded:					return "The WG couldn't send a response because we exceeded max network send size.";
	case k_EResultAccountNotFriends:						return "The user is not mutually friends.";
	case k_EResultLimitedUserAccount:						return "The user is limited.";
	case k_EResultCantRemoveItem:							return "Item can't be removed.";
	case k_EResultAccountDeleted:							return "Account has been deleted.";
	case k_EResultExistingUserCancelledLicense:				return "A license for this already exists, but cancelled.";
	case k_EResultCommunityCooldown:						return "Access is denied because of a community cooldown (probably from support profile data resets).";
	case k_EResultNoLauncherSpecified:						return "No launcher was specified, but a launcher was needed to choose correct realm for operation.";
	case k_EResultMustAgreeToSSA:							return "User must agree to china SSA or global SSA before login.";
	case k_EResultLauncherMigrated:							return "The specified launcher type is no longer supported; the user should be directed elsewhere.";
	case k_EResultSteamRealmMismatch:						return "The user's realm does not match the realm of the requested resource.";
	case k_EResultInvalidSignature:							return "Signature check did not match.";
	case k_EResultParseFailure:								return "Failed to parse input.";
	case k_EResultNoVerifiedPhone:							return "Account does not have a verified phone number.";
	case k_EResultInsufficientBattery:						return "User device doesn't have enough battery charge currently to complete the action.";
	case k_EResultChargerRequired:							return "The operation requires a charger to be plugged in, which wasn't present.";
	case k_EResultCachedCredentialInvalid:					return "Cached credential was invalid - user must reauthenticate.";
	case K_EResultPhoneNumberIsVOIP:						return "The phone number provided is a Voice Over IP number.";
	case k_EResultNotSupported:								return "The data being accessed is not supported by this API.";
	case k_EResultFamilySizeLimitExceeded:					return "Reached the maximum size of the family.";
	case k_EResultOfflineAppCacheInvalid:					return "The local data for the offline mode cache is insufficient to login.";
	case k_EResultTryLater:									return "Retry the operation later.";
	default:												return "Unknown error.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EVoiceResult, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EVoiceResultToString( EVoiceResult eVoiceResult )
{
	switch ( eVoiceResult )
	{
	case k_EVoiceResultOK:						return "The call has completed successfully.";
	case k_EVoiceResultNotInitialized:			return "The Steam Voice interface has not been initialized.";
	case k_EVoiceResultNotRecording:			return "Steam Voice is not currently recording.";
	case k_EVoiceResultNoData:					return "There is no voice data available.";
	case k_EVoiceResultBufferTooSmall:			return "The provided buffer is too small to receive the data.";
	case k_EVoiceResultDataCorrupted:			return "The voice data has been corrupted.";
	case k_EVoiceResultRestricted:				return "The user is chat restricted.";
	case k_EVoiceResultUnsupportedCodec:		return "Deprecated.";
	case k_EVoiceResultReceiverOutOfDate:		return "Deprecated.";
	case k_EVoiceResultReceiverDidNotAnswer:	return "Deprecated.";
	default:									return "Unknown voice result.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EDenyReason, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EDenyReasonToString( EDenyReason eDenyReason )
{
	switch ( eDenyReason )
	{
	case k_EDenyInvalid:					return "Unknown.";
	case k_EDenyInvalidVersion:				return "The client and server are not the same version.";
	case k_EDenyGeneric:					return "Generic.";
	case k_EDenyNotLoggedOn:				return "The client is not logged on.";
	case k_EDenyNoLicense:					return "The client does not have a license to play this game.";
	case k_EDenyCheater:					return "The client is VAC banned.";
	case k_EDenyLoggedInElseWhere:			return "The client is logged in elsewhere.";
	case k_EDenyUnknownText:				return "Unknown text.";
	case k_EDenyIncompatibleAnticheat:		return "Incompatible anti-cheat.";
	case k_EDenyMemoryCorruption:			return "Memory corruption.";
	case k_EDenyIncompatibleSoftware:		return "Incompatible software.";
	case k_EDenySteamConnectionLost:		return "The server lost connection to steam.";
	case k_EDenySteamConnectionError:		return "The server had a general error connecting to Steam.";
	case k_EDenySteamResponseTimedOut:		return "The server timed out connecting to Steam.";
	case k_EDenySteamValidationStalled:		return "The client has not authed with Steam yet.";
	case k_EDenySteamOwnerLeftGuestUser:	return "The owner of the shared game has left, called for each guest of the owner.";
	default:								return "Unknown deny reason.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EBeginAuthSessionResult, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EBeginAuthSessionResultToString( EBeginAuthSessionResult eResult )
{
	switch ( eResult )
	{
	case k_EBeginAuthSessionResultOK:				return "Ticket is valid for this game and this Steam ID.";
	case k_EBeginAuthSessionResultInvalidTicket:	return "The ticket is invalid.";
	case k_EBeginAuthSessionResultDuplicateRequest:	return "A ticket has already been submitted for this Steam ID.";
	case k_EBeginAuthSessionResultInvalidVersion:	return "Ticket is from an incompatible interface version.";
	case k_EBeginAuthSessionResultGameMismatch:		return "Ticket is not for this game.";
	case k_EBeginAuthSessionResultExpiredTicket:	return "Ticket has expired.";
	default:										return "Unknown begin auth session result.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EAuthSessionResponse, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EAuthSessionResponseToString( EAuthSessionResponse eResponse )
{
	switch ( eResponse )
	{
	case k_EAuthSessionResponseOK:									return "Steam has verified the user is online, the ticket is valid and ticket has not been reused.";
	case k_EAuthSessionResponseUserNotConnectedToSteam:				return "The user in question is not connected to steam.";
	case k_EAuthSessionResponseNoLicenseOrExpired:					return "The user doesn't have a license for this App ID or the ticket has expired.";
	case k_EAuthSessionResponseVACBanned:							return "The user is VAC banned for this game.";
	case k_EAuthSessionResponseLoggedInElseWhere:					return "The user account has logged in elsewhere and the session containing the game instance has been disconnected.";
	case k_EAuthSessionResponseVACCheckTimedOut:					return "VAC has been unable to perform anti-cheat checks on this user.";
	case k_EAuthSessionResponseAuthTicketCanceled:					return "The ticket has been canceled by the issuer.";
	case k_EAuthSessionResponseAuthTicketInvalidAlreadyUsed:		return "This ticket has already been used, it is not valid.";
	case k_EAuthSessionResponseAuthTicketInvalid:					return "This ticket is not from a user instance currently connected to steam.";
	case k_EAuthSessionResponsePublisherIssuedBan:					return "The user is banned for this game. The ban came via the web api and not VAC.";
	case k_EAuthSessionResponseAuthTicketNetworkIdentityFailure:	return "The network identity in the ticket does not match the server authenticating the ticket.";
	default:														return "Unknown auth session response.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EUserHasLicenseForAppResult, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EUserHasLicenseForAppResultToString( EUserHasLicenseForAppResult eResult )
{
	switch ( eResult )
	{
	case k_EUserHasLicenseResultHasLicense:			return "The user has a license for specified app.";
	case k_EUserHasLicenseResultDoesNotHaveLicense:	return "The user does not have a license for the specified app.";
	case k_EUserHasLicenseResultNoAuth:				return "The user has not been authenticated.";
	default:										return "Unknown license result.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EBroadcastUploadResult, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EBroadcastUploadResultToString( EBroadcastUploadResult eResult )
{
	switch ( eResult )
	{
	case k_EBroadcastUploadResultNone:				return "Broadcast state unknown.";
	case k_EBroadcastUploadResultOK:				return "Broadcast was good, no problems.";
	case k_EBroadcastUploadResultInitFailed:		return "Broadcast init failed.";
	case k_EBroadcastUploadResultFrameFailed:		return "Broadcast frame upload failed.";
	case k_EBroadcastUploadResultTimeout:			return "Broadcast upload timed out.";
	case k_EBroadcastUploadResultBandwidthExceeded:	return "Broadcast send too much data.";
	case k_EBroadcastUploadResultLowFPS:			return "Broadcast FPS is too low.";
	case k_EBroadcastUploadResultMissingKeyFrames:	return "Broadcast sending not enough key frames.";
	case k_EBroadcastUploadResultNoConnection:		return "Broadcast client failed to connect to relay.";
	case k_EBroadcastUploadResultRelayFailed:		return "Relay dropped the upload.";
	case k_EBroadcastUploadResultSettingsChanged:	return "The client changed broadcast settings.";
	case k_EBroadcastUploadResultMissingAudio:		return "Client failed to send audio data.";
	case k_EBroadcastUploadResultTooFarBehind:		return "Clients was too slow uploading.";
	case k_EBroadcastUploadResultTranscodeBehind:	return "Server failed to keep up with transcode.";
	case k_EBroadcastUploadResultNotAllowedToPlay:	return "Broadcast does not have permissions to play game.";
	case k_EBroadcastUploadResultBusy:				return "RTMP host to busy to take new broadcast stream, choose another.";
	case k_EBroadcastUploadResultBanned:			return "Account banned from community broadcast.";
	case k_EBroadcastUploadResultAlreadyActive:		return "We already already have an stream running.";
	case k_EBroadcastUploadResultForcedOff:			return "We explicitly shutting down a broadcast.";
	case k_EBroadcastUploadResultAudioBehind:		return "Audio stream was too far behind video.";
	case k_EBroadcastUploadResultShutdown:			return "Broadcast Server was shut down.";
	case k_EBroadcastUploadResultDisconnect:		return "Broadcast uploader TCP disconnected.";
	case k_EBroadcastUploadResultVideoInitFailed:	return "Invalid video settings.";
	case k_EBroadcastUploadResultAudioInitFailed:	return "Invalid audio settings.";
	default:										return "Unknown broadcast upload result.";
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the description of an EChatRoomEnterResponse, as documented in the
//          Steamworks API reference
//-----------------------------------------------------------------------------
const char *EChatRoomEnterResponseToString( EChatRoomEnterResponse eResponse )
{
	switch ( eResponse )
	{
	case k_EChatRoomEnterResponseSuccess:			return "Success.";
	case k_EChatRoomEnterResponseDoesntExist:		return "Chat doesn't exist (probably closed).";
	case k_EChatRoomEnterResponseNotAllowed:		return "General Denied - You don't have the permissions needed to join the chat.";
	case k_EChatRoomEnterResponseFull:				return "Chat room has reached its maximum size.";
	case k_EChatRoomEnterResponseError:				return "Unexpected Error.";
	case k_EChatRoomEnterResponseBanned:			return "You are banned from this chat room and may not join.";
	case k_EChatRoomEnterResponseLimited:			return "Joining this chat is not allowed because you are a limited user (no value on account).";
	case k_EChatRoomEnterResponseClanDisabled:		return "Attempt to join a clan chat when the clan is locked or disabled.";
	case k_EChatRoomEnterResponseCommunityBan:		return "Attempt to join a chat when the user has a community lock on their account.";
	case k_EChatRoomEnterResponseMemberBlockedYou:	return "Join failed - a user that is in the chat has blocked you from joining.";
	case k_EChatRoomEnterResponseYouBlockedMember:	return "Join failed - you have blocked a user that is already in the chat.";
	case k_EChatRoomEnterResponseRatelimitExceeded:	return "Join failed - too many join attempts in a very short period of time.";
	default:										return "Unknown chat room enter response.";
	}
}
