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
#include "color.h"
#include "appframework/iappsystem.h"

class QWidget;
class QPixmap;
class KeyValues3;
class IAssetPreviewBase;
class IAssetPreviewEmbedded;
class IAssetPreviewLive;
class IAssetPreviewExternal;
class IAssetPreviewSettingsChangedListener;
class IToolSceneNodeControllerFactory;

// Assets are asset system objects ( opaque here ). An "asset span" is 16 bytes passed by pointer:
// { entries, count }. Thumbnails live in CAssetThumbnailCache and its SQLite thumbnail database.
abstract_class IAssetPreviewSystem : public IAppSystem
{
public:
	// nDisplayFlags 0 shows the asset name as the tooltip; pContext is kept by the widget
	virtual IAssetPreviewEmbedded *CreateEmbeddedPreview( QWidget *pParent, int nDisplayFlags, void *pContext ) = 0;
	virtual void DestroyEmbeddedPreview( IAssetPreviewEmbedded *pPreview ) = 0;
	virtual IAssetPreviewLive *CreateLivePreview( QWidget *pParent ) = 0;
	virtual void DestroyLivePreview( IAssetPreviewLive *pPreview ) = 0;
	virtual IAssetPreviewExternal *CreateExternalPreview() = 0;
	virtual void DestroyExternalPreview( IAssetPreviewExternal *pPreview ) = 0;

	// Named by what it does: creates the two light rig controllers and, when bStartThumbnailDatabase, opens
	// the thumbnail database and starts its writer thread
	virtual void CreateLightRigs( bool bStartThumbnailDatabase ) = 0;

	// Opens the thumbnail cache management window
	virtual void ShowThumbnailCacheManager() = 0;

	// Named by what they do: render a resource through a tool scene with the editor camera ( pOutput gets the
	// "Toolscene screen space view", hColorTarget is an optional render target, pRenderContext the render
	// context ), or render a material resource ( type "vmat" ) into pOutput; nWidth x nHeight pixels
	virtual void RenderResourceToolScene( void *pResource, int nWidth, int nHeight, int nFlags, void *pOutput, void *hColorTarget, void *pRenderContext ) = 0;
	virtual bool RenderMaterialToBuffer( void *pMaterial, int nWidth, int nHeight, int nFormat, void *pOutput ) = 0;

	// Named by what it does: runs the frame listeners of the live previews and the thumbnail cache; called
	// by the tool framework every frame
	virtual void RunFrame() = 0;

	virtual void RegisterToolSceneNodeControllerFactory( IToolSceneNodeControllerFactory *pFactory, const char *pName ) = 0;

	// Named by what they do: the cached thumbnail of the asset ( queued when missing ), and a lookup of the
	// assets of an asset span ( 16-byte entries, asset at +8 ) in the thumbnail database
	virtual bool GetThumbnail( const void *pAsset, QPixmap *pPixmap ) = 0;
	virtual bool QueryThumbnailDatabase( const void *pAssetSpan ) = 0;

	virtual bool IsLivePreviewEnabled() = 0;
	virtual void SetLivePreviewEnabled( bool bEnabled ) = 0;

	virtual Color GetBackgroundColor() = 0;
	virtual void SetBackgroundColor( Color color ) = 0;

	virtual void SaveSettings( KeyValues3 *pSettings ) = 0;
	virtual void LoadSettings( KeyValues3 *pSettings ) = 0;

	// Named by what they do: keep a vector of listeners that assetpreview.dll itself never calls
	virtual void AddListener( void *pListener ) = 0;
	virtual void RemoveListener( void *pListener ) = 0;

	virtual void ResetBackgroundColor() = 0;

	virtual void AddSettingsChangedListener( IAssetPreviewSettingsChangedListener *pListener ) = 0;
	virtual void RemoveSettingsChangedListener( IAssetPreviewSettingsChangedListener *pListener ) = 0;

	// pAssetSpan holds asset pointers ( 8-byte entries ); the second form takes 16-byte { asset, data }
	// entries and a span of thumbnail database records
	virtual void MaybeUpdateThumbnail( const void *pAssetSpan ) = 0;
	virtual void MaybeUpdateThumbnailsWithRecords( const void *pAssetSpan, const void *pRecordSpan ) = 0;

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

	// Named by what it does: waits for the pending database writes, resets the cache statistics and queues
	// the resident thumbnails again
	virtual void ResetThumbnailCache() = 0;

	// Named by what it does: a cached CommandLine()->HasParm of a switch hashed at compile time
	// ( MurmurHash2 0x54EFA337 )
	virtual bool HasPreviewCommandLineSwitch() = 0;

	// The "draw skybox" state of the preview's scene widget ( CQToolSceneWidget::DrawSkyboxChanged )
	virtual bool IsSkyboxDrawn( IAssetPreviewBase *pPreview ) = 0;
	virtual void SetDrawSkybox( IAssetPreviewBase *pPreview, bool bDraw ) = 0;

	virtual void PruneStaleThumbnails() = 0;

	// Named by what it does: records the current fingerprint of the asset in the thumbnail database's map
	virtual int RecordThumbnailFingerprint( const void *pAsset ) = 0;

	// pNameFilter keeps the assets whose name contains it; gives up after nTimeoutSeconds ( 0: never );
	// bResetCache calls ResetThumbnailCache first
	virtual bool GenerateThumbnails( const char *pNameFilter, int nTimeoutSeconds, bool bResetCache ) = 0;

	virtual ~IAssetPreviewSystem() {}
};

#endif // IASSETPREVIEWSYSTEM_H
