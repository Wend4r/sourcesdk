//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Smart prop documents ( procedural prop placement ) and their evaluation
//
//===========================================================================//

#ifndef ISMARTPROPSYSTEM_H
#define ISMARTPROPSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "appframework/iappsystem.h"

class ISmartProp;
class KeyValues3;

abstract_class ISmartPropSystem : public IAppSystem
{
public:
	// pContext goes to the CSmartProp constructor, which does not use it
	virtual ISmartProp *CreateSmartProp( void *pContext ) = 0;
	virtual void DestroySmartProp( ISmartProp *pSmartProp ) = 0;
	// Named by what it does: CreateSmartProp, then loads the document from pData
	virtual ISmartProp *CreateSmartPropFromKV3( void *pContext, const KeyValues3 *pData ) = 0;

	// Writes the document ( generic_data_type "CSmartPropRoot", _editor.next_element_id ) into pData
	virtual void SaveSmartPropToKV3( ISmartProp *pSmartProp, KeyValues3 *pData ) = 0;
	virtual void SaveSmartPropToFile( ISmartProp *pSmartProp, const char *pszFilename ) = 0;

	// Named by what it does: loads a temporary smart prop from pData and calls callback with it
	virtual void VisitSmartPropFromKV3( void *pContext, const KeyValues3 *pData, const std::function< void( ISmartProp * ) > &callback ) = 0;

	// Named by what it does: evaluates the smart prop at the transform ( two 16-byte vectors ) through a
	// CSmartPropEvaluationContext and copies the results out; pQuery is called by the evaluation to probe the
	// placement. The remaining arguments are the optional inputs and outputs of the evaluation context; it
	// prints "Evaluated %s in %.2fms" when timing is enabled
	virtual void EvaluateSmartProp( void *pQuery, ISmartProp *pSmartProp, const void *pTransform, const int *pnIndex, void *pElementInput,
		void *pValueTableInput, void *pStringListInput, void *pItemListInput, void *pResourceContext, void *pOutItems, void *pOutValues, void *pOutStrings, void *pUserContext ) = 0;
};

#endif // ISMARTPROPSYSTEM_H
