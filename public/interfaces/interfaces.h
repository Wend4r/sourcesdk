//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef INTERFACES_H
#define INTERFACES_H

#if defined( COMPILER_MSVC )
#pragma once
#endif


//-----------------------------------------------------------------------------
// Interface creation function
//-----------------------------------------------------------------------------
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);


//-----------------------------------------------------------------------------
// Macros to declare interfaces appropriate for various tiers
//-----------------------------------------------------------------------------
#if 1 || defined( TIER1_LIBRARY ) || defined( TIER2_LIBRARY ) || defined( TIER3_LIBRARY ) || defined( TIER4_LIBRARY ) || defined( APPLICATION )
#define DECLARE_TIER1_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define DECLARE_TIER1_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( TIER2_LIBRARY ) || defined( TIER3_LIBRARY ) || defined( TIER4_LIBRARY ) || defined( APPLICATION )
#define DECLARE_TIER2_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define DECLARE_TIER2_INTERFACE( _Interface, _Global )
#endif

#if 1 || defined( TIER3_LIBRARY ) || defined( TIER4_LIBRARY ) || defined( APPLICATION )
#define DECLARE_TIER3_INTERFACE( _Interface, _Global )	extern _Interface * _Global;
#else
#define DECLARE_TIER3_INTERFACE( _Interface, _Global )
#endif


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ICvar;
class IProcessUtils;
class ILocalize;
class IPhysics2;
class IPhysics2ActorManager;
class IPhysics2ResourceManager;
class IEventSystem;

class IAsyncFileSystem;
class IColorCorrectionSystem;
class IDebugTextureInfo;
class IFileSystem;
class IRenderHardwareConfig;
class IInputSystem;
class IInputStackSystem;
class IMaterialSystem;
class IMaterialSystem2;
class IMaterialSystemHardwareConfig;
class IMdlLib;
class INetworkSystem;
class IP4;
class IQueuedLoader;
class IResourceAccessControl;
class IPrecacheSystem;
class IRenderDevice;
class IRenderDeviceMgr;
class IResourceSystem;
class IVBAllocTracker;
class IXboxInstaller;
class IMatchFramework;
class ISoundSystem;
class IStudioRender;
class IMatSystemSurface;
class IGameUISystemMgr;
class IDataCache;
class IMDLCache;
class IAvi;
class IBik;
class IDmeMakefileUtils;
class IPhysicsCollision;
class ISoundEmitterSystemBase;
class IMeshSystem;
class IWorldRendererMgr;
class ISceneSystem;
class IVGuiRenderSurface;

namespace vgui
{
	class ISurface;
	class IVGui;
	class IInput;
	class IPanel;
	class ILocalize;
	class ISchemeManager;
	class ISystem;
}



//-----------------------------------------------------------------------------
// Fills out global DLL exported interface pointers
//-----------------------------------------------------------------------------
#ifndef CVAR_INTERFACE_VERSION
#define CVAR_INTERFACE_VERSION					"VEngineCvar007"
DECLARE_TIER1_INTERFACE( ICvar, cvar );
DECLARE_TIER1_INTERFACE( ICvar, g_pCVar )
#endif

#ifndef PROCESS_UTILS_INTERFACE_VERSION
#define PROCESS_UTILS_INTERFACE_VERSION			"VProcessUtils002"
DECLARE_TIER1_INTERFACE( IProcessUtils, g_pProcessUtils );
#endif

#ifndef VPHYSICS2_INTERFACE_VERSION
#define VPHYSICS2_INTERFACE_VERSION				"Physics2 Interface v0.3"
DECLARE_TIER1_INTERFACE( IPhysics2, g_pPhysics2 );
#endif

#ifndef VPHYSICS2_ACTOR_MGR_INTERFACE_VERSION
#define VPHYSICS2_ACTOR_MGR_INTERFACE_VERSION	"Physics2 Interface ActorMgr v0.1"
DECLARE_TIER1_INTERFACE( IPhysics2ActorManager, g_pPhysics2ActorManager );
#endif

#ifndef VPHYSICS2_RESOURCE_MGR_INTERFACE_VERSION
#define VPHYSICS2_RESOURCE_MGR_INTERFACE_VERSION "Physics2 Interface ResourceMgr v0.1"
DECLARE_TIER1_INTERFACE( IPhysics2ResourceManager, g_pPhysics2ResourceManager );
#endif

