//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph definition: the compiled graph resource shared by
//          every instance of the graph
//
//===========================================================================//

#ifndef ANIMGRAPH_DEFINITION_H
#define ANIMGRAPH_DEFINITION_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlhashtable.h"
#include "resourcesystem/resourcehandle.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"

#include "schemacompiler/schemaon.h"

class CNmGraphVariationUserData;

schema class CNmGraphDefinition
{
public:
	struct ReferencedGraphSlot_t
	{
		int16 m_nNodeIdx;
		int16 m_dataSlotIdx;
	};

	struct ExternalGraphSlot_t
	{
		int16 m_nNodeIdx;
		CGlobalSymbol m_slotID;
	};

	struct ExternalPoseSlot_t
	{
		int16 m_nNodeIdx;
		CGlobalSymbol m_slotID;
	};

	int GetNumControlParameters() const { return m_controlParameterIDs.Count(); }

	// Control parameters are the first nodes of the graph, so the position of an ID in the list
	// is the index of its node. Returns -1 for an ID that is not a control parameter.
	int16 GetControlParameterIndex( const CGlobalSymbol &parameterID ) const
	{
		for ( int i = 0; i < m_controlParameterIDs.Count(); i++ )
		{
			if ( m_controlParameterIDs[ i ] == parameterID )
			{
				return static_cast< int16 >( i );
			}
		}

		return -1;
	}

	CGlobalSymbol m_variationID;
	CStrongHandle< InfoForResourceTypeCNmSkeleton > m_skeleton;
	CUtlVector< CStrongHandle< InfoForResourceTypeCNmSkeleton > > m_supportedSecondarySkeletons;
	CNmGraphVariationUserData *m_pUserData;
	// Nodes that stay initialized for the whole life of an instance
	CUtlVector< int16 > m_persistentNodeIndices;
	int16 m_nRootNodeIdx;
	CUtlVector< CGlobalSymbol > m_controlParameterIDs;
	// Virtual parameters are computed by the graph and cannot be set
	CUtlVector< CGlobalSymbol > m_virtualParameterIDs;
	CUtlVector< int16 > m_virtualParameterNodeIndices;
	CUtlVector< ReferencedGraphSlot_t > m_referencedGraphSlots;
	CUtlVector< ExternalGraphSlot_t > m_externalGraphSlots;
	CUtlVector< ExternalPoseSlot_t > m_externalPoseSlots;

	// The members below are not serialized, the loader fills them in

	// Maps control parameter IDs to their position and virtual parameter IDs to m_virtualParameterNodeIndices
	CUtlHashtable< CGlobalSymbol, int16 > m_parameterLookupMap;
	CUtlVector< CNmGraphNode::CDefinition * > m_nodeDefinitions;
	// Holds the node definitions; only the allocation is used, the count stays zero
	CUtlVector< uint8 > m_nodeDefinitionMemory;
	// Offset of every node in the memory block of an instance
	CUtlVector< uint32 > m_instanceNodeStartOffsets;
	uint32 m_instanceRequiredMemory;
	uint32 m_instanceRequiredAlignment;

	CUtlVector< CUtlString > m_nodePaths;
	// Strong handles in the game
	CUtlVector< ResourceHandle_t > m_resources;
	// Every resource of this definition and of the graphs it references, by resource ID
	CUtlHashtable< ResourceId_t, ResourceHandle_t > m_resourceLUT;
	// Warnings about referenced resources that failed to load
	CUtlVector< CUtlString > m_resourceLoadWarnings;
};

#include "schemacompiler/schemaoff.h"

#endif // ANIMGRAPH_DEFINITION_H
