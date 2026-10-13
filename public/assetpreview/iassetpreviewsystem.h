//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Asset previews, thumbnails and the shared preview settings of the tools
//
//===========================================================================//

#ifndef IASSETPREVIEWSYSTEM_H
#define IASSETPREVIEWSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utldict.h"
#include "color.h"
#include "tier1/utlvector.h"
#include "bitmap/imageformat_declarations.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3dm.h"

class QObject;
class QWidget;
class QPixmap;
class KeyValues3;
class IAsset;
class IToolFrameListener;
class CToolSceneNode;
class CQAssetPreviewToolSceneWidget;
class CLightingControlRig;

//-----------------------------------------------------------------------------
// Previews created by IAssetPreviewSystem
//-----------------------------------------------------------------------------
abstract_class IAssetPreviewBase
{
public:
	virtual QWidget *GetWidget() = 0;
	virtual IAsset *GetAsset() = 0;
	virtual void SetAsset( IAsset *pAsset ) = 0;

	virtual ~IAssetPreviewBase() {}
};

// Thumbnail of the asset
abstract_class IAssetPreviewEmbedded : public IAssetPreviewBase
{
public:
	// Lets the widget start a drag once the left button moved far enough
	virtual void SetDragEnabled( bool bEnabled ) = 0;
};

// Asset rendered in a tool scene
abstract_class IAssetPreviewLive : public IAssetPreviewBase
{
public:
	virtual CQAssetPreviewToolSceneWidget *GetToolSceneWidget() = 0;
};

abstract_class IAssetPreviewExternal : public IAssetPreviewBase
{
};

abstract_class IAssetPreviewSettingsChangedListener
{
public:
	virtual void OnAssetPreviewSettingsChanged() = 0;
};

abstract_class ILiveEmbeddedPreviewStateListener
{
public:
	// Called with the new value by SetLivePreviewEnabled
	virtual void OnLiveEmbeddedPreviewStateChanged( bool bEnabled ) = 0;
};

// Creates the controller widgets of a tool scene node class
abstract_class IToolSceneNodeControllerFactory
{
public:
	virtual QWidget *CreateController( QWidget *pParent, QObject *pUnknown ) = 0;
	virtual void SetControllerNode( QWidget *pController, CToolSceneNode *pNode ) = 0;
};

// Contiguous elements passed by value: the elements and their 64-bit count
template < class T >
struct AssetPreviewSpan_t
{
	T *m_pElements;
	int64 m_nCount;
};

// Element of GetThumbnailFingerprints: the asset in, the fingerprint stored in the thumbnail
// database and whether it was found out
struct ThumbnailFingerprintQuery_t
{
	uint32 m_nFingerprint;
	bool m_bFound;
	IAsset *m_pAsset;
};

// Element of the second span of MaybeUpdateThumbnails, queued to the thumbnail database writer
struct ThumbnailDatabaseEntry_t
{
	uint64 m_nUnk000;
	uint32 m_nUnk008;
};