#ifndef EVENTSYSTEM_INTERFACE_VERSION
#define EVENTSYSTEM_INTERFACE_VERSION "EventSystem001"
DECLARE_TIER1_INTERFACE( IEventSystem, g_pEventSystem );
#endif

#ifndef LOCALIZE_INTERFACE_VERSION
#define LOCALIZE_INTERFACE_VERSION 			"Localize_001"
DECLARE_TIER2_INTERFACE( ILocalize, g_pLocalize );
DECLARE_TIER3_INTERFACE( vgui::ILocalize, g_pVGuiLocalize );
#endif

#ifndef RENDER_DEVICE_MGR_INTERFACE_VERSION
#define RENDER_DEVICE_MGR_INTERFACE_VERSION		"RenderDeviceMgr001"
DECLARE_TIER2_INTERFACE( IRenderDeviceMgr, g_pRenderDeviceMgr );
#endif

#ifndef FILESYSTEM_INTERFACE_VERSION
#define FILESYSTEM_INTERFACE_VERSION			"VFileSystem017"
DECLARE_TIER2_INTERFACE( IFileSystem, g_pFullFileSystem );
#endif

#ifndef ASYNCFILESYSTEM_INTERFACE_VERSION
#define ASYNCFILESYSTEM_INTERFACE_VERSION		"VNewAsyncFileSystem001"
DECLARE_TIER2_INTERFACE( IAsyncFileSystem, g_pAsyncFileSystem );
#endif

#ifndef RESOURCESYSTEM_INTERFACE_VERSION
#define RESOURCESYSTEM_INTERFACE_VERSION		"ResourceSystem004"
DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );
#endif

#ifndef MATERIAL_SYSTEM_INTERFACE_VERSION
#define MATERIAL_SYSTEM_INTERFACE_VERSION		"VMaterialSystem080"
DECLARE_TIER2_INTERFACE( IMaterialSystem, materials );
DECLARE_TIER2_INTERFACE( IMaterialSystem, g_pMaterialSystem );
#endif

#ifndef MATERIAL_SYSTEM2_INTERFACE_VERSION
#define MATERIAL_SYSTEM2_INTERFACE_VERSION		"VMaterialSystem2_001"
DECLARE_TIER2_INTERFACE( IMaterialSystem2, g_pMaterialSystem2 );
#endif

#ifndef INPUTSYSTEM_INTERFACE_VERSION
#define INPUTSYSTEM_INTERFACE_VERSION			"InputSystemVersion001"
DECLARE_TIER2_INTERFACE( IInputSystem, g_pInputSystem );
#endif

#ifndef INPUTSTACKSYSTEM_INTERFACE_VERSION
#define INPUTSTACKSYSTEM_INTERFACE_VERSION		"InputStackSystemVersion001"
DECLARE_TIER2_INTERFACE( IInputStackSystem, g_pInputStackSystem );
#endif

#ifndef NETWORKSYSTEM_INTERFACE_VERSION
#define NETWORKSYSTEM_INTERFACE_VERSION			"NetworkSystemVersion001"
DECLARE_TIER2_INTERFACE( INetworkSystem, g_pNetworkSystem );
#endif

#ifndef MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION
#define MATERIALSYSTEM_HARDWARECONFIG_INTERFACE_VERSION		"MaterialSystemHardwareConfig013"
DECLARE_TIER2_INTERFACE( IMaterialSystemHardwareConfig, g_pMaterialSystemHardwareConfig );
#endif

#ifndef DEBUG_TEXTURE_INFO_VERSION
#define DEBUG_TEXTURE_INFO_VERSION				"DebugTextureInfo001"
DECLARE_TIER2_INTERFACE( IDebugTextureInfo, g_pMaterialSystemDebugTextureInfo );
#endif

#ifndef VB_ALLOC_TRACKER_INTERFACE_VERSION
#define VB_ALLOC_TRACKER_INTERFACE_VERSION		"VBAllocTracker001"
DECLARE_TIER2_INTERFACE( IVBAllocTracker, g_VBAllocTracker );
#endif

#ifndef COLORCORRECTION_INTERFACE_VERSION
#define COLORCORRECTION_INTERFACE_VERSION		"COLORCORRECTION_VERSION_1"
DECLARE_TIER2_INTERFACE( IColorCorrectionSystem, colorcorrection );
#endif

#ifndef P4_INTERFACE_VERSION
#define P4_INTERFACE_VERSION					"VP4002"
DECLARE_TIER2_INTERFACE( IP4, p4 );
#endif

