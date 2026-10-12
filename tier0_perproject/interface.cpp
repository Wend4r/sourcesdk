//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Exposes the interfaces of a module through CreateInterface.
//
//=============================================================================//

#include <string.h>
#include "interfaces/interfaces.h"

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *s_pInterfaceRegs = NULL;

InterfaceReg::InterfaceReg(InstantiateInterfaceFn fn, const char *pName) :
	m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

#ifndef CREATE_INTEFACE_OVERRIDE

// ------------------------------------------------------------------------------------ //
// CreateInterface.
// This is the primary exported function by a dll, referenced by name via dynamic binding
// that exposes an opqaue function pointer to the interface.
// ------------------------------------------------------------------------------------ //
void* CreateInterface(const char *pName, int *pReturnCode)
{
	InterfaceReg *pCur;

	for (pCur = s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
	{
		if (strcmp(pCur->m_pName, pName) == 0)
		{
			if (pReturnCode)
			{
				*pReturnCode = IFACE_OK;
			}
			return pCur->m_CreateFn();
		}
	}

	if (pReturnCode)
	{
		*pReturnCode = IFACE_FAILED;
	}
	return NULL;
}

#endif // CREATE_INTEFACE_OVERRIDE
