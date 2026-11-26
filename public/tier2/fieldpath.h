#ifndef TIER2_FIELDPATH_H
#define TIER2_FIELDPATH_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CFieldPath
{
public:
	enum
	{
		MAX_PATH_DEPTH = 10
	};

	int Count() const { return m_nCount; }
	const int16 *Base() const { return m_bReadOnly ? m_pPath : m_Path; }

public:
	union
	{
		int16 *m_pPath;
		int16 m_Path[12];
	};

	int16 m_nCount;
	bool m_bReadOnly;
};

//-----------------------------------------------------------------------------
// The Huffman tree that encodes field path operations. Only the node interface is declared
// here; the encoder's members are not.
//-----------------------------------------------------------------------------
class CFieldPathHuffmanEncoder
{
public:
	// A node of the tree: InternalNode and LeafNode derive from it. On Windows the game has no
	// run-time type information for these classes.
	abstract_class INode
	{
	public:
		virtual ~INode() {}
		virtual bool IsLeafNode() const = 0;
	};
};

#endif // TIER2_FIELDPATH_H
