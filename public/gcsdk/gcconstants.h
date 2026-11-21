//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: declares a variety of constants
//
// $NoKeywords: $
//=============================================================================

#ifndef GCCONSTANTS_H
#define GCCONSTANTS_H
#ifdef _WIN32
#pragma once
#endif

#include "steam/steamtypes.h"
#include "tier0/t0constants.h"

namespace GCSDK
{

//-----------------------------------------------------------------------------
// Timing constants
//-----------------------------------------------------------------------------

// How long each frame should last
const int k_cMicroSecPerShellFrame = 50 * k_nThousand;

// How many frames per second should we have
const uint32 k_cFramesPerSec = k_nMillion / k_cMicroSecPerShellFrame;

// Task granularity -- the longest time in microseconds you should spend without yielding
const int k_cMicroSecTaskGranularity = 5 * k_nThousand;

// If a frame runs longer than it's supposed to (which they always will by at least a little),
// we subtract the overage from the next frame.  This is the maximum amount to correct each frame.
const int k_cMicroSecMaxFrameCorrection = 25 * k_nThousand;

// if server time is too far behind, we will start skipping frames to re-synchronize it with real time
const int k_cMicroSecBehindToStartFrameSkipping = (k_nMillion * 2);

// Default Max time to allow a job to be blocked on I/O
const int k_cMicroSecJobPausedTimeout = 20 * k_nMillion;

// How much time a job should run before heartbeating
const int k_cMicroSecJobHeartbeat = k_cMicroSecJobPausedTimeout / 4;

// Default Max number of job heartbeat intervals to allow a job to be blocked on I/O
const int k_cJobHeartbeatsBeforeTimeoutDefault = k_cMicroSecJobPausedTimeout / k_cMicroSecJobHeartbeat;

//-----------------------------------------------------------------------------
// Server types
//-----------------------------------------------------------------------------
// EServerType
// Specifies the type of a specific server
// MUST BE KEPT IN SYNC with g_rgchServerTypeName !!!
    enum EServerType
    {
        k_EServerTypeInvalid = -1,

        k_EServerTypeShell = 0,
        k_EServerTypeGC = 1,
        k_EServerTypeGCClient = 2,

        // Must be last!!!
        k_EServerTypeMax = 2,
    };
    const EServerType k_EServerTypeFirst = k_EServerTypeShell;


//-----------------------------------------------------------------------------
// Spew / EmitEvent constants
//-----------------------------------------------------------------------------

#define SPEW_ALWAYS 1
#define SPEW_NEVER 5

#define LOG_ALWAYS 1
#define LOG_NEVER 5


//-----------------------------------------------------------------------------
// string constants
//-----------------------------------------------------------------------------

const char k_rgchUnknown[] = "Unknown";


//-----------------------------------------------------------------------------
// Game Server constants
//-----------------------------------------------------------------------------

const uint32 k_cubGameServerToken = 16;									// size of the game server identity token key
const uint32 k_cchGameServerTokenStringMax = (k_cubGameServerToken * 2) + 1;	// size of string representing game server identity token (hex encoded so twice the size + NUL)
typedef	unsigned char GameServerIdentityToken_t[ k_cubGameServerToken ];


//-----------------------------------------------------------------------------
// Other constants
//-----------------------------------------------------------------------------

const int k_cSmallBuff = 255;					// smallish buffer 
const int k_cMedBuff = 1024;					// medium buffer 

const int k_cubMsgSizeSmall = 512;				// small sized packet

const int k_cInitialNetworkBuffers = 10;		// # of network buffers to see the system with

const int k_cubMaxExpectedMsgDataSize = 5 * k_nMegabyte;// the maximum application data that we EXPECT to be sent in a single message

#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif // CS:GO already defines STRINGIFY appropriately, no reason to redefine here
#define TOSTRING(x) STRINGIFY(x)
#define FILE_AND_LINE __FILE__ ":" TOSTRING(__LINE__)	

const char *PchNameFromEUniverse( EUniverse eUniverse );
EUniverse EUniverseFromName( const char *pchName );

} // namespace GCSDK

#endif // GCCONSTANTS_H