#ifndef MDLLIB_INTERFACE_VERSION
#define MDLLIB_INTERFACE_VERSION				"VMDLLIB001"
DECLARE_TIER2_INTERFACE( IMdlLib, mdllib );
#endif

#ifndef QUEUEDLOADER_INTERFACE_VERSION
#define QUEUEDLOADER_INTERFACE_VERSION			"QueuedLoaderVersion001"
DECLARE_TIER2_INTERFACE( IQueuedLoader, g_pQueuedLoader );
#endif

#ifndef RESOURCE_ACCESS_CONTROL_INTERFACE_VERSION
#define RESOURCE_ACCESS_CONTROL_INTERFACE_VERSION	"VResourceAccessControl001"
DECLARE_TIER2_INTERFACE( IResourceAccessControl, g_pResourceAccessControl );
#endif

#ifndef PRECACHE_SYSTEM_INTERFACE_VERSION
#define PRECACHE_SYSTEM_INTERFACE_VERSION		"VPrecacheSystem001"
DECLARE_TIER2_INTERFACE( IPrecacheSystem, g_pPrecacheSystem );
#endif

#if defined( _X360 )
#	ifndef XBOXINSTALLER_INTERFACE_VERSION
#	define XBOXINSTALLER_INTERFACE_VERSION			"XboxInstallerVersion001"
DECLARE_TIER2_INTERFACE( IXboxInstaller, g_pXboxInstaller );
	#endif
#endif

#ifndef MATCHFRAMEWORK_INTERFACE_VERSION
#define MATCHFRAMEWORK_INTERFACE_VERSION		"MATCHFRAMEWORK_001"
DECLARE_TIER2_INTERFACE( IMatchFramework, g_pMatchFramework );
#endif

#ifndef GAMEUISYSTEMMGR_INTERFACE_VERSION
#define GAMEUISYSTEMMGR_INTERFACE_VERSION	"GameUISystemMgr001"
DECLARE_TIER3_INTERFACE( IGameUISystemMgr, g_pGameUISystemMgr );
#endif


//-----------------------------------------------------------------------------
// Not exactly a global, but we're going to keep track of these here anyways
// NOTE: Appframework deals with connecting these bad boys. See materialsystem2app.cpp
//-----------------------------------------------------------------------------
#ifndef RENDER_DEVICE_INTERFACE_VERSION
#define RENDER_DEVICE_INTERFACE_VERSION			"RenderDevice001"
DECLARE_TIER2_INTERFACE( IRenderDevice, g_pRenderDevice );
#endif

#ifndef RENDER_HARDWARECONFIG_INTERFACE_VERSION
#define RENDER_HARDWARECONFIG_INTERFACE_VERSION		"RenderHardwareConfig001"
DECLARE_TIER2_INTERFACE( IRenderHardwareConfig, g_pRenderHardwareConfig );
#endif

#ifndef SOUNDSYSTEM_INTERFACE_VERSION
#define SOUNDSYSTEM_INTERFACE_VERSION		"SoundSystem001"
DECLARE_TIER2_INTERFACE( ISoundSystem, g_pSoundSystem );
#endif

#ifndef MESHSYSTEM_INTERFACE_VERSION
#define MESHSYSTEM_INTERFACE_VERSION			"MeshSystem001"
DECLARE_TIER3_INTERFACE( IMeshSystem, g_pMeshSystem );
#endif

#ifndef STUDIO_RENDER_INTERFACE_VERSION
#define STUDIO_RENDER_INTERFACE_VERSION			"VStudioRender026"
DECLARE_TIER3_INTERFACE( IStudioRender, g_pStudioRender );
DECLARE_TIER3_INTERFACE( IStudioRender, studiorender );
#endif

#ifndef MAT_SYSTEM_SURFACE_INTERFACE_VERSION
#define MAT_SYSTEM_SURFACE_INTERFACE_VERSION	"MatSystemSurface006"
DECLARE_TIER3_INTERFACE( IMatSystemSurface, g_pMatSystemSurface );
#endif

#ifndef RENDER_SYSTEM_SURFACE_INTERFACE_VERSION
#define RENDER_SYSTEM_SURFACE_INTERFACE_VERSION	"RenderSystemSurface001"
DECLARE_TIER3_INTERFACE( IVGuiRenderSurface, g_pVGuiRenderSurface );
#endif

#ifndef SCENESYSTEM_INTERFACE_VERSION
#define SCENESYSTEM_INTERFACE_VERSION			"SceneSystem_001"
DECLARE_TIER3_INTERFACE( ISceneSystem, g_pSceneSystem );
#endif

