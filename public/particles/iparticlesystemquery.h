//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The query interface the particle system calls back into the game with.
//
//=============================================================================//

#ifndef IPARTICLESYSTEMQUERY_H
#define IPARTICLESYSTEMQUERY_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/fltx4.h"

class Vector;
class Color;
class CTransform;
class CEntityHandle;
class CGameTrace;
class CUtlStringToken;
class IParticleCollection;
class ISceneWorld;
class CHitBoxSet;
class FourRays;
struct RayTracingResult;
struct ModelHitBoxInfo_t;
struct ParticleModelQueryInfo_t;
struct ParticleProjectedTextureInfo_t;
struct HProjectedTexture__;
struct HDeferredLight__;

//-----------------------------------------------------------------------------
// What the particle system asks the game: the game module hands one query object per world to the particle
// systems of that world (CParticleSystemQueryGameSystem). 101 methods; the server's implementation answers
// almost every query with a constant, the client's does the work. The particle library has a default
// implementation of its own that the operators compare against before calling. The Stub_ methods are named
// by their slot; what the server and the client do with each, and which operators call it, is noted.
//-----------------------------------------------------------------------------
abstract_class IParticleSystemQuery
{
public:
	// The client returns the transform of the entity behind a handle; the server an identity transform.
	virtual void Stub_0() = 0;

	// The client writes a pointer it finds through the entity of a scene object (null when there is none);
	// the server writes null to its pointer argument.
	virtual void Stub_1() = 0;

	// Named by what it does: the token of the query's world (the member at offset 8), in the server and in the
	// client.
	virtual uint32 GetWorldToken() const = 0;

	// The light colour at a point (the client samples the lighting of the query's world; white in the server
	// and in the editor worlds).
	virtual void GetLightingAtPoint( const Vector &vecOrigin, Color &tint ) = 0;

	// A line trace for the particle operators that collide with or snap to the world. nCollisionGroup is
	// what GetCollisionGroupFromName returned; nFilterFlags is a byte the trace filter keeps next to it.
	virtual void TraceLine( const Vector &vecAbsStart, const Vector &vecAbsEnd, CEntityHandle hIgnoreEntity, uint64 nInteractsWith,
		uint8 nCollisionGroup, uint8 nFilterFlags, CGameTrace *pTrace ) = 0;

	// Writes one bool per point: inside a hitbox of the model at a control point. pHitBoxes describes the
	// boxes (the model's own, or a list of 80-byte box entries); pPointQuery is { control point, number of
	// points, bounding box only }. The server returns false.
	virtual bool ArePointsInControllingObjectHitBoxes( IParticleCollection *pParticles, const void *pHitBoxes, const void *pPointQuery,
		const Vector *pPoints, bool *pResults ) = 0;

	// 0 for "PRECIPITATION", 1 for "PRECIPITATIONBLOCKER" (case-insensitive), 0 for anything else.
	virtual int GetRayTraceEnvironmentFromName( const char *pszRtEnvName ) = 0;

	// The client returns the bounds of one object of a ray trace environment (the precipitation entities, the
	// blockers, or the environments the client builds), drawing them when debugging; the server returns
	// empty bounds. Called by the ray trace environment cull initializer.
	virtual void Stub_7() = 0;

	// NONE, DEBRIS, INTERACTIVE, NPC, ACTOR and PASSABLE to their collision group; 4 for anything else.
	virtual int GetCollisionGroupFromName( const char *pszName ) = 0;

	// For each input point the closest hitbox of the model at a control point: its index and the point
	// relative to it. The server clears the outputs.
	virtual void GetClosestControllingObjectHitBox( IParticleCollection *pParticles, int nControlPointNumber, int nNumPtsIn, float flBBoxScale,
		Vector *pPntsIn, Vector *pHitBoxRelativeCoordOut, int *pHitBoxIndexOut, int nDesiredHitbox, const char *pszHitboxSetName ) = 0;

	// The transforms of the hitboxes of a hitbox set of the model at a control point; returns how many were
	// written. The server returns 0.
	virtual int GetControllingObjectHitBoxTransforms( IParticleCollection *pParticles, int nControlPointNumber, const CHitBoxSet *pHitBoxSet,
		int nBufSize, CTransform *pTransformsOut ) = 0;

	// The client does what GetControllingObjectHitBoxTransforms does for the entity behind a handle instead
	// of a control point; the server returns 0.
	virtual void Stub_11() = 0;

	// One box entry per bone of the model at a control point (a box of +-8 units at the bone); returns how
	// many were written. The server returns 0.
	virtual int GetControllingObjectBoneInfo( IParticleCollection *pParticles, int nControlPointNumber, int nBufSize,
		ModelHitBoxInfo_t *pHitBoxOutputBuffer ) = 0;

	// The client does what GetControllingObjectBoneInfo does for the entity behind a handle; the server
	// returns 0.
	virtual void Stub_13() = 0;

	// The client fills a box list (count at offset 28, 80-byte entries at offset 40) from the physics shapes
	// of the entity at a control point and returns the count; the server returns 0.
	virtual void Stub_14() = 0;

	// The oriented bounds of the entity at a control point, relative to the control point; zero in the server.
	virtual void GetControllingObjectOBBox( IParticleCollection *pParticles, int nControlPointNumber, Vector &vecMin, Vector &vecMax ) = 0;

	// Traces four rays against a ray trace environment of the client. Empty in the server.
	virtual void TraceAgainstRayTraceEnv( int nEnvNumber, const FourRays &rays, fltx4 TMin, fltx4 TMax, RayTracingResult *pResult,
		int nSkipId ) const = 0;

	// The position of a player (the entity behind the handle, else the local player): 0 its origin, 1 the
	// centre of its bounds, 2 its eyes. Also in the server.
	virtual Vector GetLocalPlayerPos( CEntityHandle hPlayer, int nPositionType ) = 0;

	// The client returns a direction of the player behind a handle (or of the local player), ( 0, 0, 1 )
	// without one; the server does the same with its players.
	virtual void Stub_18() = 0;

	// The client returns the eye transform of the player behind a handle (or of the local player), with or
	// without the eye angles; the server does the same with its players. Called by the operator that moves a
	// control point to the player.
	virtual void Stub_19() = 0;

	// The client returns the entity handle of the object at a control point (the world trace operators pass
	// it to TraceLine to ignore it); the server returns 0.
	virtual void Stub_20() = 0;

	// The client writes the transform of the entity behind a handle as a 3x4 matrix and returns true; the
	// server returns 0.
	virtual void Stub_21() = 0;

	// The client converts a point into the space of the entity behind a handle and returns true; the server
	// returns 0. Called by the world trace constraint.
	virtual void Stub_22() = 0;

	// The activity of a model by name; -1 in the server and in the client.
	virtual int GetActivityNumber( void *pModel, const char *pszActivityName ) = 0;

	// The sequence the model at a control point plays, and its rate in *pflRate when that is positive; -1
	// without a model.
	virtual int GetSequenceFromModel( IParticleCollection *pParticles, int nControlPointNumber, float *pflRate ) = 0;

	// The scale of the entity at a control point; 1 without one.
	virtual float GetScaleFromCPObject( IParticleCollection *pParticles, int nControlPointNumber ) = 0;

	// How much of a point the camera of the query's world sees.
	virtual float GetPointVisibilityPercentage( Vector &vecPos, float &flRadius ) = 0;

	// The client does what GetModelFromCPObject does for the entity behind a handle; the server writes null
	// and -1.
	virtual int Stub_27() = 0;

	// The model and the skeleton of the entity at a control point (a default model without one). The game
	// ignores nIgnored and bIgnored. The server writes null and -1.
	virtual void GetModelFromCPObject( IParticleCollection *pParticles, int nControlPointNumber, int nIgnored, bool bIgnored,
		ParticleModelQueryInfo_t *pInfo ) = 0;

	// The client returns an object from the entity at a control point (under the entity lock); the server
	// returns 0.
	virtual void Stub_29() = 0;

	// Creates or updates the projected texture of a projected texture renderer.
	virtual void UpdateProjectedTexture( const ParticleProjectedTextureInfo_t &info, HProjectedTexture__ *&hTexture ) = 0;

	// Releases a projected texture.
	virtual void DeleteProjectedTexture( HProjectedTexture__ *hTexture ) = 0;

	// Creates or updates the light of a deferred light renderer and returns its handle; 0 in the server. The
	// parameters (the light's position, colour, radius and falloff) are not reconstructed.
	virtual HDeferredLight__ *UpdateDeferredLight() = 0;

	// Releases a deferred light.
	virtual void DeleteDeferredLight( HDeferredLight__ *hLight ) = 0;

	// The client forwards to a client interface; the standard light renderer creates its lights with it. The
	// server returns 0.
	virtual void Stub_34() = 0;

	// The client forwards to a client interface; the standard light renderer releases its lights with it.
	virtual void Stub_35() = 0;

	// The client forwards to a client interface; the omni light and light beam renderers create their lights
	// with it. The server returns 0.
	virtual void Stub_36() = 0;

	// The client forwards to a client interface; the omni light and light beam renderers release their lights
	// with it.
	virtual void Stub_37() = 0;

	// The post processing renderer creates its post processing object with it; the server returns 0.
	virtual void Stub_38() = 0;

	// The post processing renderer releases that object with it. Empty in the server.
	virtual void Stub_39() = 0;

	// The post processing renderer updates that object with it. Empty in the server.
	virtual void Stub_40() = 0;

	// The volumetric emitter renderer updates its emitter with it. Empty in the client and the server.
	virtual void Stub_41() = 0;

	// The volumetric emitter renderer releases its emitter with it. Empty in the client and the server.
	virtual void Stub_42() = 0;

	// Empty in the client and the server; no caller in the particle library.
	virtual void Stub_43() = 0;

	// Starts or updates the sound of the sound renderer; the handle is written back.
	virtual void UpdateSoundPatch( const void *pSoundInfo, void *&pSound ) = 0;

	// Stops a sound of the sound renderer (the client flags it to stop).
	virtual void DeleteParticleSound( void *&pSound ) = 0;

	// A vector force (nForce) of the cloth renderer. Empty in the client and the server.
	virtual void SetClothForceVector( int nForce, Vector &vecForce, void *&pCloth ) = 0;

	// A scalar force (nForce) of the cloth renderer. Empty in the client and the server.
	virtual void SetClothForceFloat( int nForce, float flValue, void *&pCloth ) = 0;

	// A screen shake for the local player, its amplitude fading out with the distance from vecCenter to
	// flRadius. Empty in the server.
	virtual void ScreenShakeGlobal( const Vector &vecCenter, float flAmplitude, float flFrequency, float flDuration, float flRadius ) = 0;

	// A screen shake for the player at a control point when that is the local player. Empty in the server.
	virtual void ScreenShakeLocal( Vector vecCenter, float flAmplitude, float flFrequency, float flDuration, IParticleCollection *pParticles,
		int nControlPointNumber ) = 0;

	// The loadout slot of a slot name, for the model renderer; 0 in the client and the server.
	virtual int GetEconLoadoutSlotType( const char *pszSlotName ) = 0;

	// The wind at a point, for the external wind force.
	virtual Vector GetWindVelocityAtLocation( const Vector &vecPos ) = 0;

	// The client returns ( stack hash << 32 ) | the index of a global sound float, for the operator that
	// drives a control point from a global sound float; the server returns -1.
	virtual int Stub_52() = 0;

	// The client returns the value of a global sound float, for the same operator; the server returns zero.
	virtual void Stub_53() = 0;

	// The time the model renderer decodes animations at; 0 in the client and the server.
	virtual double GetAnimDecodeRealTime() const = 0;

	// A time of day value of the query's world; the client and the server return vecDefault.
	virtual Vector GetTimeOfDayValue( CUtlStringToken token, const Vector &vecDefault ) const = 0;

	// The render bounds of the entity at a control point, and its transform.
	virtual void GetControllingObjectRenderBounds( IParticleCollection *pParticles, int nControlPointNumber, Vector *pMins, Vector *pMaxs,
		CTransform *pTransform ) = 0;

	// Whether a point is under water, and the height of the water; false in the client and the server.
	virtual bool IsPointUnderWater( IParticleCollection *pParticles, Vector vecPos, float &flWaterHeight ) = 0;

	// Flattens the grass of a scene world around a point, for the flatten grass renderer. Empty in the client
	// and the server.
	virtual void AddGrassFlattenEvent( ISceneWorld *pWorld, const Vector &vecPos, float flRadius, const Vector &vecDirection, float flStrength ) = 0;

	// Shakes the trees around a point, for the tree shake renderer. Empty in the client and the server.
	virtual void AddShakeTreesInRadiusEvent( const Vector &vecCenter, const Vector &vecDirection, float flParam0, float flParam1, float flParam2,
		float flParam3, float flParam4, float flParam5, float flParam6 ) = 0;

	// The callback operator calls it with its particle collection; the client walks up to the root collection
	// and calls its callback. Empty in the server.
	virtual void Stub_60() = 0;

	// The game liquid spill operator calls it. Empty in the client and the server.
	virtual void Stub_61() = 0;

	// The client asks one of its systems for a resource by its argument, writes the resource handle and
	// returns true; the server returns 0. Called on the way of the skinned control point snapshot operators.
	virtual void Stub_62() = 0;

	// The server returns 0.
	virtual void Stub_63() = 0;

	// The external game impulse force calls it and tests the returned mask; the client and the server return 0.
	virtual void Stub_64() = 0;

	// The particle impulse initializer passes its impulses to it. Empty in the client and the server.
	virtual void Stub_65() = 0;

	// The offscreen decay operator calls it with its bounds and uses what it returns (the client body is large,
	// with a handle and the bounds); the server returns 0.
	virtual void Stub_66() = 0;

	// The light renderers pass whether they draw and use what it returns; the server returns its int
	// argument.
	virtual void Stub_67() = 0;

	// The material proxy renderer calls it. Empty in the client and the server.
	virtual void Stub_68() = 0;

	// The material proxy renderer calls it. Empty in the client and the server.
	virtual void Stub_69() = 0;

	// The material proxy renderer switches a state of the entity at a control point with it. Empty in the
	// client and the server.
	virtual void Stub_70() = 0;

	// The material proxy renderer passes two floats for the entity at a control point; the client hands them
	// to a component of that entity (under the entity lock). Empty in the server.
	virtual void Stub_71() = 0;

	// The material proxy renderer reads two floats of the entity at a control point from the same component;
	// ( 1, 1 ) without one, and in the server.
	virtual void Stub_72() = 0;

	// The server returns 0.
	virtual void Stub_73() = 0;

	// The vector field snapshot operator reads a resolution from it; the server returns 32.
	virtual void Stub_74() = 0;

	// The server returns zero (float or vector).
	virtual void Stub_75() = 0;

	// The external wind force and the operator that moves a control point to the water surface call it; the
	// server returns 0.
	virtual void Stub_76() = 0;

	// Many vector operators (set / init vector, lock to bone, wind and noise forces) call it; the client fills
	// its output array with one vector per entry when the box it gets is not empty and returns true, the
	// server returns 0.
	virtual void Stub_77() = 0;

	// Empty in the client and the server.
	virtual void Stub_78() = 0;

	// The external wind force calls it; the server returns 0.
	virtual void Stub_79() = 0;

	// The server returns -1.
	virtual int Stub_80() = 0;

	// The client physics operator goes on only when it returns true; the client and the server return 0.
	virtual void Stub_81() = 0;

	// The client physics operator creates its physics objects with it; the client and the server return 0.
	virtual void Stub_82() = 0;

	// The client physics operator calls it per object. Empty in the client and the server.
	virtual void Stub_83() = 0;

	// The client physics operator releases its objects with it. Empty in the client and the server.
	virtual void Stub_84() = 0;

	// The client physics operator reads two outputs per object with it. Empty in the client and the server.
	virtual void Stub_85() = 0;

	// The server returns -1.
	virtual int Stub_86() = 0;

	// The client physics operator calls it per object. Empty in the client and the server.
	virtual void Stub_87() = 0;

	// The client physics impulse renderer calls it. Empty in the client and the server.
	virtual void Stub_88() = 0;

	// The server returns 0.
	virtual void Stub_89() = 0;

	// The game decal renderer calls it with its decals (the client body is large). Empty in the server.
	virtual void Stub_90() = 0;

	// The particle system renderer queues a request with it (the client copies the request and its name
	// into a locked list). Empty in the server.
	virtual void Stub_91() = 0;

	// Many operators ask it about the entity behind a handle; the client copies a 1052-byte block of the
	// player slot of that entity (or of the local player) and returns true, the server returns 0.
	virtual void Stub_92() = 0;

	// The water impulse renderer calls it. Empty in the client and the server.
	virtual void Stub_93() = 0;

	// The client looks its argument up as a global symbol and returns the index found (0 without one).
	virtual void Stub_94() = 0;

	// Draws a debug line from vecOrigin to vecDest.
	virtual void DebugDrawLine( Vector vecOrigin, Vector vecDest, int r, int g, int b, bool bNoDepthTest, float flDuration ) = 0;

	// The client returns the debug name of the entity behind the handle its argument gives, "None" without
	// one; the server returns "None".
	virtual void Stub_96() = 0;

	// The model renderer passes a model name and uses what it returns; the client passes the string on to a
	// lookup of its own.
	virtual void Stub_97() = 0;

	// The light renderers call it per light; ( 1, 1 ) without an object (two floats).
	virtual void Stub_98() = 0;

	// The client physics operator calls it; the client passes the call on to its physics. Empty in the server.
	virtual void Stub_99() = 0;

	// The server returns 0.
	virtual void Stub_100() = 0;
};

#endif // IPARTICLESYSTEMQUERY_H
