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
#include "tier0/utllinkedlist.h"
#include "tier0/smartptr.h"
#include "tier1/utldelegate.h"
#include "inputsystem/iinputstacksystem.h"
#include "panorama/panoramasymbol.h"

class ISteamHTMLSurface;
class IRefCounted;
class CBufferString;

namespace v8
{
	class Isolate;
	class Context;
	class TryCatch;
	class Value;
	class Function;
	class FunctionTemplate;
	class ObjectTemplate;
	class Object;
	template < class T > class Local;
	template < class T > class Global;
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
class IUIStyleFactory;
class IUIEvent;
class IUIJSObject;
class CPanel2DFactory;
class CUILength;
struct PanelHandle_t;
struct RegisterJSScopeInfo_t;
struct RegisterJSEntryInfo_t;
struct UIEventFactory;

// Line and column the source of a script starts at, for the script origin of V8
struct ScriptLineColumn_t
{
	int m_nLine;
	int m_nColumn;
};

// The implementation is CUIEngineSource2. It has three more bases after this table (a resource
// status listener and a render device listener); on Linux the primary table repeats the seven
// overrides of their methods after the last slot here, which this interface leaves out.
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

	// Named by what it does: an invisible window that never gets a render surface (no
	// BInitializeSurface), marked with IUIWindow::SetSurfaceless and kept in the window list
	virtual IUIWindow *CreateNewSurfacelessUIWindow( const char *pszName ) = 0;

	virtual bool DestroyWindow( IUIWindow *pWindow ) = 0;

	// Fill the layout parameters of the text services from the arguments and the display language
	virtual IUITextLayout *CreateTextLayout( const char *pszText, const char *pszFontName, float flFontSize, const CUILength &lineHeight, int8 eFontWeight, int8 eFontStyle, int8 eFontStretch, int8 eTextAlign, bool bJapaneseLineBreaking, bool bWrap, float flLetterSpacing, float flMaxWidth, float flMaxHeight, float flParagraphSpacing ) = 0;
	virtual IUITextLayout *CreateTextLayout( const uchar16 *pszText, const char *pszFontName, float flFontSize, const CUILength &lineHeight, int8 eFontWeight, int8 eFontStyle, int8 eFontStretch, int8 eTextAlign, bool bJapaneseLineBreaking, bool bWrap, float flLetterSpacing, float flMaxWidth, float flMaxHeight, float flParagraphSpacing ) = 0;
	virtual IUITextLayout *CreateTextLayout( const uchar32 *pszText, const char *pszFontName, float flFontSize, const CUILength &lineHeight, int8 eFontWeight, int8 eFontStyle, int8 eFontStretch, int8 eTextAlign, bool bJapaneseLineBreaking, bool bWrap, float flLetterSpacing, float flMaxWidth, float flMaxHeight, float flParagraphSpacing ) = 0;
	virtual void FreeTextLayout( IUITextLayout *pLayout ) = 0;
	virtual const CUtlVector< CUtlString > &GetSortedValidFontNames() = 0;

	virtual IUIInput *UIInputEngine() = 0;
	virtual IUILocalization *UILocalize() = 0;
	virtual IUISoundSystem *UISoundSystem() = 0;
	virtual IUISettings *UISettings() = 0;
	virtual IUILayoutManager *UILayoutManager() = 0;
	virtual IUIFileSystem *UIFileSystem() = 0;
	virtual IUIImageManager *UIImageManager() = 0;

	// Called at the start of every RunFrame
	virtual void RegisterFrameFunc( void ( *pfnFrameFunc )() ) = 0;
	virtual void ReloadLayoutFile( CPanoramaSymbol symPath ) = 0;

	// Opens the debugger window, or closes it when it is open
	virtual void ToggleDebugMode() = 0;

	virtual CUtlLinkedList< CUtlString > &GetConsoleHistory() = 0;

	virtual IUIPanel *CreatePanel( IUIWindow *pWindow ) = 0;

	// Creates a panel of a registered panel type through its factory
	virtual IUIPanelClient *CreatePanel( CPanoramaSymbol symPanelType, const char *pszID, IUIPanel *pParent ) = 0;