#ifndef VGUI_SURFACE_INTERFACE_VERSION
#define VGUI_SURFACE_INTERFACE_VERSION			"VGUI_Surface031"
DECLARE_TIER3_INTERFACE( vgui::ISurface, g_pVGuiSurface );
#endif

#ifndef SCHEME_SURFACE_INTERFACE_VERSION
#define SCHEME_SURFACE_INTERFACE_VERSION		"SchemeSurface001"
#endif

#ifndef VGUI_INPUT_INTERFACE_VERSION
#define VGUI_INPUT_INTERFACE_VERSION			"VGUI_Input005"
DECLARE_TIER3_INTERFACE( vgui::IInput, g_pVGuiInput );
#endif

#ifndef VGUI_IVGUI_INTERFACE_VERSION
#define VGUI_IVGUI_INTERFACE_VERSION			"VGUI_ivgui008"
DECLARE_TIER3_INTERFACE( vgui::IVGui, g_pVGui );
#endif

#ifndef VGUI_PANEL_INTERFACE_VERSION
#define VGUI_PANEL_INTERFACE_VERSION			"VGUI_Panel009"
DECLARE_TIER3_INTERFACE( vgui::IPanel, g_pVGuiPanel );
#endif

#ifndef VGUI_SCHEME_INTERFACE_VERSION
#define VGUI_SCHEME_INTERFACE_VERSION			"VGUI_Scheme010"
DECLARE_TIER3_INTERFACE( vgui::ISchemeManager, g_pVGuiSchemeManager );
#endif

#ifndef VGUI_SYSTEM_INTERFACE_VERSION
#define VGUI_SYSTEM_INTERFACE_VERSION			"VGUI_System010"
DECLARE_TIER3_INTERFACE( vgui::ISystem, g_pVGuiSystem );
#endif

#ifndef DATACACHE_INTERFACE_VERSION
#define DATACACHE_INTERFACE_VERSION				"VDataCache003"
DECLARE_TIER3_INTERFACE( IDataCache, g_pDataCache );	// FIXME: Should IDataCache be in tier2?
#endif

#ifndef MDLCACHE_INTERFACE_VERSION
#define MDLCACHE_INTERFACE_VERSION				"MDLCache004"
DECLARE_TIER3_INTERFACE( IMDLCache, g_pMDLCache );
DECLARE_TIER3_INTERFACE( IMDLCache, mdlcache );
#endif

#ifndef AVI_INTERFACE_VERSION
#define AVI_INTERFACE_VERSION					"VAvi001"
DECLARE_TIER3_INTERFACE( IAvi, g_pAVI );
#endif

#ifndef BIK_INTERFACE_VERSION
#define BIK_INTERFACE_VERSION					"VBik001"
DECLARE_TIER3_INTERFACE( IBik, g_pBIK );
#endif

#ifndef DMEMAKEFILE_UTILS_INTERFACE_VERSION
#define DMEMAKEFILE_UTILS_INTERFACE_VERSION		"VDmeMakeFileUtils001"
DECLARE_TIER3_INTERFACE( IDmeMakefileUtils, g_pDmeMakefileUtils );
#endif

#ifndef VPHYSICS_COLLISION_INTERFACE_VERSION
#define VPHYSICS_COLLISION_INTERFACE_VERSION	"VPhysicsCollision007"
DECLARE_TIER3_INTERFACE( IPhysicsCollision, g_pPhysicsCollision );
#endif

#ifndef SOUNDEMITTERSYSTEM_INTERFACE_VERSION
#define SOUNDEMITTERSYSTEM_INTERFACE_VERSION	"VSoundEmitter003"
DECLARE_TIER3_INTERFACE( ISoundEmitterSystemBase, g_pSoundEmitterSystem );
#endif

#ifndef WORLD_RENDERER_MGR_INTERFACE_VERSION
#define WORLD_RENDERER_MGR_INTERFACE_VERSION	"WorldRendererMgr001"
DECLARE_TIER3_INTERFACE( IWorldRendererMgr, g_pWorldRendererMgr );
#endif

//-----------------------------------------------------------------------------
// Fills out global DLL exported interface pointers
//-----------------------------------------------------------------------------
void ConnectInterfaces( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectInterfaces();


//-----------------------------------------------------------------------------
// Reconnects an interface
//-----------------------------------------------------------------------------
void ReconnectInterface( CreateInterfaceFn factory, const char *pInterfaceName );


#endif // INTERFACES_H

