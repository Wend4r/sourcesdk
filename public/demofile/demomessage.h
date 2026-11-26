//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A message of a demo file.
//
//=============================================================================//

#ifndef DEMOMESSAGE_H
#define DEMOMESSAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include <string>

enum EDemoCommands : int;

//-----------------------------------------------------------------------------
// A demo file message; CDemoMessagePB< command, protobuf message > derives from it and from its message. The
// server library of Windows has no implementation.
//-----------------------------------------------------------------------------
abstract_class IDemoMessage
{
public:
	virtual ~IDemoMessage() {}
	virtual EDemoCommands GetType() const = 0;
	virtual const char *GetName() const = 0;
	virtual std::string ToString() const = 0;
	virtual int GetSize() const = 0;
	virtual void *GetProtoMsg() = 0;
};

#endif // DEMOMESSAGE_H
