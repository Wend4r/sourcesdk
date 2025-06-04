#ifndef PROTOBUFBINDING_H
#define PROTOBUFBINDING_H

#ifdef _WIN32
#pragma once
#endif

#include <tier0/utlstring.h>
#include <tier1/bitbuf.h>
#include <inetchannel.h>
#include "color.h"

class CNetMessage;

abstract_class IProtobufBinding
{
public:
	virtual const char *GetName() const = 0;
	virtual int GetSize() const = 0;

	virtual const char *ToString( CNetMessage *pData, CUtlString &sResult ) const = 0;

	virtual const char *GetGroup() const = 0;
	virtual Color GetGroupColor() const = 0;
	virtual NetChannelBufType_t GetBufType() const = 0;

	virtual CNetMessage *AllocateMessage() const = 0;

	virtual bool OkToRedispatch() const = 0;
	virtual bool IsParent() const = 0;
};

#endif /* PROTOBUFBINDING_H */