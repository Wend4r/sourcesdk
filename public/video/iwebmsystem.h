//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Factory for WebM recording components (Windows only)
//
//===========================================================================//

#ifndef IWEBMSYSTEM_H
#define IWEBMSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class IAV1EncoderInstance;
class IVPXEncoderInstance;
class IVorbisEncoderInstance;
class IWebmMuxer;

abstract_class IWebmSystem : public IAppSystem
{
public:
	virtual IAV1EncoderInstance *CreateAV1Encoder() = 0;
	virtual IVPXEncoderInstance *CreateVPXEncoder() = 0;
	virtual IVorbisEncoderInstance *CreateVorbisEncoder() = 0;
	virtual IWebmMuxer *CreateWebmMuxer() = 0;
};

#endif // IWEBMSYSTEM_H
