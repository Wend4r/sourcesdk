//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Particle system manager interface
//
//===========================================================================//

#ifndef IPARTICLESYSTEMMGR_H
#define IPARTICLESYSTEMMGR_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcefile/resourcehandle.h>

class IParticleSystemDefinition;

class InfoForResourceTypeIParticleSystemDefinition
{
public:
	using RuntimeClass_t = IParticleSystemDefinition;
};

using HParticleSystemDefinition = CWeakHandle< InfoForResourceTypeIParticleSystemDefinition >;

abstract_class IParticleSystemMgr : public IAppSystem
{
public:
	virtual void unk011() = 0;
	virtual void unk012() = 0;
	virtual void unk013() = 0;
	virtual bool unk014() = 0;

	virtual HParticleSystemDefinition FindParticleSystem( const char *pszName, bool bWarnIfNotPrecached ) = 0;

	virtual void unk016() = 0;

	virtual void *CreateParticleCollection( const char *pszName, void *pUnk1, void *pUnk2, bool bUnk, int nUnk1, int nUnk2 ) = 0;

	virtual void *CreateParticleCollection( ResourceHandle_t hDefinition, void *pUnk1, void *pUnk2, bool bUnk, int nUnk1, int nUnk2 ) = 0;
	virtual void Unk_DestroyParticleCollection( void *pCollection ) = 0;
	virtual bool unk020() = 0;
	virtual bool unk021() = 0;
	virtual void unk022() = 0;

	// Display names such as Position, Life Duration or Radius.
	virtual const char *Unk_GetParticleAttributeName( int nAttribute ) = 0;

	virtual int Unk_GetOperatorDefinitionCount( int nFunctionType ) = 0;
	virtual void *Unk_GetOperatorDefinition( int nFunctionType, int nIndex ) = 0;
	virtual void unk026() = 0;
	virtual float unk027() = 0;

	virtual void Unk_DumpParticleSystems( void *p ) = 0;

	virtual void Unk_CreateParticleSceneObject( const char *pszName, void *pUnk1, void *pUnk2, uint32 nUnk, float flUnk ) = 0;

	virtual void Unk_CreateParticleSceneObject( void *pDefinition, void *pUnk1, void *pUnk2, uint32 nUnk ) = 0;
	virtual void Unk_CreateParticleSceneObject( void *pCollection, void *pUnk, bool bUnk ) = 0;
	virtual void unk032() = 0;

	// Draws from the random sequence of a particle collection.
	virtual int Unk_RandomInt( void *pCollection, int nMin, int nMax ) = 0;

	virtual float Unk_RandomFloat( void *pCollection, float flMin, float flMax ) = 0;
	virtual const char * unk035() = 0;
	virtual int unk036() = 0;
	virtual void unk037() = 0;
	virtual void unk038() = 0;
	virtual void unk039() = 0;
	virtual void unk040() = 0;

	virtual void Unk_CreateProceduralSnapshot( void *p ) = 0;

	virtual void unk042() = 0;
	virtual void unk043() = 0;
	virtual void unk044() = 0;
	virtual void unk045() = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual int Unk_GetParticleSystemVersion() = 0;
	virtual const char *Unk_GetParticleSystemVersionChangeDescription( int nVersion ) = 0;
	virtual void unk053() = 0;
	virtual bool unk054() = 0;
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void unk057() = 0;
	virtual void unk058() = 0;
	virtual void unk059() = 0;
	virtual void Unk_SimulateGPUParticles( void *p ) = 0;
};

#endif // IPARTICLESYSTEMMGR_H
