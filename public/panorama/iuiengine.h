//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: The panorama UI engine
//
//===========================================================================//

#ifndef IUIENGINE_H
#define IUIENGINE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/platwindow.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "tier1/utldelegate.h"
#include "inputsystem/iinputstacksystem.h"
#include "panorama/panoramasymbol.h"

class ISteamHTMLSurface;

namespace v8
{
	class Isolate;
};

namespace panorama
{

class IUIWindow;
class IUIPanel;
class IUIPanelClient;
class IUIPanelStyle;
class IUISettings;
class IUITextLayout;
class IUIInput;
class IUILocalization;
class IUISoundSystem;
class IUILayoutManager;
class IUIFileSystem;
class IUIImageManager;
struct PanelHandle_t;
struct RegisterJSScopeInfo_t;
struct RegisterJSEntryInfo_t;

// Declared up to the last Windows slot; Linux has seven more slots after it.
abstract_class IUIEngine
{
public:
	virtual ~IUIEngine() {}

	// Loads the panorama manifest and resource types
	virtual bool StartupSubsystems( IUISettings *pSettings, PlatWindow_t hWindow ) = 0;

	// Registers the console variables of the engine
	virtual void ConCommandInit() = 0;

	virtual void Shutdown() = 0;

	// Makes BIsRunning return false
	virtual void RequestShutdown() = 0;

	virtual void RunFrame() = 0;
	virtual bool BIsRunning() = 0;

	// True when one of the windows answers IUIWindow::IsWindowFocused
	virtual bool BHasFocus() = 0;

	virtual double GetCurrentFrameTime() = 0;

	// A window drawn straight to the back buffer; the engine keeps it in its window list
	virtual IUIWindow *CreateNewUILayerWindow( int x, int y, int nWidth, int nHeight, bool bFixedSurfaceSize, bool bEnforceWindowAspectRatio, bool bAcceptKBandMouse, const char *pszName, InputContextHandle_t hInputContext ) = 0;

	// Unlike a UI layer window, it does not release mouse buttons the input system reports as up
	// (IUIWindow::BUseAutoMouseUpBehavior is false), which a window the input system does not
	// pump needs; it is created with bAcceptKBandMouse false, so after every unhandled left or right
	// click the window clears the input focus and a text entry never keeps it
	virtual IUIWindow *CreateNewOffscreenUIWindow( int nWidth, int nHeight, const char *pszName, InputContextHandle_t hInputContext, bool bDrawToBackBuffer ) = 0;

	// A hidden window without a render surface, kept in the window list
	virtual IUIWindow *unk011( const char *pszName ) = 0;

	virtual bool DestroyWindow( IUIWindow *pWindow ) = 0;

	// The remaining arguments are not recovered
	virtual void Unk_CreateTextLayout( const char *pszText ) = 0;
	virtual void Unk_CreateTextLayout( const uchar16 *pszText ) = 0;
	virtual void Unk_CreateTextLayout( const uchar32 *pszText ) = 0;
	virtual void FreeTextLayout( IUITextLayout *pLayout ) = 0;
	virtual const CUtlVector< CUtlString > &GetSortedValidFontNames() = 0;

	virtual IUIInput *UIInputEngine() = 0;
	virtual IUILocalization *UILocalize() = 0;
	virtual IUISoundSystem *UISoundSystem() = 0;
	virtual IUISettings *UISettings() = 0;
	virtual IUILayoutManager *UILayoutManager() = 0;
	virtual IUIFileSystem *UIFileSystem() = 0;
	virtual IUIImageManager *UIImageManager() = 0;

	virtual void Unk_RegisterFrameFunc( void *p ) = 0;
	virtual void ReloadLayoutFile( CPanoramaSymbol symPath ) = 0;

	// Opens the debugger window, or closes it when it is open
	virtual void ToggleDebugMode() = 0;

	virtual void *Unk_GetConsoleHistory() = 0;

	virtual IUIPanel *CreatePanel( IUIWindow *pWindow ) = 0;

	// Creates a panel of a registered panel type through its factory
	virtual IUIPanelClient *CreatePanel( CPanoramaSymbol symPanelType, const char *pszID, IUIPanel *pParent ) = 0;

	virtual void PanelDestroyed( IUIPanel *pPanel ) = 0;
	virtual bool IsValidPanelPointer( const IUIPanel *pPanel ) = 0;
	virtual PanelHandle_t GetPanelHandle( const IUIPanel *pPanel ) = 0;
	virtual IUIPanel *GetPanelPtr( const PanelHandle_t &hPanel ) = 0;

	// Queues the panel for a callback to its client before the next style and layout pass
	virtual void CallBeforeStyleAndLayout( IUIPanel *pPanel ) = 0;

	// Queues the panel in a second per-frame set
	virtual void unk036( IUIPanel *pPanel ) = 0;

	// The IUIPanelClient overloads forward to the IUIPanel ones
	virtual void RegisterEventHandler( CPanoramaSymbol symEvent, IUIPanel *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterEventHandler( CPanoramaSymbol symEvent, IUIPanel *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void RegisterEventHandler( CPanoramaSymbol symEvent, IUIPanelClient *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterEventHandler( CPanoramaSymbol symEvent, IUIPanelClient *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterEventHandlersForPanel( IUIPanel *pPanel ) = 0;

	// The delegate is called with its object as the only argument. nUnk 2 takes the handler as returning
	// bool, any other value as returning nothing; every handler of the event is called either way
	virtual void RegisterForUnhandledEvent( CPanoramaSymbol symEvent, const CUtlAbstractDelegate &del, uint8 nUnk ) = 0;
	virtual void UnregisterForUnhandledEvent( CPanoramaSymbol symEvent, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterForUnhandledEvents( void *pEventHandler ) = 0;

	virtual bool BHaveEventHandlersRegisteredForType( CPanoramaSymbol symPanelType ) = 0;
	virtual void RegisterPanelTypeEventHandler( CPanoramaSymbol symEvent, CPanoramaSymbol symPanelType, const CUtlAbstractDelegate &del, bool bThisPtrIsUIPanel ) = 0;

	// Take ownership of the event, passed as an owning pointer by value
	virtual bool Unk_DispatchEvent( void *p ) = 0;
	virtual void Unk_DispatchEventAsync( float flDelay, void *p ) = 0;

	virtual void LayoutAndPaintWindows() = 0;

	// Maps a "{name}" path, optionally prefixed with "@mod", to a local directory
	virtual void RegisterNamedLocalPath( const char *pszPathName, const char *pszLocalPath ) = 0;

	// Reads the MOD search paths of the file system
	virtual void unk051() = 0;

	virtual void GetLocalPathForRelativePath( const char *pszPathName, const char *pszRelativePath, CUtlString &sLocalPath ) = 0;

	virtual ISteamHTMLSurface *AccessHTMLController() = 0;

	// Prints the message instead in headless mode
	virtual bool ShowNativeTopMostMessageBox( const char *pszMessage, const char *pszTitle, int eType ) = 0;

	virtual void RegisterMouseCanActivateParent( IUIPanel *pPanel, const char *pszParent ) = 0;
	virtual void UnregisterMouseCanActivateParent( IUIPanel *pPanel ) = 0;
	virtual const char *GetMouseCanActivateParent( IUIPanel *pPanel ) = 0;

	virtual bool BAnyWindowHasFocus() = 0;
	virtual bool BAnyVisibleWindowHasFocus() = 0;
	virtual IUIWindow *GetFocusedWindow() = 0;

	virtual double GetLastInputTime() = 0;
	virtual void UpdateLastInputTime() = 0;

	virtual void ClearClipboard() = 0;
	virtual void CopyToClipboard( const char *pszText, const char *pszPasteStringLocToken ) = 0;
	virtual void GetClipboardText( CUtlString &sText, CUtlString *pPasteStringLocToken ) = 0;

	// ELanguage values
	virtual int GetDisplayLanguage() = 0;
	virtual int GetCurrentInputLocale() = 0;
	virtual bool BHaveInputLocale( int eLanguage ) = 0;
	virtual void SetInputLocale( int eLanguage ) = 0;

	virtual IUIPanelStyle *AllocPanelStyle( IUIPanel *pPanel ) = 0;
	virtual void FreePanelStyle( IUIPanelStyle *pStyle ) = 0;

	virtual void SetPanelWaitingAsyncDelete( IUIPanel *pPanel ) = 0;
	virtual bool BIsPanelWaitingAsyncDelete( IUIPanel *pPanel ) = 0;

	virtual void MarkLayerToRepaintThreadSafe( uint64 ulCompositionLayerID ) = 0;

	virtual void AddDirectoryChangeWatch( const char *pszPath ) = 0;

	virtual uint32 GetWheelScrollLines() = 0;

	virtual void Unk_RunScript( void *p ) = 0;
	virtual void Unk_RunFunction( void *p ) = 0;
	virtual void Unk_ExposeObjectTypeToJavaScript( void *p ) = 0;
	virtual bool Unk_IsObjectTypeExposedToJavaScript( void *p ) = 0;
	virtual void Unk_ExposeGlobalObjectToJavaScript( void *p ) = 0;
	virtual void Unk_ClearGlobalObjectForJavaScript( void *p ) = 0;
	virtual void Unk_DeleteJSObjectInstance( void *p ) = 0;
	virtual void Unk_GetPanelForJavaScriptContext( void *p ) = 0;
	virtual void Unk_GetJavaScriptContextForPanel( void *p ) = 0;
	virtual void Unk_OutputJSExceptionToConsole( void *p ) = 0;
	virtual void Unk_AddGlobalV8FunctionTemplate( void *p ) = 0;
	virtual void *Unk_GetV8GlobalContext() = 0;

	// V8 class template getters: the first returns its handle by value
	virtual void unk089() = 0;
	virtual void *unk090() = 0;

	virtual void *Unk_UIStyleFactory() = 0;
	virtual v8::Isolate *GetV8Isolate() = 0;

	// Returns a function pointer
	virtual void *unk093() = 0;

	virtual void Unk_CreateV8PanelInstance( void *p ) = 0;
	virtual void Unk_CreateV8PanelStyleInstance( void *p ) = 0;
	virtual void Unk_CreateV8IUIWindowInstance( void *p ) = 0;
	virtual void Unk_CreateV8ObjectInstance( void *p ) = 0;

	virtual void Unk_RegisterEventWithEngine( void *p ) = 0;
	virtual bool IsValidEventName( CPanoramaSymbol symEvent ) = 0;
	virtual bool IsValidPanelEvent( CPanoramaSymbol symEvent, int *pParams ) = 0;
	virtual void Unk_CreateInputEventFromSymbol( void *p ) = 0;
	virtual void Unk_CreateEventFromString( void *p ) = 0;
	virtual void Unk_CreateEventsFromString( void *p ) = 0;

	virtual void Unk_RegisterPanelFactoryWithEngine( void *p ) = 0;
	virtual bool BRegisteredPanelType( CPanoramaSymbol symPanelType ) = 0;

	// Turn the engine's debugger state on or off and dispatch the CreateDebuggerWindow or
	// CloseDebuggerWindow event; the window itself is created by whoever handles the event
	virtual void CreateDebuggerWindow() = 0;
	virtual void CloseDebuggerWindow() = 0;

	virtual bool unk108() = 0;
	virtual void unk109( bool bUnk ) = 0;

	virtual void Unk_RegisterScheduledDelegate( void *p ) = 0;
	virtual void CancelScheduledDelegate( int iScheduleIndex ) = 0;
	virtual double GetLastScheduledDelegateRunTime() = 0;

	virtual CPanoramaSymbol MakeSymbol( const char *pszText ) = 0;

	// Symbol table helpers
	virtual void unk114() = 0;
	virtual void unk115() = 0;
	virtual void unk116() = 0;

	virtual void Unk_QueueDecrementRefNextFrame( void *p ) = 0;

	virtual void Unk_RegisterJSGenericCallback( void *p ) = 0;
	virtual void Unk_InvokeJSGenericCallback( void *p ) = 0;
	virtual void UnregisterJSGenericCallback( int nHandle ) = 0;

	virtual int GetNumRegisterJSScopes() = 0;
	virtual void GetRegisterJSScopeInfo( int nScope, RegisterJSScopeInfo_t *pInfo ) = 0;
	virtual void GetRegisterJSEntryInfo( int nScope, int nEntry, RegisterJSEntryInfo_t *pInfo ) = 0;
	virtual int StartRegisterJSScope( const char *pszName, const char *pszDesc ) = 0;
	virtual void EndRegisterJSScope() = 0;
	virtual void Unk_NewRegisterJSEntry( void *p ) = 0;
	virtual void Unk_SetRegisterJSEntryParams( void *p ) = 0;

	virtual bool Unk_BMatchDomainForJSRequest( void *p ) = 0;

	virtual void ClearFileCache() = 0;
	virtual void PrintCacheStatus() = 0;

	virtual void GetWindowsForDebugger( CUtlVector< IUIWindow * > &vecWindows ) = 0;

	virtual void SetPaintCountTrackingEnabled( bool bEnabled ) = 0;
	virtual bool GetPaintCountTrackingEnabled() = 0;
	virtual void IncrementPaintCountForPanel( uint64 ulPanelPtrValue, bool bRequiredCompositionLayer, double flFrameTime ) = 0;
	virtual void GetPanelPaintInfo( uint64 ulPanelPtrValue, uint32 &unMaxPanelPaintCount, uint32 &unPaintCount, bool &bRequiredCompositionLayer, double &flFrameTimeLastPaint ) = 0;

	virtual bool BHasAnyWindows() = 0;

	virtual void unk137( bool bUnk ) = 0;

	// Writes the XML schema of the registered panel types
	virtual void unk138() = 0;

	virtual void unk139() = 0;
	virtual void unk140() = 0;

	virtual void CaptureJSStackTrace( bool bPrint ) = 0;

	virtual void unk142() = 0;
	virtual void unk143() = 0;

	virtual void CopyToClipboardImpl( const char *pszText ) = 0;
	virtual void GetClipboardTextImpl( CUtlString &sText ) = 0;

	virtual void unk146() = 0;
	virtual void unk147() = 0;

	// Runs the input engine and gamepad frame
	virtual void RunPlatformFrame() = 0;

	virtual void unk149() = 0;
};

} // namespace panorama

#endif // IUIENGINE_H
