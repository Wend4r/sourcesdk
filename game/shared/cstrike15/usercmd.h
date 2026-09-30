#ifndef CSTRIKE15_USERCMD_H
#define CSTRIKE15_USERCMD_H

#pragma once

#include "basetypes.h"
#include "schemasystem/schematypes.h"
#include "inbuttonstate.h"

#include <cs_usercmd.pb.h>

#include <string>

class CUserCmdBase
{
public:
	virtual ~CUserCmdBase() = default;

	// The protobuf type name, cached per class.
	virtual const char *GetMessageTypeName() const = 0;

	// Clears the message, zeroes the command number and reloads the button states from it.
	virtual void Reset()
	{
		GetProtobufMessage()->Clear();
		m_cmdNum = 0;
		ButtonsFromMessage();
	}

	virtual google::protobuf::Message *GetProtobufMessage() = 0;
	virtual CBaseUserCmdPB *MutableBase() = 0;
	virtual const CBaseUserCmdPB &GetBase() const = 0;

	// Writes the button states into base.buttons_pb.
	virtual void ButtonsToMessage() = 0;

	// Loads the button states from base.buttons_pb.
	virtual void ButtonsFromMessage() = 0;

public:
	int m_cmdNum;
};

template < class T >
class CUserCmdBaseHost : public CUserCmdBase, public T
{
public:
	const char *GetMessageTypeName() const override
	{
		static const std::string s_sTypeName = T::default_instance().GetTypeName();
		return s_sTypeName.c_str();
	}

	google::protobuf::Message *GetProtobufMessage() override { return static_cast< T * >( this ); }
	CBaseUserCmdPB *MutableBase() override { return T::mutable_base(); }
	const CBaseUserCmdPB &GetBase() const override { return T::base(); }
};

#endif // CSTRIKE15_USERCMD_H