abstract_class IAssetPreviewSystem : public IAppSystem
{
public:
	// nUnknown 0 makes the thumbnail show the asset name as its tool tip
	virtual IAssetPreviewEmbedded *CreateEmbeddedPreview( QWidget *pParent, int nUnknown, void *pUnknown ) = 0;
	virtual void DestroyEmbeddedPreview( IAssetPreviewEmbedded *pPreview ) = 0;
	virtual IAssetPreviewLive *CreateLivePreview( QWidget *pParent ) = 0;
	virtual void DestroyLivePreview( IAssetPreviewLive *pPreview ) = 0;
	virtual IAssetPreviewExternal *CreateExternalPreview() = 0;
	virtual void DestroyExternalPreview( IAssetPreviewExternal *pPreview ) = 0;

	// Creates the two lighting control rigs and opens the thumbnail database when the argument is
	// true; the tool framework calls it during Init ("Initializing preview system...") with false
	// when "-disableassetsystem" is set
	virtual void CreateLightingControlRigs( bool bOpenThumbnailDatabase ) = 0;

	// Opens the thumbnail cache management window
	virtual void ShowThumbnailCacheManager() = 0;

	// Render through a tool scene with the editor camera. RenderAsset simulates particle systems
	// first; RenderMaterialToPixels accepts only materials and writes nWidth * nHeight pixels of
	// IMAGE_FORMAT_RGBA8888 or IMAGE_FORMAT_RGB888 to pPixels, failing for other formats
	virtual void RenderAsset( IAsset *pAsset, int nWidth, int nHeight, int nUnknown, void *pUnk5, void *pUnk6, void *pUnk7 ) = 0;
	virtual bool RenderMaterialToPixels( IAsset *pMaterial, int nWidth, int nHeight, ImageFormat nFormat, uint8 *pPixels ) = 0;

	// Updates the previews and the tool scene widgets every tools frame ("Asset Preview System")
	virtual void RunFrame() = 0;

	virtual void RegisterToolSceneNodeControllerFactory( IToolSceneNodeControllerFactory *pFactory, const char *pName ) = 0;

	// Queues the thumbnail for generation when it is not cached yet
	virtual bool GetThumbnail( IAsset *pAsset, QPixmap *pPixmap ) = 0;

	// Thumbnail database lookup by asset path
	virtual bool GetThumbnailFingerprints( AssetPreviewSpan_t< ThumbnailFingerprintQuery_t > queries ) = 0;

	virtual bool IsLivePreviewEnabled() = 0;
	virtual void SetLivePreviewEnabled( bool bEnabled ) = 0;

	virtual Color GetBackgroundColor() = 0;
	virtual void SetBackgroundColor( Color color ) = 0;

	virtual void SaveSettings( KeyValues3 *pSettings ) = 0;
	virtual void LoadSettings( KeyValues3 *pSettings ) = 0;

	virtual void AddLiveEmbeddedPreviewStateListener( ILiveEmbeddedPreviewStateListener *pListener ) = 0;
	virtual void RemoveLiveEmbeddedPreviewStateListener( ILiveEmbeddedPreviewStateListener *pListener ) = 0;

	virtual void ResetBackgroundColor() = 0;

	virtual void AddSettingsChangedListener( IAssetPreviewSettingsChangedListener *pListener ) = 0;
	virtual void RemoveSettingsChangedListener( IAssetPreviewSettingsChangedListener *pListener ) = 0;

	// Thumbnail cache updates of the assets; MaybeUpdateThumbnails is the thumbnail cache's
	// MaybeUpdateThumbnail for two spans
	virtual void MaybeUpdateThumbnail( AssetPreviewSpan_t< IAsset * > assets ) = 0;
	virtual void MaybeUpdateThumbnails( AssetPreviewSpan_t< IAsset * > assets, AssetPreviewSpan_t< ThumbnailDatabaseEntry_t > entries ) = 0;

	virtual bool IsPreviewCameraLocked() = 0;
	virtual void SetPreviewCameraLocked( bool bLocked ) = 0;
	virtual bool IsPreviewMenuEnabled() = 0;
	virtual void SetPreviewMenuEnabled( bool bEnabled ) = 0;
	virtual bool IsPreviewAnimSliderEnabled() = 0;
	virtual void SetPreviewAnimSliderEnabled( bool bEnabled ) = 0;
	virtual bool IsPreviewOverlayIconsEnabled() = 0;
	virtual void SetPreviewOverlayIconsEnabled( bool bEnabled ) = 0;
	virtual bool IsPreviewModelVertexFramingEnabled() = 0;
	virtual void SetPreviewModelVertexFramingEnabled( bool bEnabled ) = 0;

	virtual void ReloadLightRigConfig() = 0;

	// Waits for the thumbnail worker and clears its pending requests; the asset browser calls it before
	// resetting all tool settings
	virtual void FlushThumbnailQueue() = 0;

	// Cached command line switch, the name is hashed at compile time
	virtual bool HasCommandLineSwitch() = 0;

	// "Draw Skybox" of the live preview's tool scene widget
	virtual bool GetDrawSkybox( IAssetPreviewLive *pPreview ) = 0;
	virtual void SetDrawSkybox( IAssetPreviewLive *pPreview, bool bDraw ) = 0;

	virtual void PruneStaleThumbnails() = 0;

	// Records the current fingerprint of the asset in the thumbnail cache
	virtual void UpdateThumbnailFingerprint( IAsset *pAsset ) = 0;

	// Arguments of the "-generatethumbnailssubstr", "-generatethumbnailsmaxtime" (300 by default)
	// and "-generatethumbnailsclear" command line options
	virtual bool GenerateThumbnails( const char *pSubstr, int nMaxTime, bool bClear ) = 0;

	virtual ~IAssetPreviewSystem() {}
};

class CAssetPreviewSystem : public CTier3DmAppSystem< IAssetPreviewSystem >
{
public:
	CUtlVector< void * > m_vecUnk028;
	CUtlVector< IAssetPreviewSettingsChangedListener * > m_vecUnk040;
	uint8 m_unk058[ 0x28 ];

	// Live preview state, including the "live_preview" setting and the "background_color" setting
	uint8 m_unk080[ 0x70 ];

	// Light rig controllers
	void *m_pUnk0F0;
	void *m_pUnk0F8;

	bool m_bPreviewCameraLocked;
	bool m_bPreviewMenuEnabled;
	bool m_bPreviewAnimSliderEnabled;
	bool m_bPreviewOverlayIconsEnabled;
	bool m_bPreviewModelVertexFramingEnabled;

	double m_flUnk108;
	double m_flUnk110;
	uint8 m_unk118[ 0x58 ];
};

#endif // IASSETPREVIEWSYSTEM_H
