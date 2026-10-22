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

FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

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
	// pToolSceneWidget is the tool scene widget of the live preview the controller is created for
	virtual QWidget *CreateController( QWidget *pParent, CQAssetPreviewToolSceneWidget *pToolSceneWidget ) = 0;
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

// Element of both spans of MaybeUpdateThumbnails: the asset and its thumbnail fingerprint. The entries
// of the second span are queued to the thumbnail database writer, which reads the thumbnail of that
// path and fingerprint
struct ThumbnailDatabaseEntry_t
{
	IAsset *m_pAsset;
	uint32 m_nFingerprint;
};

abstract_class IAssetPreviewSystem : public IAppSystem
{
public:
	// With nUnknown 0 the thumbnail shows the asset name as its tool tip, a right click opens the asset
	// browser's context menu for the asset and a double click opens it in a tool; otherwise both clicks
	// are ignored. A left click on the thumbnail passes the asset to the second virtual of pUnknown, which
	// the context menu also receives
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
	// first and renders with the swap chain textures 0 and 1 of hSwapChain, reading the pixels back to pPixels when
	// set; pRenderAttributes, when set, is merged into the attributes of the tool scene (nUnknown is not
	// read). RenderMaterialToPixels accepts only materials and writes nWidth * nHeight pixels of
	// IMAGE_FORMAT_RGBA8888 or IMAGE_FORMAT_RGB888 to pPixels, failing for other formats
	virtual void RenderAsset( IAsset *pAsset, int nWidth, int nHeight, int nUnknown, uint8 *pPixels, void *pRenderAttributes, SwapChainHandle_t hSwapChain ) = 0;
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
	virtual void MaybeUpdateThumbnails( AssetPreviewSpan_t< ThumbnailDatabaseEntry_t > assets, AssetPreviewSpan_t< ThumbnailDatabaseEntry_t > entries ) = 0;

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
	// Embedded preview rendered live onto a page of m_LivePreviewPages
	struct EmbeddedLivePreview_t
	{
		QWidget *m_pWidget;
		void *m_pLivePreview;
		IAsset *m_pAsset;

		// Page and rectangle the widget copies its pixmap from, the page is -1 while the widget is hidden
		int m_nPage;
		int m_nX;
		int m_nY;
		int m_nWidth;
		int m_nHeight;

		uint8 m_unk02C[ 0x1C ];
	};

	// Tool scene widgets of the previews, run by RunFrame
	CUtlVector< IToolFrameListener * > m_ToolFrameListeners;

	CUtlVector< IAssetPreviewSettingsChangedListener * > m_SettingsChangedListeners;
	CUtlDict< IToolSceneNodeControllerFactory * > m_ToolSceneNodeControllerFactories;

	CUtlVector< EmbeddedLivePreview_t > m_EmbeddedLivePreviews;

	// Pages of the size of the monitor resolution the live previews are rendered to, deleted by Shutdown
	CUtlVector< QPixmap * > m_LivePreviewPages;
	int m_nLivePreviewPageWidth;
	int m_nLivePreviewPageHeight;

	// Plat_FloatTime of the last live preview render
	float m_flLastLivePreviewRenderTime;
	uint8 m_unk0BC[ 4 ];

	// CThreadEvent, whose size differs between the platforms
	uint8 m_unk0C0[ 0x10 ];

	// "live_preview" and "background_color" settings
	bool m_bLivePreviewEnabled;
	Color m_BackgroundColor;

	CUtlVector< ILiveEmbeddedPreviewStateListener * > m_LiveEmbeddedPreviewStateListeners;

	CLightingControlRig *m_pLightingControlRigs[ 2 ];

	bool m_bPreviewCameraLocked;
	bool m_bPreviewMenuEnabled;
	bool m_bPreviewAnimSliderEnabled;
	bool m_bPreviewOverlayIconsEnabled;
	bool m_bPreviewModelVertexFramingEnabled;

	// RunFrame copies the thumbnail cache statistics into m_unk118 at this interval
	double m_flNextThumbnailStatsTime;
	double m_flThumbnailStatsStartTime;
	uint8 m_unk118[ 0x58 ];
};

#endif // IASSETPREVIEWSYSTEM_H