	virtual void PanelDestroyed( IUIPanel *pPanel ) = 0;
	virtual bool IsValidPanelPointer( const IUIPanel *pPanel ) = 0;
	virtual PanelHandle_t GetPanelHandle( const IUIPanel *pPanel ) = 0;
	virtual IUIPanel *GetPanelPtr( const PanelHandle_t &hPanel ) = 0;

	// Queues the panel for a callback to its client before the next style and layout pass
	virtual void CallBeforeStyleAndLayout( IUIPanel *pPanel ) = 0;

	// Named by what it does: queues the panel in a second set; the next RunFrame calls another
	// client callback for it right after the CallBeforeStyleAndLayout ones
	virtual void QueueDeferredPanelCallback( IUIPanel *pPanel ) = 0;

	// The IUIPanelClient overloads forward to the IUIPanel ones
	virtual void RegisterEventHandler( CPanoramaSymbol symEvent, IUIPanel *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterEventHandler( CPanoramaSymbol symEvent, IUIPanel *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void RegisterEventHandler( CPanoramaSymbol symEvent, IUIPanelClient *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterEventHandler( CPanoramaSymbol symEvent, IUIPanelClient *pPanel, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterEventHandlersForPanel( IUIPanel *pPanel ) = 0;

	// The delegate is called with its object as the only argument. nHandlerType 2 takes the handler
	// as returning bool, any other value as returning nothing; every handler of the event is called
	// either way
	virtual void RegisterForUnhandledEvent( CPanoramaSymbol symEvent, const CUtlAbstractDelegate &del, uint8 nHandlerType ) = 0;
	virtual void UnregisterForUnhandledEvent( CPanoramaSymbol symEvent, const CUtlAbstractDelegate &del ) = 0;
	virtual void UnregisterForUnhandledEvents( void *pEventHandler ) = 0;

	virtual bool BHaveEventHandlersRegisteredForType( CPanoramaSymbol symPanelType ) = 0;
	virtual void RegisterPanelTypeEventHandler( CPanoramaSymbol symEvent, CPanoramaSymbol symPanelType, const CUtlAbstractDelegate &del, bool bThisPtrIsUIPanel ) = 0;

	// Take ownership of the event; off the main thread DispatchEvent drops it and returns true
	virtual bool DispatchEvent( CPlainAutoPtr< IUIEvent > pEvent ) = 0;
	virtual void DispatchEventAsync( float flDelay, CPlainAutoPtr< IUIEvent > pEvent ) = 0;

	virtual void LayoutAndPaintWindows() = 0;

	// Maps a "{name}" path, optionally prefixed with "@mod", to a local directory
	virtual void RegisterNamedLocalPath( const char *pszPathName, const char *pszLocalPath ) = 0;

	// Registers the relative font path under every MOD search path with the text services
	virtual void RegisterCustomFontPath( const char *pszFontPath ) = 0;

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

	// Compiles (or takes from the script cache) and runs the script in the context of the panel
	virtual void RunScript( IUIPanel *pPanelContext, const char *pszScript, const char *pszSourceFilename, ScriptLineColumn_t origin ) = 0;

	// bPrintResult writes "=> result" to the console of the panel
	virtual void RunFunction( IUIPanel *pPanelContext, v8::Global< v8::Function > *pFunction, int nArgs, v8::Local< v8::Value > *pArgs, bool bPrintResult ) = 0;

	virtual void ExposeObjectTypeToJavaScript( const char *pszObjectTypeName, CUtlAbstractDelegate &del ) = 0;
	virtual bool IsObjectTypeExposedToJavaScript( const char *pszObjectTypeName ) = 0;
	virtual void ExposeGlobalObjectToJavaScript( const char *pszJSVarName, void *pInstance, const char *pszJSTypeName, bool bTrueGlobal ) = 0;
	virtual void ClearGlobalObjectForJavaScript( const char *pszJSVarName ) = 0;
	virtual void DeleteJSObjectInstance( IUIJSObject *pInstance ) = 0;
	virtual IUIPanel *GetPanelForJavaScriptContext( v8::Context *pContext ) = 0;
	virtual v8::Global< v8::Context > *GetJavaScriptContextForPanel( IUIPanel *pPanel ) = 0;
	virtual void OutputJSExceptionToConsole( v8::TryCatch &tryCatch, IUIPanel *pPanelContext ) = 0;
	virtual void AddGlobalV8FunctionTemplate( const char *pszJSFuncName, v8::Local< v8::FunctionTemplate > *pFunc, bool bTrueGlobal ) = 0;
	virtual v8::Global< v8::Context > &GetV8GlobalContext() = 0;

	// The object template and the type name ExposeObjectTypeToJavaScript is setting up
	virtual v8::Local< v8::ObjectTemplate > GetCurrentV8ObjectTemplateToSetup() = 0;
	virtual const char *GetCurrentV8ObjectTypeNameToSetup() = 0;

	virtual IUIStyleFactory *UIStyleFactory() = 0;
	virtual v8::Isolate *GetV8Isolate() = 0;

	// Named by what it does: the v8::PromiseRejectCallback RunFunction pushes with
	// ISource2V8System::PushPromiseRejectCallback while the function runs
	virtual void *GetPromiseRejectCallback() = 0;

	virtual v8::Global< v8::Object > *CreateV8PanelInstance( IUIPanel *pPanel ) = 0;
	virtual v8::Global< v8::Object > *CreateV8PanelStyleInstance( IUIPanelStyle *pPanelStyle ) = 0;
	virtual v8::Global< v8::Object > *CreateV8IUIWindowInstance( IUIWindow *pWindow ) = 0;
	virtual v8::Global< v8::Object > *CreateV8ObjectInstance( const char *pszObjectType, void *pActualObject, IUIJSObject *pJSObject ) = 0;

	virtual void RegisterEventWithEngine( CPanoramaSymbol symEvent, const UIEventFactory &factory ) = 0;
	virtual bool IsValidEventName( CPanoramaSymbol symEvent ) = 0;
	virtual bool IsValidPanelEvent( CPanoramaSymbol symEvent, int *pParams ) = 0;
	virtual CPlainAutoPtr< IUIEvent > CreateInputEventFromSymbol( CPanoramaSymbol symEvent, IUIPanel *pPanel, uint8 eSource, int nRepeats ) = 0;
	virtual CPlainAutoPtr< IUIEvent > CreateEventFromString( IUIPanel *pCreatingPanel, const char *pszEvent, const char **ppszEventEnd ) = 0;

	// Parses ';' separated events into the vector
	virtual bool CreateEventsFromString( CUtlVector< IUIEvent * > *pVecEvents, IUIPanel *pCreatingPanel, const char *pszEvents ) = 0;

	virtual void RegisterPanelFactoryWithEngine( CPanoramaSymbol symPanelType, CPanel2DFactory *pFactory ) = 0;
	virtual bool BRegisteredPanelType( CPanoramaSymbol symPanelType ) = 0;

	// Turn the engine's debugger state on or off and dispatch the CreateDebuggerWindow or
	// CloseDebuggerWindow event; the window itself is created by whoever handles the event
	virtual void CreateDebuggerWindow() = 0;
	virtual void CloseDebuggerWindow() = 0;

	// Named by what it does: IPanoramaUIClient::SetupUIEngine stores its bAllowDebugger here;
	// panorama itself does not read it
	virtual bool BAllowDebugger() = 0;
	virtual void SetAllowDebugger( bool bAllowDebugger ) = 0;

	virtual int RegisterScheduledDelegate( double flTargetFrameTime, CUtlDelegate< void() > del ) = 0;
	virtual void CancelScheduledDelegate( int iScheduleIndex ) = 0;
	virtual double GetLastScheduledDelegateRunTime() = 0;

	virtual CPanoramaSymbol MakeSymbol( const char *pszText ) = 0;

	// Named by what they do: add, find without adding and resolve a symbol in the panorama symbol
	// table nTable (MakeSymbol adds to the default one)
	virtual uint16 MakeSymbolInTable( int nTable, const char *pszText ) = 0;
	virtual uint16 FindSymbolInTable( int nTable, const char *pszText ) = 0;
	virtual const char *ResolveSymbolInTable( int nTable, uint16 nSymbol ) = 0;

	virtual void QueueDecrementRefNextFrame( IRefCounted *pRefCounted ) = 0;

	virtual int RegisterJSGenericCallback( IUIPanel *pContextPanel, v8::Local< v8::Function > callbackFunc ) = 0;
	virtual bool InvokeJSGenericCallback( int nHandle, int nArgs, v8::Local< v8::Value > *pArgs, v8::Local< v8::Value > *pRetVal ) = 0;
	virtual void UnregisterJSGenericCallback( int nHandle ) = 0;

	virtual int GetNumRegisterJSScopes() = 0;
	virtual void GetRegisterJSScopeInfo( int nScope, RegisterJSScopeInfo_t *pInfo ) = 0;
	virtual void GetRegisterJSEntryInfo( int nScope, int nEntry, RegisterJSEntryInfo_t *pInfo ) = 0;
	virtual int StartRegisterJSScope( const char *pszName, const char *pszDesc ) = 0;
	virtual void EndRegisterJSScope() = 0;
	virtual int NewRegisterJSEntry( const char *pszName, uint32 unFlags, const char *pszDesc, uint8 eDataType ) = 0;
	virtual void SetRegisterJSEntryParams( int nEntry, uint8 unNumParams, uint8 *pParamTypes ) = 0;

	// True when the URL is http(s) and has the domain of the panel's layout URL
	virtual bool BMatchDomainForJSRequest( IUIPanel *pContextPanel, const char *pszURL ) = 0;

	virtual void ClearFileCache() = 0;
	virtual void PrintCacheStatus() = 0;

	virtual void GetWindowsForDebugger( CUtlVector< IUIWindow * > &vecWindows ) = 0;

	virtual void SetPaintCountTrackingEnabled( bool bEnabled ) = 0;
	virtual bool GetPaintCountTrackingEnabled() = 0;
	virtual void IncrementPaintCountForPanel( uint64 ulPanelPtrValue, bool bRequiredCompositionLayer, double flFrameTime ) = 0;
	virtual void GetPanelPaintInfo( uint64 ulPanelPtrValue, uint32 &unMaxPanelPaintCount, uint32 &unPaintCount, bool &bRequiredCompositionLayer, double &flFrameTimeLastPaint ) = 0;

	virtual bool BHasAnyWindows() = 0;

	// Named by what it does: an empty function on both platforms in this build
	virtual void SetUnusedFlag( bool bValue ) = 0;

	// Named by what it does: writes the XML schema (xs:schema) of the registered panel types
	virtual void GetPanelTypesXMLSchema( CBufferString &sSchema ) = 0;

	// Forward the text entry focus to the IME of the panel's window
	virtual void TextEntryFocusChange( IUIPanel *pPanel ) = 0;
	virtual void TextEntryInvalid( IUIPanel *pPanel ) = 0;

	virtual void CaptureJSStackTrace( bool bPrint ) = 0;

	// Named by what it does: disposes every compiled script and script origin the engine caches
	virtual void ClearScriptCache() = 0;

	// Named by what it does: keeps a reference to the object, a video texture, until it has the
	// only reference left; RunFrame updates the textures of the list (CUIEngine::UpdateVideoTextures)
	virtual void AddVideoTexture( IRefCounted *pVideoTexture ) = 0;

	virtual void CopyToClipboardImpl( const char *pszText ) = 0;
	virtual void GetClipboardTextImpl( CUtlString &sText ) = 0;

	virtual void OnFileCacheRemoved( CPanoramaSymbol symPath ) = 0;

	// Named by what it does: forwards to the image manager, which reloads every cached image of
	// the URL ("file://" is prefixed to paths without a scheme)
	virtual void ReloadImage( const char *pszImageURL ) = 0;

	// Runs the input engine and gamepad frame
	virtual void RunPlatformFrame() = 0;

	// Creates the panorama file system; false without the text services
	virtual bool BInitialize() = 0;
};

} // namespace panorama

#endif // IUIENGINE_H
