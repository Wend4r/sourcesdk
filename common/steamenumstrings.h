//=============================================================================//
//
// Purpose: Text descriptions of Steam result enums
//
//=============================================================================//

#ifndef STEAMENUMSTRINGS_H
#define STEAMENUMSTRINGS_H
#ifdef _WIN32
#pragma once
#endif

#include "steam/steamclientpublic.h"

// Descriptions of the values, as documented in the Steamworks API reference
const char *EResultToString( EResult eResult );
const char *EVoiceResultToString( EVoiceResult eVoiceResult );
const char *EDenyReasonToString( EDenyReason eDenyReason );
const char *EBeginAuthSessionResultToString( EBeginAuthSessionResult eResult );
const char *EAuthSessionResponseToString( EAuthSessionResponse eResponse );
const char *EUserHasLicenseForAppResultToString( EUserHasLicenseForAppResult eResult );
const char *EBroadcastUploadResultToString( EBroadcastUploadResult eResult );
const char *EChatRoomEnterResponseToString( EChatRoomEnterResponse eResponse );

#endif // STEAMENUMSTRINGS_H
