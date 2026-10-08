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
class KeyValues3;
class IAssetPreviewBase;
class IAssetPreviewEmbedded;
class IAssetPreviewLive;
class IAssetPreviewExternal;
class IAssetPreviewSettingsChangedListener;
class IToolSceneNodeControllerFactory;

abstract_class IAssetPreviewSystem : public IAppSystem
{
public:
	virtual IAssetPreviewEmbedded *CreateEmbeddedPreview( QWidget *pParent, int nUnknown, void *pUnknown ) = 0;
	virtual void DestroyEmbeddedPreview( IAssetPreviewEmbedded *pPreview ) = 0;
	virtual IAssetPreviewLive *CreateLivePreview( QWidget *pParent ) = 0;
	virtual void DestroyLivePreview( IAssetPreviewLive *pPreview ) = 0;
	virtual IAssetPreviewExternal *CreateExternalPreview() = 0;
	virtual void DestroyExternalPreview( IAssetPreviewExternal *pPreview ) = 0;

	// Creates the two light rig controllers
	virtual void unk017( bool bUnknown ) = 0;

	// Opens the thumbnail cache management window
	virtual void ShowThumbnailCacheManager() = 0;

	// Render through a tool scene with the editor camera
	virtual void unk019( void *pUnk1, int nUnk2, int nUnk3, int nUnk4, void *pUnk5, void *pUnk6, void *pUnk7 ) = 0;
	virtual bool unk020( void *pUnk1, int nUnk2, int nUnk3, int nUnk4, void *pUnk5 ) = 0;

	virtual void unk021() = 0;

	virtual void RegisterToolSceneNodeControllerFactory( IToolSceneNodeControllerFactory *pFactory, const char *pName ) = 0;

	virtual bool unk023( void *pUnk1, void *pUnk2 ) = 0;

	// Thumbnail database lookup by asset path
	virtual bool unk024( void *pUnk ) = 0;

	virtual bool IsLivePreviewEnabled() = 0;
	virtual void unk026( bool bUnknown ) = 0;

	virtual Color GetBackgroundColor() = 0;
	virtual void SetBackgroundColor( Color color ) = 0;

	virtual void SaveSettings( KeyValues3 *pSettings ) = 0;
	virtual void LoadSettings( KeyValues3 *pSettings ) = 0;

	virtual void unk031( void *pListener ) = 0;
	virtual void unk032( void *pListener ) = 0;

	virtual void ResetBackgroundColor() = 0;

	virtual void AddSettingsChangedListener( IAssetPreviewSettingsChangedListener *pListener ) = 0;
	virtual void RemoveSettingsChangedListener( IAssetPreviewSettingsChangedListener *pListener ) = 0;

	virtual void MaybeUpdateThumbnail( void *pAsset ) = 0;
	virtual void unk037( void *pAsset1, void *pAsset2 ) = 0;

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

	virtual void *unk049() = 0;

	// Cached command line switch
	virtual bool unk050() = 0;

	// Get and set a flag on the preview's scene
	virtual bool unk051( IAssetPreviewBase *pPreview ) = 0;
	virtual void unk052( IAssetPreviewBase *pPreview, bool bValue ) = 0;

	virtual void PruneStaleThumbnails() = 0;
	virtual void *unk054( void *p ) = 0;

	virtual bool GenerateThumbnails( void *pUnk1, int nUnk2, bool bUnk3 ) = 0;

	virtual ~IAssetPreviewSystem() {}
};

#endif // IASSETPREVIEWSYSTEM_H
