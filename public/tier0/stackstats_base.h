#ifndef TIER0_STACKSTATS_BASE_H
#define TIER0_STACKSTATS_BASE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/threadtools.h"

class PLATFORM_CLASS CallStackStatStructDescFuncs
{
public:
	//description file format
	//1 byte version per entry. Assuming that the field will get more descriptive over time, reserving this now.
	virtual size_t DescribeField( uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength ) = 0;

	enum MergeScript_Language
	{
		SSMSL_Squirrel, //Only support squirrel for now, theoretically expandable to any vscript supported language
	};

#if 0 //embedded script handling not ready yet
	//this is expected to write a piece of script code into the body of a function that will merge two values of this type
	//The function will have two parameters, "mergeTo", and "mergeFrom". Both are tables with your field defined by name
	//So, an example to merge an integer count defined as "foo" in the stack struct would look like this "mergeTo.foo += mergeFrom.foo\n"
	virtual size_t DescribeMergeOperation( MergeScript_Language scriptLanguage, uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength ) = 0;
#endif

	//reserve your description field versions here to avoid stomping others
	enum DescribeFieldVersions_t
	{
		DFV_BasicStatStructFieldTypes_t, //Format: 1 byte BasicStatStructFieldTypes_t type, 4 byte field offset, null terminated string field name
	};

	const char *m_szFieldName;
	size_t m_iFieldOffset;
	CallStackStatStructDescFuncs *m_pNext; //needed for how the description macros are laid out. Couldn't figure out a way to store the static struct instances as a static array
};

enum StatStructDescription_LumpID
{
	SSDLID_UNKNOWN,
	SSDLID_STATICINTERPRETER,
	SSDLID_FIELDDESC,
	SSDLID_EMBEDDEDSCRIPT,


	SSDLID_COUNT,
	SSDLID_FORCE_UINT32 = 0xFFFFFFFF,
};

enum BasicStatStructFieldTypes_t
{
	BSSFT_UNKNOWN,
	BSSFT_BOOL,
	BSSFT_INT8,
	BSSFT_UINT8,
	BSSFT_INT16,
	BSSFT_UINT16,
	BSSFT_INT32,
	BSSFT_UINT32,
	BSSFT_INT64,
	BSSFT_UINT64,
	BSSFT_FLOAT,
	BSSFT_DOUBLE,
	BSSFT_VECTOR2D,
	BSSFT_VECTOR3D,
	BSSFT_VECTOR4D,

	BSSFT_COUNT,
};

enum BasicStatStructFieldCombineMethods_t
{
	BSSFCM_UNKNOWN,
	BSSFCM_CUSTOM, //rely on some outside handler
	BSSFCM_ADD, //add the values
	//BSSFCM_SUBTRACT, //what would subtract even mean? which one from which?
	BSSFCM_MAX, //keep max value
	BSSFCM_MIN, //keep min value
	BSSFCM_AND, // &= , Non-integer behavior undefined
	BSSFCM_OR, // |= , Non-integer behavior undefined
	BSSFCM_XOR, // ^= , Non-integer behavior undefined
	/*BSSFCM_LIST, //keep a list of each value (probably complicated)*/

	BSSFCM_COUNT,
};

class PLATFORM_CLASS BasicStatStructFieldDesc : public CallStackStatStructDescFuncs
{
public:
	BasicStatStructFieldDesc( BasicStatStructFieldTypes_t type, BasicStatStructFieldCombineMethods_t combineMethod ) : m_Type(type), m_Combine(combineMethod) {};
	size_t DescribeField( uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength );
#if 0 //embedded script handling not ready yet
	size_t DescribeMergeOperation( MergeScript_Language scriptLanguage, uint8 *pDescribeWriteBuffer, size_t iDescribeMaxLength );
#endif

	BasicStatStructFieldTypes_t m_Type;
	BasicStatStructFieldCombineMethods_t m_Combine;	
};

#endif // TIER0_STACKSTATS_BASE_H
