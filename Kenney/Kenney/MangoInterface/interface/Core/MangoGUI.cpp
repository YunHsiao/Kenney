//--------------------------------------------------------------------------------------
// File: Utility.cpp 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#include "../Mango.h"

#define MANGO_MIN_WINDOW_SIZE_X 200
#define MANGO_MIN_WINDOW_SIZE_Y 200
#undef min // use __min instead inside this source file
#undef max // use __max instead inside this source file

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B // (not always defined)
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C // (not always defined)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A // (not always defined)
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120 // (not always defined)
#endif

//--------------------------------------------------------------------------------------
// 多线程标志
//--------------------------------------------------------------------------------------
CRITICAL_SECTION g_cs;  
bool g_bThreadSafe = true;

class MANGOLock
{
public:
	inline MANGOLock()  { if( g_bThreadSafe ) EnterCriticalSection( &g_cs ); }
	inline ~MANGOLock() { if( g_bThreadSafe ) LeaveCriticalSection( &g_cs ); }
};

//--------------------------------------------------------------------------------------
// 用以批量定义函数的宏
//--------------------------------------------------------------------------------------
#define SET_ACCESSOR( T, name_ )       inline void Set##name_( T t )  { MANGOLock l; m_state.m_##name_ = t; };
#define GET_ACCESSOR( T, name_ )       inline T Get##name_() { MANGOLock l; return m_state.m_##name_; };
#define GET_SET_ACCESSOR( T, name_ )   SET_ACCESSOR( T, name_ ) GET_ACCESSOR( T, name_ )

#define SETP_ACCESSOR( T, name_ )      inline void Set##name_( T* t )  { MANGOLock l; m_state.m_##name_ = *t; };
#define GETP_ACCESSOR( T, name_ )      inline T* Get##name_() { MANGOLock l; return &m_state.m_##name_; };
#define GETP_SETP_ACCESSOR( T, name_ ) SETP_ACCESSOR( T, name_ ) GETP_ACCESSOR( T, name_ )


//--------------------------------------------------------------------------------------
// Stores timer callback info
//--------------------------------------------------------------------------------------
struct MANGO_TIMER
{
	LPMANGOCALLBACKTIMER pCallbackTimer;
	void* pCallbackUserContext;
	float fTimeoutInSecs;
	float fCountdown;
	bool  bEnabled;
	UINT  nID;
};


//--------------------------------------------------------------------------------------
// Stores MANGO state and data access is done with thread safety (if g_bThreadSafe==true)
//--------------------------------------------------------------------------------------
class MANGOState
{
protected:
	struct STATE
	{
		IDirect3D9*          m_D3D;                     // the main D3D object

		IDirect3DDevice9*    m_D3DDevice;               // the D3D rendering device
		CD3DEnumeration*     m_D3DEnumeration;          // CD3DEnumeration object

		MANGODeviceSettings*  m_CurrentDeviceSettings;  // current device settings
		D3DSURFACE_DESC      m_BackBufferSurfaceDesc;   // back buffer surface description
		D3DCAPS9             m_Caps;                    // D3D caps for current device

		HWND				 m_HWNDFocus;               // the main app focus window
		HWND				 m_HWNDDeviceFullScreen;    // the main app device window in fullscreen mode
		HWND				 m_HWNDDeviceWindowed;      // the main app device window in windowed mode
		HMONITOR			 m_AdapterMonitor;          // the monitor of the adapter 
		HMENU				 m_Menu;                    // handle to menu

		UINT m_FullScreenBackBufferWidthAtModeChange;   // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_FullScreenBackBufferHeightAtModeChange;  // back buffer size of fullscreen mode right before switching to windowed mode.  Used to restore to same resolution when toggling back to fullscreen
		UINT m_WindowBackBufferWidthAtModeChange;       // back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		UINT m_WindowBackBufferHeightAtModeChange;		// back buffer size of windowed mode right before switching to fullscreen mode.  Used to restore to same resolution when toggling back to windowed mode
		DWORD m_WindowedStyleAtModeChange;				// window style
		WINDOWPLACEMENT m_WindowedPlacement;			// record of windowed HWND position/show state/etc
		bool  m_TopmostWhileWindowed;		// if true, the windowed HWND is topmost 
		bool  m_Minimized;                  // if true, the HWND is minimized
		bool  m_Maximized;                  // if true, the HWND is maximized
		bool  m_MinimizedWhileFullscreen;   // if true, the HWND is minimized due to a focus switch away when fullscreen mode
		bool  m_IgnoreSizeChange;           // if true, MANGO won't reset the device upon HWND size change

		double m_Time;                      // current time in seconds
		double m_AbsoluteTime;              // absolute time in seconds
		float m_ElapsedTime;                // time elapsed since last frame

		HINSTANCE m_HInstance;              // handle to the app instance
		double m_LastStatsUpdateTime;       // last time the stats were updated
		DWORD m_LastStatsUpdateFrames;      // frames count since last time the stats were updated
		float m_FPS;                        // frames per second
		int   m_CurrentFrameNumber;         // the current frame number
		HHOOK m_KeyboardHook;               // handle to keyboard hook
		bool  m_AllowShortcutKeysWhenFullscreen; // if true, when fullscreen enable shortcut keys (Windows keys, StickyKeys shortcut, ToggleKeys shortcut, FilterKeys shortcut) 
		bool  m_AllowShortcutKeysWhenWindowed;   // if true, when windowed enable shortcut keys (Windows keys, StickyKeys shortcut, ToggleKeys shortcut, FilterKeys shortcut) 
		bool  m_AllowShortcutKeys;          // if true, then shortcut keys are currently disabled (Windows key, etc)
		bool  m_CallDefWindowProc;          // if true, MANGOStaticWndProc will call DefWindowProc for unhandled messages. Applications rendering to a dialog may need to set this to false.
		STICKYKEYS m_StartupStickyKeys;     // StickyKey settings upon startup so they can be restored later
		TOGGLEKEYS m_StartupToggleKeys;     // ToggleKey settings upon startup so they can be restored later
		FILTERKEYS m_StartupFilterKeys;     // FilterKey settings upon startup so they can be restored later

		bool  m_HandleDefaultHotkeys;       // if true, then MANGO will handle some default hotkeys
		bool  m_HandleAltEnter;             // if true, then MANGO will handle Alt-Enter
		bool  m_ShowMsgBoxOnError;          // if true, then msgboxes are displayed upon errors
		bool  m_NoStats;                    // if true, then MANGOGetFrameStats() and MANGOGetDeviceStats() will return blank strings
		bool  m_ClipCursorWhenFullScreen;   // if true, then MANGO will keep the cursor from going outside the window when full screen
		bool  m_ShowCursorWhenFullScreen;   // if true, then MANGO will show a cursor when full screen
		bool  m_ConstantFrameTime;          // if true, then elapsed frame time will always be 0.05f seconds which is good for debugging or automated capture
		float m_TimePerFrame;               // the constant time per frame in seconds, only valid if m_ConstantFrameTime==true
		bool  m_WireframeMode;              // if true, then D3DRS_FILLMODE==D3DFILL_WIREFRAME else D3DRS_FILLMODE==D3DFILL_SOLID 
		bool  m_AutoChangeAdapter;          // if true, then the adapter will automatically change if the window is different monitor
		bool  m_WindowCreatedWithDefaultPositions; // if true, then CW_USEDEFAULT was used and the window should be moved to the right adapter
		int   m_ExitCode;                   // the exit code to be returned to the command line

		bool  m_MANGOInited;                 // if true, then MANGOInit() has succeeded
		bool  m_WindowCreated;              // if true, then MANGOCreateWindow() or MANGOSetWindow() has succeeded
		bool  m_DeviceCreated;              // if true, then MANGOCreateDevice*() or MANGOSetDevice() has succeeded

		bool  m_MANGOInitCalled;             // if true, then MANGOInit() was called
		bool  m_WindowCreateCalled;         // if true, then MANGOCreateWindow() or MANGOSetWindow() was called
		bool  m_DeviceCreateCalled;         // if true, then MANGOCreateDevice*() or MANGOSetDevice() was called

		bool  m_DeviceObjectsCreated;       // if true, then DeviceCreated callback has been called (if non-NULL)
		bool  m_DeviceObjectsReset;         // if true, then DeviceReset callback has been called (if non-NULL)
		bool  m_InsideDeviceCallback;       // if true, then the framework is inside an app device callback
		bool  m_InsideMainloop;             // if true, then the framework is inside the main loop
		bool  m_Active;                     // if true, then the app is the active top level window
		bool  m_TimePaused;                 // if true, then time is paused
		bool  m_RenderingPaused;            // if true, then rendering is paused
		int   m_PauseRenderingCount;        // pause rendering ref count
		int   m_PauseTimeCount;             // pause time ref count
		bool  m_DeviceLost;                 // if true, then the device is lost and needs to be reset
		bool  m_NotifyOnMouseMove;          // if true, include WM_MOUSEMOVE in mousecallback
		bool  m_Automation;                 // if true, automation is enabled
		bool  m_InSizeMove;                 // if true, app is inside a WM_ENTERSIZEMOVE
		UINT  m_TimerLastID;               // last ID of the MANGO timer

		int   m_OverrideAdapterOrdinal;     // if != -1, then override to use this adapter ordinal
		bool  m_OverrideWindowed;           // if true, then force to start windowed
		bool  m_OverrideFullScreen;         // if true, then force to start full screen
		int   m_OverrideStartX;             // if != -1, then override to this X position of the window
		int   m_OverrideStartY;             // if != -1, then override to this Y position of the window
		int   m_OverrideWidth;              // if != 0, then override to this width
		int   m_OverrideHeight;             // if != 0, then override to this height
		bool  m_OverrideForceHAL;           // if true, then force to HAL device (failing if one doesn't exist)
		bool  m_OverrideForceREF;           // if true, then force to REF device (failing if one doesn't exist)
		bool  m_OverrideForcePureHWVP;      // if true, then force to use pure HWVP (failing if device doesn't support it)
		bool  m_OverrideForceHWVP;          // if true, then force to use HWVP (failing if device doesn't support it)
		bool  m_OverrideForceSWVP;          // if true, then force to use SWVP 
		bool  m_OverrideConstantFrameTime;  // if true, then force to constant frame time
		float m_OverrideConstantTimePerFrame; // the constant time per frame in seconds if m_OverrideConstantFrameTime==true
		int   m_OverrideQuitAfterFrame;     // if != 0, then it will force the app to quit after that frame
		int   m_OverrideForceVsync;         // if == 0, then it will force the app to use D3DPRESENT_INTERVAL_IMMEDIATE, if == 1 force use of D3DPRESENT_INTERVAL_DEFAULT
		bool  m_OverrideRelaunchMCE;          // if true, then force relaunch of MCE at exit

		LPMANGOCALLBACKISDEVICEACCEPTABLE    m_IsDeviceAcceptableFunc;   // is device acceptable callback
		LPMANGOCALLBACKMODIFYDEVICESETTINGS  m_ModifyDeviceSettingsFunc; // modify device settings callback
		LPMANGOCALLBACKDEVICECREATED         m_DeviceCreatedFunc;        // device created callback
		LPMANGOCALLBACKDEVICERESET           m_DeviceResetFunc;          // device reset callback
		LPMANGOCALLBACKDEVICELOST            m_DeviceLostFunc;           // device lost callback
		LPMANGOCALLBACKDEVICEDESTROYED       m_DeviceDestroyedFunc;      // device destroyed callback
		LPMANGOCALLBACKFRAMEMOVE             m_FrameMoveFunc;            // frame move callback
		LPMANGOCALLBACKFRAMERENDER           m_FrameRenderFunc;          // frame render callback
		LPMANGOCALLBACKKEYBOARD              m_KeyboardFunc;             // keyboard callback
		LPMANGOCALLBACKMOUSE                 m_MouseFunc;                // mouse callback
		LPMANGOCALLBACKMSGPROC               m_WindowMsgFunc;            // window messages callback

		void*                               m_IsDeviceAcceptableFuncUserContext;   // user context for is device acceptable callback
		void*                               m_ModifyDeviceSettingsFuncUserContext; // user context for modify device settings callback
		void*                               m_DeviceCreatedUserContext;            // user context for device created callback
		void*                               m_DeviceCreatedFuncUserContext;        // user context for device created callback
		void*                               m_DeviceResetFuncUserContext;          // user context for device reset callback
		void*                               m_DeviceLostFuncUserContext;           // user context for device lost callback
		void*                               m_DeviceDestroyedFuncUserContext;      // user context for device destroyed callback
		void*                               m_FrameMoveFuncUserContext;            // user context for frame move callback
		void*                               m_FrameRenderFuncUserContext;          // user context for frame render callback
		void*                               m_KeyboardFuncUserContext;             // user context for keyboard callback
		void*                               m_MouseFuncUserContext;                // user context for mouse callback
		void*                               m_WindowMsgFuncUserContext;            // user context for window messages callback

		bool								m_Keys[256];                       // array of key state
		bool								m_MouseButtons[5];                 // array of mouse states

		CMangoArray<MANGO_TIMER>*			m_TimerList;                       // list of MANGO_TIMER structs
		WCHAR								m_StaticFrameStats[256];           // static part of frames stats 
		WCHAR								m_FPSStats[64];                    // fps stats
		WCHAR								m_FrameStats[256];                 // frame stats (fps, width, etc)
		WCHAR								m_DeviceStats[256];                // device stats (description, device type, etc)
		WCHAR								m_WindowTitle[256];                // window title
	};

	STATE m_state;

public:
	MANGOState()  { Create(); }
	~MANGOState() { Destroy(); }

	void Create()
	{
		// Make sure these are created before MANGOState so they 
		// destroyed last because MANGOState cleanup needs them
		MANGOGetGlobalResourceCache();

		ZeroMemory( &m_state, sizeof(STATE) ); 
		g_bThreadSafe = true; 
		InitializeCriticalSection( &g_cs ); 
		m_state.m_OverrideStartX = -1; 
		m_state.m_OverrideStartY = -1; 
		m_state.m_OverrideAdapterOrdinal = -1; 
		m_state.m_OverrideForceVsync = -1;
		m_state.m_AutoChangeAdapter = true; 
		m_state.m_ShowMsgBoxOnError = true;
		m_state.m_AllowShortcutKeysWhenWindowed = true;
		m_state.m_Active = true;
		m_state.m_CallDefWindowProc = true;
	}

	void Destroy()
	{
		MANGOShutdown();
		DeleteCriticalSection( &g_cs ); 
	}

	// Macros to define access functions for thread safe access into m_state 
	GET_SET_ACCESSOR( IDirect3D9*, D3D );

	GET_SET_ACCESSOR( IDirect3DDevice9*, D3DDevice );
	GET_SET_ACCESSOR( CD3DEnumeration*, D3DEnumeration );   
	GET_SET_ACCESSOR( MANGODeviceSettings*, CurrentDeviceSettings );   
	GETP_SETP_ACCESSOR( D3DSURFACE_DESC, BackBufferSurfaceDesc );
	GETP_SETP_ACCESSOR( D3DCAPS9, Caps );

	GET_SET_ACCESSOR( HWND, HWNDFocus );
	GET_SET_ACCESSOR( HWND, HWNDDeviceFullScreen );
	GET_SET_ACCESSOR( HWND, HWNDDeviceWindowed );
	GET_SET_ACCESSOR( HMONITOR, AdapterMonitor );
	GET_SET_ACCESSOR( HMENU, Menu );   

	GET_SET_ACCESSOR( UINT, FullScreenBackBufferWidthAtModeChange );
	GET_SET_ACCESSOR( UINT, FullScreenBackBufferHeightAtModeChange );
	GET_SET_ACCESSOR( UINT, WindowBackBufferWidthAtModeChange );
	GET_SET_ACCESSOR( UINT, WindowBackBufferHeightAtModeChange );
	GETP_SETP_ACCESSOR( WINDOWPLACEMENT, WindowedPlacement );
	GET_SET_ACCESSOR( DWORD, WindowedStyleAtModeChange );
	GET_SET_ACCESSOR( bool, TopmostWhileWindowed );
	GET_SET_ACCESSOR( bool, Minimized );
	GET_SET_ACCESSOR( bool, Maximized );
	GET_SET_ACCESSOR( bool, MinimizedWhileFullscreen );
	GET_SET_ACCESSOR( bool, IgnoreSizeChange );   

	GET_SET_ACCESSOR( double, Time );
	GET_SET_ACCESSOR( double, AbsoluteTime );
	GET_SET_ACCESSOR( float, ElapsedTime );

	GET_SET_ACCESSOR( HINSTANCE, HInstance );
	GET_SET_ACCESSOR( double, LastStatsUpdateTime );   
	GET_SET_ACCESSOR( DWORD, LastStatsUpdateFrames );   
	GET_SET_ACCESSOR( float, FPS );    
	GET_SET_ACCESSOR( int, CurrentFrameNumber );
	GET_SET_ACCESSOR( HHOOK, KeyboardHook );
	GET_SET_ACCESSOR( bool, AllowShortcutKeysWhenFullscreen );
	GET_SET_ACCESSOR( bool, AllowShortcutKeysWhenWindowed );
	GET_SET_ACCESSOR( bool, AllowShortcutKeys );
	GET_SET_ACCESSOR( bool, CallDefWindowProc );
	GET_SET_ACCESSOR( STICKYKEYS, StartupStickyKeys );
	GET_SET_ACCESSOR( TOGGLEKEYS, StartupToggleKeys );
	GET_SET_ACCESSOR( FILTERKEYS, StartupFilterKeys );

	GET_SET_ACCESSOR( bool, HandleDefaultHotkeys );
	GET_SET_ACCESSOR( bool, HandleAltEnter );
	GET_SET_ACCESSOR( bool, ShowMsgBoxOnError );
	GET_SET_ACCESSOR( bool, NoStats );
	GET_SET_ACCESSOR( bool, ClipCursorWhenFullScreen );   
	GET_SET_ACCESSOR( bool, ShowCursorWhenFullScreen );
	GET_SET_ACCESSOR( bool, ConstantFrameTime );
	GET_SET_ACCESSOR( float, TimePerFrame );
	GET_SET_ACCESSOR( bool, WireframeMode );   
	GET_SET_ACCESSOR( bool, AutoChangeAdapter );
	GET_SET_ACCESSOR( bool, WindowCreatedWithDefaultPositions );
	GET_SET_ACCESSOR( int, ExitCode );

	GET_SET_ACCESSOR( bool, MANGOInited );
	GET_SET_ACCESSOR( bool, WindowCreated );
	GET_SET_ACCESSOR( bool, DeviceCreated );
	GET_SET_ACCESSOR( bool, MANGOInitCalled );
	GET_SET_ACCESSOR( bool, WindowCreateCalled );
	GET_SET_ACCESSOR( bool, DeviceCreateCalled );
	GET_SET_ACCESSOR( bool, InsideDeviceCallback );
	GET_SET_ACCESSOR( bool, InsideMainloop );
	GET_SET_ACCESSOR( bool, DeviceObjectsCreated );
	GET_SET_ACCESSOR( bool, DeviceObjectsReset );
	GET_SET_ACCESSOR( bool, Active );
	GET_SET_ACCESSOR( bool, RenderingPaused );
	GET_SET_ACCESSOR( bool, TimePaused );
	GET_SET_ACCESSOR( int, PauseRenderingCount );
	GET_SET_ACCESSOR( int, PauseTimeCount );
	GET_SET_ACCESSOR( bool, DeviceLost );
	GET_SET_ACCESSOR( bool, NotifyOnMouseMove );
	GET_SET_ACCESSOR( bool, Automation );
	GET_SET_ACCESSOR( bool, InSizeMove );
	GET_SET_ACCESSOR( UINT, TimerLastID );

	GET_SET_ACCESSOR( int, OverrideAdapterOrdinal );
	GET_SET_ACCESSOR( bool, OverrideWindowed );
	GET_SET_ACCESSOR( bool, OverrideFullScreen );
	GET_SET_ACCESSOR( int, OverrideStartX );
	GET_SET_ACCESSOR( int, OverrideStartY );
	GET_SET_ACCESSOR( int, OverrideWidth );
	GET_SET_ACCESSOR( int, OverrideHeight );
	GET_SET_ACCESSOR( bool, OverrideForceHAL );
	GET_SET_ACCESSOR( bool, OverrideForceREF );
	GET_SET_ACCESSOR( bool, OverrideForcePureHWVP );
	GET_SET_ACCESSOR( bool, OverrideForceHWVP );
	GET_SET_ACCESSOR( bool, OverrideForceSWVP );
	GET_SET_ACCESSOR( bool, OverrideConstantFrameTime );
	GET_SET_ACCESSOR( float, OverrideConstantTimePerFrame );
	GET_SET_ACCESSOR( int, OverrideQuitAfterFrame );
	GET_SET_ACCESSOR( int, OverrideForceVsync );
	GET_SET_ACCESSOR( bool, OverrideRelaunchMCE );

	GET_SET_ACCESSOR( LPMANGOCALLBACKISDEVICEACCEPTABLE, IsDeviceAcceptableFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKMODIFYDEVICESETTINGS, ModifyDeviceSettingsFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKDEVICECREATED, DeviceCreatedFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKDEVICERESET, DeviceResetFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKDEVICELOST, DeviceLostFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKDEVICEDESTROYED, DeviceDestroyedFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKFRAMEMOVE, FrameMoveFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKFRAMERENDER, FrameRenderFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKKEYBOARD, KeyboardFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKMOUSE, MouseFunc );
	GET_SET_ACCESSOR( LPMANGOCALLBACKMSGPROC, WindowMsgFunc );

	GET_SET_ACCESSOR( void*, IsDeviceAcceptableFuncUserContext );
	GET_SET_ACCESSOR( void*, ModifyDeviceSettingsFuncUserContext );
	GET_SET_ACCESSOR( void*, DeviceCreatedFuncUserContext );
	GET_SET_ACCESSOR( void*, DeviceResetFuncUserContext );
	GET_SET_ACCESSOR( void*, DeviceLostFuncUserContext );
	GET_SET_ACCESSOR( void*, DeviceDestroyedFuncUserContext );
	GET_SET_ACCESSOR( void*, FrameMoveFuncUserContext );
	GET_SET_ACCESSOR( void*, FrameRenderFuncUserContext );
	GET_SET_ACCESSOR( void*, KeyboardFuncUserContext );
	GET_SET_ACCESSOR( void*, MouseFuncUserContext );
	GET_SET_ACCESSOR( void*, WindowMsgFuncUserContext );

	GET_SET_ACCESSOR( CMangoArray<MANGO_TIMER>*, TimerList );   
	GET_ACCESSOR( bool*, Keys );
	GET_ACCESSOR( bool*, MouseButtons );
	GET_ACCESSOR( WCHAR*, StaticFrameStats );
	GET_ACCESSOR( WCHAR*, FPSStats );
	GET_ACCESSOR( WCHAR*, FrameStats );
	GET_ACCESSOR( WCHAR*, DeviceStats );    
	GET_ACCESSOR( WCHAR*, WindowTitle );
};


//--------------------------------------------------------------------------------------
// Global state class
//--------------------------------------------------------------------------------------
MANGOState& GetMANGOState()
{
	// Using an accessor function gives control of the construction order
	static MANGOState state;
	return state;
}


//--------------------------------------------------------------------------------------
// Internal functions forward declarations
//--------------------------------------------------------------------------------------
typedef IDirect3D9* (WINAPI* LPDIRECT3DCREATE9)(UINT SDKVersion);
typedef DECLSPEC_IMPORT UINT (WINAPI* LPTIMEBEGINPERIOD)( UINT uPeriod );
int     MANGOMapButtonToArrayIndex( BYTE vButton );
void    MANGOSetProcessorAffinity();
void    MANGOParseCommandLine();
CD3DEnumeration* MANGOPrepareEnumerationObject( bool bEnumerate = false );
void    MANGOBuildOptimalDeviceSettings( MANGODeviceSettings* pOptimalDeviceSettings, MANGODeviceSettings* pDeviceSettingsIn, MANGOMatchOptions* pMatchOptions );
bool    MANGODoesDeviceComboMatchPreserveOptions( CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, MANGODeviceSettings* pDeviceSettingsIn, MANGOMatchOptions* pMatchOptions );
float   MANGORankDeviceCombo( CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, MANGODeviceSettings* pDeviceSettingsIn, D3DDISPLAYMODE* pAdapterDesktopDisplayMode );
void    MANGOBuildValidDeviceSettings( MANGODeviceSettings* pDeviceSettings, CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, MANGODeviceSettings* pDeviceSettingsIn, MANGOMatchOptions* pMatchOptions );
HRESULT MANGOFindValidResolution( CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, D3DDISPLAYMODE displayModeIn, D3DDISPLAYMODE* pBestDisplayMode );
HRESULT MANGOFindAdapterFormat( UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT BackBufferFormat, BOOL Windowed, D3DFORMAT* pAdapterFormat );
HRESULT MANGOChangeDevice( MANGODeviceSettings* pNewDeviceSettings, IDirect3DDevice9* pd3dDeviceFromApp, bool bForceRecreate, bool bClipWindowToSingleAdapter );
void    MANGOUpdateDeviceSettingsWithOverrides( MANGODeviceSettings* pNewDeviceSettings );
HRESULT MANGOCreate3DEnvironment( IDirect3DDevice9* pd3dDeviceFromApp );
HRESULT MANGOReset3DEnvironment();
void    MANGORender3DEnvironment();
void    MANGOCleanup3DEnvironment( bool bReleaseSettings = true );
void    MANGOUpdateFrameStats();
void    MANGOUpdateDeviceStats( D3DDEVTYPE DeviceType, DWORD BehaviorFlags, D3DADAPTER_IDENTIFIER9* pAdapterIdentifier );
void    MANGOUpdateStaticFrameStats();
void    MANGOHandleTimers();
bool    MANGOIsNextArg( WCHAR*& strCmdLine, WCHAR* strArg );
bool    MANGOGetCmdParam( WCHAR*& strCmdLine, WCHAR* strFlag );
void    MANGODisplayErrorMessage( HRESULT hr );
LRESULT CALLBACK MANGOStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void    MANGOCheckForWindowSizeChange();
void    MANGOCheckForWindowChangingMonitors();
UINT    MANGOColorChannelBits( D3DFORMAT fmt );
UINT    MANGOStencilBits( D3DFORMAT fmt );
UINT    MANGODepthBits( D3DFORMAT fmt );
HRESULT MANGOGetAdapterOrdinalFromMonitor( HMONITOR hMonitor, UINT* pAdapterOrdinal );
void    MANGOAllowShortcutKeys( bool bAllowKeys );
void    MANGOUpdateBackBufferDesc();
void    MANGOSetupCursor();
HRESULT MANGOSetDeviceCursor( IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark );


//--------------------------------------------------------------------------------------
// External callback setup functions
//--------------------------------------------------------------------------------------
void MANGOSetDeviceCreated( LPMANGOCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext ) { GetMANGOState().SetDeviceCreatedFunc( pCallbackDeviceCreated ); GetMANGOState().SetDeviceCreatedFuncUserContext( pUserContext ); }
void MANGOSetDeviceReset( LPMANGOCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext )       { GetMANGOState().SetDeviceResetFunc( pCallbackDeviceReset );  GetMANGOState().SetDeviceResetFuncUserContext( pUserContext ); }
void MANGOSetDeviceLost( LPMANGOCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext )          { GetMANGOState().SetDeviceLostFunc( pCallbackDeviceLost );  GetMANGOState().SetDeviceLostFuncUserContext( pUserContext ); }
void MANGOSetDeviceDestroyed( LPMANGOCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext ) { GetMANGOState().SetDeviceDestroyedFunc( pCallbackDeviceDestroyed );  GetMANGOState().SetDeviceDestroyedFuncUserContext( pUserContext ); }
void MANGOSetDeviceChanging( LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext ) { GetMANGOState().SetModifyDeviceSettingsFunc( pCallbackModifyDeviceSettings );  GetMANGOState().SetModifyDeviceSettingsFuncUserContext( pUserContext ); }
void MANGOSetFrameMove( LPMANGOCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext ) { GetMANGOState().SetFrameMoveFunc( pCallbackFrameMove );  GetMANGOState().SetFrameMoveFuncUserContext( pUserContext ); }
void MANGOSetFrameRender( LPMANGOCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext )       { GetMANGOState().SetFrameRenderFunc( pCallbackFrameRender );  GetMANGOState().SetFrameRenderFuncUserContext( pUserContext ); }
void MANGOSetKeyboard( LPMANGOCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext )                { GetMANGOState().SetKeyboardFunc( pCallbackKeyboard );  GetMANGOState().SetKeyboardFuncUserContext( pUserContext ); }
void MANGOSetMouse( LPMANGOCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove, void* pUserContext ) { GetMANGOState().SetMouseFunc( pCallbackMouse ); GetMANGOState().SetNotifyOnMouseMove( bIncludeMouseMove );  GetMANGOState().SetMouseFuncUserContext( pUserContext ); }
void MANGOSetMsgProc( LPMANGOCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext )                   { GetMANGOState().SetWindowMsgFunc( pCallbackMsgProc );  GetMANGOState().SetWindowMsgFuncUserContext( pUserContext ); }

//======================================================================================
// new class main for App
//
//======================================================================================

CMANGOXApp::CMANGOXApp(CMANGOXApp * v)
{
	//MANGOSetDeviceCreated(v->OnCreateDevice);
}

HRESULT CMANGOXApp::MANGOInit( bool bParseCommandLine, bool bHandleDefaultHotkeys, bool bShowMsgBoxOnError, bool bHandleAltEnter )
{
	GetMANGOState().SetMANGOInitCalled( true );

	// Not always needed, but lets the app create GDI dialogs
	// InitCommonControls();

	// Save the current sticky/toggle/filter key settings so MANGO can restore them later
	STICKYKEYS sk = {sizeof(STICKYKEYS), 0};
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
	GetMANGOState().SetStartupStickyKeys( sk );

	TOGGLEKEYS tk = {sizeof(TOGGLEKEYS), 0};
	SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tk, 0);
	GetMANGOState().SetStartupToggleKeys( tk );

	FILTERKEYS fk = {sizeof(FILTERKEYS), 0};
	SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);
	GetMANGOState().SetStartupFilterKeys( fk );

	// Increase the accuracy of Sleep() without needing to link to winmm.lib
	WCHAR wszPath[MAX_PATH+1];
	if( GetSystemDirectory( wszPath, MAX_PATH+1 ) )
	{
		StringCchCat( wszPath, MAX_PATH, L"\\winmm.dll" );
		HINSTANCE hInstWinMM = LoadLibrary( wszPath );
		if( hInstWinMM ) 
		{
			LPTIMEBEGINPERIOD pTimeBeginPeriod = (LPTIMEBEGINPERIOD)GetProcAddress( hInstWinMM, "timeBeginPeriod" );
			if( NULL != pTimeBeginPeriod )
				pTimeBeginPeriod(1);

			FreeLibrary(hInstWinMM);
		}
	}

	GetMANGOState().SetShowMsgBoxOnError( bShowMsgBoxOnError );
	GetMANGOState().SetHandleDefaultHotkeys( bHandleDefaultHotkeys );
	GetMANGOState().SetHandleAltEnter( bHandleAltEnter );

	if( bParseCommandLine )
		MANGOParseCommandLine();

	// Verify D3DX version
	if( !D3DXCheckVersion( D3D_SDK_VERSION, D3DX_SDK_VERSION ) )
	{
		MANGODisplayErrorMessage( MANGOERR_INCORRECTVERSION );
		return MANGO_ERR( L"D3DXCheckVersion", MANGOERR_INCORRECTVERSION );
	}

	// Create a Direct3D object if one has not already been created
	IDirect3D9* pD3D = MANGOGetD3DObject();
	if( pD3D == NULL )
	{
		// This may fail if DirectX 9 isn't installed
		// This may fail if the DirectX headers are out of sync with the installed DirectX DLLs
		pD3D = MANGO_Dynamic_Direct3DCreate9( D3D_SDK_VERSION );
		GetMANGOState().SetD3D( pD3D );
	}

	if( pD3D == NULL )
	{
		// If still NULL, then something went wrong
		MANGODisplayErrorMessage( MANGOERR_NODIRECT3D );
		return MANGO_ERR( L"Direct3DCreate9", MANGOERR_NODIRECT3D );
	}

	// Reset the timer
	MANGOGetGlobalTimer()->Reset();

	GetMANGOState().SetMANGOInited( true );

	return S_OK;
}
//======================================================================================
//CreateWindow
//======================================================================================
HRESULT CMANGOXApp::MANGOCreateWindow( const WCHAR* strWindowTitle, HINSTANCE hInstance, 
						  HICON hIcon, HMENU hMenu, int x, int y )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	GetMANGOState().SetWindowCreateCalled( true );

	if( !GetMANGOState().GetMANGOInited() ) 
	{
		// If MANGOInit() was already called and failed, then fail.
		// MANGOInit() must first succeed for this function to succeed
		if( GetMANGOState().GetMANGOInitCalled() )
			return E_FAIL; 

		// If MANGOInit() hasn't been called, then automatically call it
		// with default params
		hr = MANGOInit();
		if( FAILED(hr) )
			return hr;
	}

	if( MANGOGetHWNDFocus() == NULL )
	{
		if( hInstance == NULL ) 
			hInstance = (HINSTANCE)GetModuleHandle(NULL);
		GetMANGOState().SetHInstance( hInstance );

		WCHAR szExePath[MAX_PATH];
		GetModuleFileName( NULL, szExePath, MAX_PATH );
		if( hIcon == NULL ) // If the icon is NULL, then use the first one found in the exe
			hIcon = ExtractIcon( hInstance, szExePath, 0 ); 

		// Register the windows class
		WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS;
		wndClass.lpfnWndProc = ::MANGOStaticWndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = hIcon;
		wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = L"Direct3DWindowClass";

		if( !RegisterClass( &wndClass ) )
		{
			DWORD dwError = GetLastError();
			if( dwError != ERROR_CLASS_ALREADY_EXISTS )
				return MANGO_ERR_MSGBOX( L"RegisterClass", HRESULT_FROM_WIN32(dwError) );
		}

		RECT rc;
		// Override the window's initial & size position if there were cmd line args
		if( GetMANGOState().GetOverrideStartX() != -1 )
			x = GetMANGOState().GetOverrideStartX();
		if( GetMANGOState().GetOverrideStartY() != -1 )
			y = GetMANGOState().GetOverrideStartY();

		GetMANGOState().SetWindowCreatedWithDefaultPositions( false );
		if( x == CW_USEDEFAULT && y == CW_USEDEFAULT )
			GetMANGOState().SetWindowCreatedWithDefaultPositions( true );

		// Find the window's initial size, but it might be changed later
		int nDefaultWidth = 640;
		int nDefaultHeight = 480;
		if( GetMANGOState().GetOverrideWidth() != 0 )
			nDefaultWidth = GetMANGOState().GetOverrideWidth();
		if( GetMANGOState().GetOverrideHeight() != 0 )
			nDefaultHeight = GetMANGOState().GetOverrideHeight();
		SetRect( &rc, 0, 0, nDefaultWidth, nDefaultHeight );        
		AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, ( hMenu != NULL ) ? true : false );

		WCHAR* strCachedWindowTitle = GetMANGOState().GetWindowTitle();
		StringCchCopy( strCachedWindowTitle, 256, strWindowTitle );

		// Create the render window
		HWND hWnd = CreateWindow( L"Direct3DWindowClass", strWindowTitle, WS_OVERLAPPEDWINDOW,
			x, y, (rc.right-rc.left), (rc.bottom-rc.top), 0,
			hMenu, hInstance, 0 );
		if( hWnd == NULL )
		{
			DWORD dwError = GetLastError();
			return MANGO_ERR_MSGBOX( L"CreateWindow", HRESULT_FROM_WIN32(dwError) );
		}

		GetMANGOState().SetWindowCreated( true );
		GetMANGOState().SetHWNDFocus( hWnd );
		GetMANGOState().SetHWNDDeviceFullScreen( hWnd );
		GetMANGOState().SetHWNDDeviceWindowed( hWnd );
	}

	return S_OK;
}

//======================================================================================
//MANGOSetWindow
//======================================================================================
HRESULT CMANGOXApp::MANGOSetWindow( HWND hWndFocus, HWND hWndDeviceFullScreen, HWND hWndDeviceWindowed, bool bHandleMessages )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	GetMANGOState().SetWindowCreateCalled( true );

	// To avoid confusion, we do not allow any HWND to be NULL here.  The
	// caller must pass in valid HWND for all three parameters.  The same
	// HWND may be used for more than one parameter.
	if( hWndFocus == NULL || hWndDeviceFullScreen == NULL || hWndDeviceWindowed == NULL )
		return MANGO_ERR_MSGBOX( L"MANGOSetWindow", E_INVALIDARG );

	// If subclassing the window, set the pointer to the local window procedure
	if( bHandleMessages )
	{
		// Switch window procedures
#ifdef _WIN64
		LONG_PTR nResult = SetWindowLongPtr( hWndFocus, GWLP_WNDPROC, (LONG_PTR)MANGOStaticWndProc );
#else
		LONG_PTR nResult = SetWindowLongPtr( hWndFocus, GWLP_WNDPROC, (LONG)(LONG_PTR)MANGOStaticWndProc );
#endif 

		DWORD dwError = GetLastError();
		if( nResult == 0 )
			return MANGO_ERR_MSGBOX( L"SetWindowLongPtr", HRESULT_FROM_WIN32(dwError) );
	}

	if( !GetMANGOState().GetMANGOInited() ) 
	{
		// If MANGOInit() was already called and failed, then fail.
		// MANGOInit() must first succeed for this function to succeed
		if( GetMANGOState().GetMANGOInitCalled() )
			return E_FAIL; 

		// If MANGOInit() hasn't been called, then automatically call it
		// with default params
		hr = MANGOInit();
		if( FAILED(hr) )
			return hr;
	}

	WCHAR* strCachedWindowTitle = GetMANGOState().GetWindowTitle();
	GetWindowText( hWndFocus, strCachedWindowTitle, 255 );
	strCachedWindowTitle[255] = 0;

	HINSTANCE hInstance = (HINSTANCE) (LONG_PTR) GetWindowLongPtr( hWndFocus, GWLP_HINSTANCE ); 
	GetMANGOState().SetHInstance( hInstance );
	GetMANGOState().SetWindowCreatedWithDefaultPositions( false );
	GetMANGOState().SetWindowCreated( true );
	GetMANGOState().SetHWNDFocus( hWndFocus );
	GetMANGOState().SetHWNDDeviceFullScreen( hWndDeviceFullScreen );
	GetMANGOState().SetHWNDDeviceWindowed( hWndDeviceWindowed );

	return S_OK;
}

//======================================================================================
//MANGOCreateDevice
//======================================================================================

HRESULT CMANGOXApp::MANGOCreateDevice( UINT AdapterOrdinal, bool bWindowed, 
						  int nSuggestedWidth, int nSuggestedHeight,
						  LPMANGOCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable,
						  LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings,
						  void* pUserContext )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	// Record the function arguments in the global state 
	GetMANGOState().SetIsDeviceAcceptableFunc( pCallbackIsDeviceAcceptable );
	GetMANGOState().SetModifyDeviceSettingsFunc( pCallbackModifyDeviceSettings );
	GetMANGOState().SetIsDeviceAcceptableFuncUserContext( pUserContext );
	GetMANGOState().SetModifyDeviceSettingsFuncUserContext( pUserContext );

	GetMANGOState().SetDeviceCreateCalled( true );

	// If MANGOCreateWindow() or MANGOSetWindow() has not already been called, 
	// then call MANGOCreateWindow() with the default parameters.         
	if( !GetMANGOState().GetWindowCreated() ) 
	{
		// If MANGOCreateWindow() or MANGOSetWindow() was already called and failed, then fail.
		// MANGOCreateWindow() or MANGOSetWindow() must first succeed for this function to succeed
		if( GetMANGOState().GetWindowCreateCalled() )
			return E_FAIL; 

		// If MANGOCreateWindow() or MANGOSetWindow() hasn't been called, then 
		// automatically call MANGOCreateWindow() with default params
		hr = MANGOCreateWindow();
		if( FAILED(hr) )
			return hr;
	}

	// Force an enumeration with the new IsDeviceAcceptable callback
	MANGOPrepareEnumerationObject( true );

	MANGOMatchOptions matchOptions;
	matchOptions.eAdapterOrdinal     = MANGOMT_PRESERVE_INPUT;
	matchOptions.eDeviceType         = MANGOMT_IGNORE_INPUT;
	matchOptions.eWindowed           = MANGOMT_PRESERVE_INPUT;
	matchOptions.eAdapterFormat      = MANGOMT_IGNORE_INPUT;
	matchOptions.eVertexProcessing   = MANGOMT_IGNORE_INPUT;
	if( bWindowed || (nSuggestedWidth != 0 && nSuggestedHeight != 0) )
		matchOptions.eResolution     = MANGOMT_CLOSEST_TO_INPUT;
	else
		matchOptions.eResolution     = MANGOMT_IGNORE_INPUT;
	matchOptions.eBackBufferFormat   = MANGOMT_IGNORE_INPUT;
	matchOptions.eBackBufferCount    = MANGOMT_IGNORE_INPUT;
	matchOptions.eMultiSample        = MANGOMT_IGNORE_INPUT;
	matchOptions.eSwapEffect         = MANGOMT_IGNORE_INPUT;
	matchOptions.eDepthFormat        = MANGOMT_IGNORE_INPUT;
	matchOptions.eStencilFormat      = MANGOMT_IGNORE_INPUT;
	matchOptions.ePresentFlags       = MANGOMT_IGNORE_INPUT;
	matchOptions.eRefreshRate        = MANGOMT_IGNORE_INPUT;
	matchOptions.ePresentInterval    = MANGOMT_IGNORE_INPUT;

	MANGODeviceSettings deviceSettings;
	ZeroMemory( &deviceSettings, sizeof(MANGODeviceSettings) );
	deviceSettings.AdapterOrdinal      = AdapterOrdinal;
	deviceSettings.pp.Windowed         = bWindowed;
	deviceSettings.pp.BackBufferWidth  = nSuggestedWidth;
	deviceSettings.pp.BackBufferHeight = nSuggestedHeight;

	// Override with settings from the command line
	if( GetMANGOState().GetOverrideWidth() != 0 )
		deviceSettings.pp.BackBufferWidth = GetMANGOState().GetOverrideWidth();
	if( GetMANGOState().GetOverrideHeight() != 0 )
		deviceSettings.pp.BackBufferHeight = GetMANGOState().GetOverrideHeight();

	if( GetMANGOState().GetOverrideAdapterOrdinal() != -1 )
		deviceSettings.AdapterOrdinal = GetMANGOState().GetOverrideAdapterOrdinal();

	if( GetMANGOState().GetOverrideFullScreen() )
	{
		deviceSettings.pp.Windowed = FALSE;
		if( GetMANGOState().GetOverrideWidth() == 0 && GetMANGOState().GetOverrideHeight() == 0 )
			matchOptions.eResolution = MANGOMT_IGNORE_INPUT;
	}
	if( GetMANGOState().GetOverrideWindowed() )
		deviceSettings.pp.Windowed = TRUE;

	if( GetMANGOState().GetOverrideForceHAL() )
	{
		deviceSettings.DeviceType = D3DDEVTYPE_HAL;
		matchOptions.eDeviceType = MANGOMT_PRESERVE_INPUT;
	}
	if( GetMANGOState().GetOverrideForceREF() )
	{
		deviceSettings.DeviceType = D3DDEVTYPE_REF;
		matchOptions.eDeviceType = MANGOMT_PRESERVE_INPUT;
	}

	if( GetMANGOState().GetOverrideForcePureHWVP() )
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
		matchOptions.eVertexProcessing = MANGOMT_PRESERVE_INPUT;
	}
	else if( GetMANGOState().GetOverrideForceHWVP() )
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		matchOptions.eVertexProcessing = MANGOMT_PRESERVE_INPUT;
	}
	else if( GetMANGOState().GetOverrideForceSWVP() )
	{
		deviceSettings.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		matchOptions.eVertexProcessing = MANGOMT_PRESERVE_INPUT;
	}

	if( GetMANGOState().GetOverrideForceVsync() == 0 )
	{
		deviceSettings.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		matchOptions.ePresentInterval = MANGOMT_PRESERVE_INPUT;
	}
	else if( GetMANGOState().GetOverrideForceVsync() == 1 )
	{
		deviceSettings.pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		matchOptions.ePresentInterval = MANGOMT_PRESERVE_INPUT;
	}

	hr = MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );
	if( FAILED(hr) ) // the call will fail if no valid devices were found
	{
		MANGODisplayErrorMessage( hr );
		return MANGO_ERR( L"MANGOFindValidDeviceSettings", hr );
	}

	// Change to a Direct3D device created from the new device settings.  
	// If there is an existing device, then either reset or recreated the scene
	hr = MANGOChangeDevice( &deviceSettings, NULL, false, true );
	if( FAILED(hr) )
		return hr;

	return S_OK;
}

//======================================================================================
//MANGOSetDevice
//======================================================================================

HRESULT CMANGOXApp::MANGOSetDevice( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr;

	if( pd3dDevice == NULL )
		return MANGO_ERR_MSGBOX( L"MANGOSetDevice", E_INVALIDARG );

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	GetMANGOState().SetDeviceCreateCalled( true );

	// If MANGOCreateWindow() or MANGOSetWindow() has not already been called, 
	// then call MANGOCreateWindow() with the default parameters.         
	if( !GetMANGOState().GetWindowCreated() ) 
	{
		// If MANGOCreateWindow() or MANGOSetWindow() was already called and failed, then fail.
		// MANGOCreateWindow() or MANGOSetWindow() must first succeed for this function to succeed
		if( GetMANGOState().GetWindowCreateCalled() )
			return E_FAIL; 

		// If MANGOCreateWindow() or MANGOSetWindow() hasn't been called, then 
		// automatically call MANGOCreateWindow() with default params
		hr = MANGOCreateWindow();
		if( FAILED(hr) )
			return hr;
	}

	MANGODeviceSettings* pDeviceSettings = new MANGODeviceSettings;
	if( pDeviceSettings == NULL )
		return E_OUTOFMEMORY;
	ZeroMemory( pDeviceSettings, sizeof(MANGODeviceSettings) );

	// Get the present params from the swap chain
	IDirect3DSurface9* pBackBuffer = NULL;
	hr = pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	if( SUCCEEDED(hr) )
	{
		IDirect3DSwapChain9* pSwapChain = NULL;
		hr = pBackBuffer->GetContainer( IID_IDirect3DSwapChain9, (void**) &pSwapChain );
		if( SUCCEEDED(hr) )
		{
			pSwapChain->GetPresentParameters( &pDeviceSettings->pp );
			SAFE_RELEASE( pSwapChain );
		}

		SAFE_RELEASE( pBackBuffer );
	}

	D3DDEVICE_CREATION_PARAMETERS d3dCreationParams;
	pd3dDevice->GetCreationParameters( &d3dCreationParams );

	// Fill out the rest of the device settings struct
	pDeviceSettings->AdapterOrdinal = d3dCreationParams.AdapterOrdinal;
	pDeviceSettings->DeviceType     = d3dCreationParams.DeviceType;
	MANGOFindAdapterFormat( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, 
		pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed, 
		&pDeviceSettings->AdapterFormat );
	pDeviceSettings->BehaviorFlags  = d3dCreationParams.BehaviorFlags;

	// Change to the Direct3D device passed in
	hr = MANGOChangeDevice( pDeviceSettings, pd3dDevice, false, false );

	delete pDeviceSettings;

	if( FAILED(hr) ) 
		return hr;

	return S_OK;
}

//======================================================================================
//MANGOCreateDeviceFromSettings
//======================================================================================

HRESULT CMANGOXApp::MANGOCreateDeviceFromSettings( MANGODeviceSettings* pDeviceSettings, bool bPreserveInput, bool bClipWindowToSingleAdapter )
{
	HRESULT hr;

	GetMANGOState().SetDeviceCreateCalled( true );

	// If MANGOCreateWindow() or MANGOSetWindow() has not already been called, 
	// then call MANGOCreateWindow() with the default parameters.         
	if( !GetMANGOState().GetWindowCreated() ) 
	{
		// If MANGOCreateWindow() or MANGOSetWindow() was already called and failed, then fail.
		// MANGOCreateWindow() or MANGOSetWindow() must first succeed for this function to succeed
		if( GetMANGOState().GetWindowCreateCalled() )
			return E_FAIL; 

		// If MANGOCreateWindow() or MANGOSetWindow() hasn't been called, then 
		// automatically call MANGOCreateWindow() with default params
		hr = MANGOCreateWindow();
		if( FAILED(hr) )
			return hr;
	}

	if( !bPreserveInput )
	{
		// If not preserving the input, then find the closest valid to it
		MANGOMatchOptions matchOptions;
		matchOptions.eAdapterOrdinal     = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eDeviceType         = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eWindowed           = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eAdapterFormat      = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eVertexProcessing   = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eResolution         = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eBackBufferFormat   = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eBackBufferCount    = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eMultiSample        = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eSwapEffect         = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eDepthFormat        = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eStencilFormat      = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.ePresentFlags       = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eRefreshRate        = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.ePresentInterval    = MANGOMT_CLOSEST_TO_INPUT;

		hr = MANGOFindValidDeviceSettings( pDeviceSettings, pDeviceSettings, &matchOptions );
		if( FAILED(hr) ) // the call will fail if no valid devices were found
		{
			MANGODisplayErrorMessage( hr );
			return MANGO_ERR( L"MANGOFindValidDeviceSettings", hr );
		}
	}

	// Change to a Direct3D device created from the new device settings.  
	// If there is an existing device, then either reset or recreate the scene
	hr = MANGOChangeDevice( pDeviceSettings, NULL, false, bClipWindowToSingleAdapter );
	if( FAILED(hr) )
		return hr;

	return S_OK;
}


//======================================================================================
//MANGOMainLoop
//======================================================================================
HRESULT CMANGOXApp::MANGOMainLoop( HACCEL hAccel )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks or reenter
	if( GetMANGOState().GetInsideDeviceCallback() || GetMANGOState().GetInsideMainloop() )
	{
		if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
			GetMANGOState().SetExitCode(1);
		return MANGO_ERR_MSGBOX( L"MANGOMainLoop", E_FAIL );
	}

	GetMANGOState().SetInsideMainloop( true );

	// If MANGOCreateDevice*() or MANGOSetDevice() has not already been called, 
	// then call MANGOCreateDevice() with the default parameters.         
	if( !GetMANGOState().GetDeviceCreated() ) 
	{
		if( GetMANGOState().GetDeviceCreateCalled() )
		{
			if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
				GetMANGOState().SetExitCode(1);
			return E_FAIL; // MANGOCreateDevice() must first succeed for this function to succeed
		}

		hr = MANGOCreateDevice();
		if( FAILED(hr) )
		{
			if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
				GetMANGOState().SetExitCode(1);
			return hr;
		}
	}

	HWND hWnd = MANGOGetHWND();

	// MANGOInit() must have been called and succeeded for this function to proceed
	// MANGOCreateWindow() or MANGOSetWindow() must have been called and succeeded for this function to proceed
	// MANGOCreateDevice() or MANGOCreateDeviceFromSettings() or MANGOSetDevice() must have been called and succeeded for this function to proceed
	if( !GetMANGOState().GetMANGOInited() || !GetMANGOState().GetWindowCreated() || !GetMANGOState().GetDeviceCreated() )
	{
		if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
			GetMANGOState().SetExitCode(1);
		return MANGO_ERR_MSGBOX( L"MANGOMainLoop", E_FAIL );
	}

	// Now we're ready to receive and process Windows messages.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message  )
	{
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

		if( bGotMsg )
		{
			// Translate and dispatch the message
			if( hAccel == NULL || hWnd == NULL || 
				0 == TranslateAccelerator( hWnd, hAccel, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			// Render a frame during idle time (no messages are waiting)
			MANGORender3DEnvironment();
		}
	}

	// Cleanup the accelerator table
	if( hAccel != NULL )
		DestroyAcceleratorTable( hAccel );

	GetMANGOState().SetInsideMainloop( false );

	return S_OK;
}

//======================================================================================
//MANGOApp End
//======================================================================================


//--------------------------------------------------------------------------------------
// Optionally parses the command line and sets if default hotkeys are handled
//
//       Possible command line parameters are:
//          -adapter:#              forces app to use this adapter # (fails if the adapter doesn't exist)
//          -windowed               forces app to start windowed
//          -fullscreen             forces app to start full screen
//          -forcehal               forces app to use HAL (fails if HAL doesn't exist)
//          -forceref               forces app to use REF (fails if REF doesn't exist)
//          -forcepurehwvp          forces app to use pure HWVP (fails if device doesn't support it)
//          -forcehwvp              forces app to use HWVP (fails if device doesn't support it)
//          -forceswvp              forces app to use SWVP 
//          -forcevsync:#           if # is 0, forces app to use D3DPRESENT_INTERVAL_IMMEDIATE otherwise force use of D3DPRESENT_INTERVAL_DEFAULT 
//          -width:#                forces app to use # for width. for full screen, it will pick the closest possible supported mode
//          -height:#               forces app to use # for height. for full screen, it will pick the closest possible supported mode
//          -startx:#               forces app to use # for the x coord of the window position for windowed mode
//          -starty:#               forces app to use # for the y coord of the window position for windowed mode
//          -constantframetime:#    forces app to use constant frame time, where # is the time/frame in seconds
//          -quitafterframe:x       forces app to quit after # frames
//          -noerrormsgboxes        prevents the display of message boxes generated by the framework so the application can be run without user interaction
//          -nostats                prevents the display of the stats
//          -relaunchmce            re-launches the MCE UI after the app exits
//          -automation             every CMANGODialog created will have EnableKeyboardInput(true) called, enabling UI navigation with keyboard
//                                  This is useful when automating application testing.
//
//      Hotkeys handled by default are:
//          Alt-Enter           toggle between full screen & windowed (hotkey always enabled)
//          ESC                 exit app 
//          F3                  toggle HAL/REF
//          F8                  toggle wire-frame mode
//          Pause               pause time
//--------------------------------------------------------------------------------------
HRESULT MANGOInit( bool bParseCommandLine, bool bHandleDefaultHotkeys, bool bShowMsgBoxOnError, bool bHandleAltEnter )
{
	GetMANGOState().SetMANGOInitCalled( true );

	// Not always needed, but lets the app create GDI dialogs
	InitCommonControls();

	// Save the current sticky/toggle/filter key settings so MANGO can restore them later
	STICKYKEYS sk = {sizeof(STICKYKEYS), 0};
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
	GetMANGOState().SetStartupStickyKeys( sk );

	TOGGLEKEYS tk = {sizeof(TOGGLEKEYS), 0};
	SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tk, 0);
	GetMANGOState().SetStartupToggleKeys( tk );

	FILTERKEYS fk = {sizeof(FILTERKEYS), 0};
	SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);
	GetMANGOState().SetStartupFilterKeys( fk );

	// Increase the accuracy of Sleep() without needing to link to winmm.lib
	WCHAR wszPath[MAX_PATH+1];
	if( GetSystemDirectory( wszPath, MAX_PATH+1 ) )
	{
		StringCchCat( wszPath, MAX_PATH, L"\\winmm.dll" );
		HINSTANCE hInstWinMM = LoadLibrary( wszPath );
		if( hInstWinMM ) 
		{
			LPTIMEBEGINPERIOD pTimeBeginPeriod = (LPTIMEBEGINPERIOD)GetProcAddress( hInstWinMM, "timeBeginPeriod" );
			if( NULL != pTimeBeginPeriod )
				pTimeBeginPeriod(1);

			FreeLibrary(hInstWinMM);
		}
	}

	GetMANGOState().SetShowMsgBoxOnError( bShowMsgBoxOnError );
	GetMANGOState().SetHandleDefaultHotkeys( bHandleDefaultHotkeys );
	GetMANGOState().SetHandleAltEnter( bHandleAltEnter );

	if( bParseCommandLine )
		MANGOParseCommandLine();

	// Verify D3DX version
	if( !D3DXCheckVersion( D3D_SDK_VERSION, D3DX_SDK_VERSION ) )
	{
		MANGODisplayErrorMessage( MANGOERR_INCORRECTVERSION );
		return MANGO_ERR( L"D3DXCheckVersion", MANGOERR_INCORRECTVERSION );
	}

	// Create a Direct3D object if one has not already been created
	IDirect3D9* pD3D = MANGOGetD3DObject();
	if( pD3D == NULL )
	{
		// This may fail if DirectX 9 isn't installed
		// This may fail if the DirectX headers are out of sync with the installed DirectX DLLs
		pD3D = MANGO_Dynamic_Direct3DCreate9( D3D_SDK_VERSION );
		GetMANGOState().SetD3D( pD3D );
	}

	if( pD3D == NULL )
	{
		// If still NULL, then something went wrong
		MANGODisplayErrorMessage( MANGOERR_NODIRECT3D );
		return MANGO_ERR( L"Direct3DCreate9", MANGOERR_NODIRECT3D );
	}

	// Reset the timer
	MANGOGetGlobalTimer()->Reset();

	GetMANGOState().SetMANGOInited( true );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// 处理执行程序命令行.  See MANGOInit() for list 
//--------------------------------------------------------------------------------------
void MANGOParseCommandLine()
{
	WCHAR* strCmdLine;
	WCHAR strFlag[MAX_PATH];

	int nNumArgs;
	WCHAR** pstrArgList = CommandLineToArgvW( GetCommandLine(), &nNumArgs );
	for( int iArg=1; iArg<nNumArgs; iArg++ )
	{
		strCmdLine = pstrArgList[iArg];

		// Handle flag args
		if( *strCmdLine == L'/' || *strCmdLine == L'-' )
		{
			strCmdLine++;

			if( MANGOIsNextArg( strCmdLine, L"adapter" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nAdapter = _wtoi(strFlag);
					GetMANGOState().SetOverrideAdapterOrdinal( nAdapter );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"windowed" ) )
			{
				GetMANGOState().SetOverrideWindowed( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"fullscreen" ) )
			{
				GetMANGOState().SetOverrideFullScreen( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"forcehal" ) )
			{
				GetMANGOState().SetOverrideForceHAL( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"forceref" ) )
			{
				GetMANGOState().SetOverrideForceREF( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"forcepurehwvp" ) )
			{
				GetMANGOState().SetOverrideForcePureHWVP( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"forcehwvp" ) )
			{
				GetMANGOState().SetOverrideForceHWVP( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"forceswvp" ) )
			{
				GetMANGOState().SetOverrideForceSWVP( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"forcevsync" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nOn = _wtoi(strFlag);
					GetMANGOState().SetOverrideForceVsync( nOn );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"width" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nWidth = _wtoi(strFlag);
					GetMANGOState().SetOverrideWidth( nWidth );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"height" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nHeight = _wtoi(strFlag);
					GetMANGOState().SetOverrideHeight( nHeight );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"startx" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nX = _wtoi(strFlag);
					GetMANGOState().SetOverrideStartX( nX );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"starty" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nY = _wtoi(strFlag);
					GetMANGOState().SetOverrideStartY( nY );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"constantframetime" ) )
			{
				float fTimePerFrame;
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
					fTimePerFrame = (float)wcstod( strFlag, NULL );
				else
					fTimePerFrame = 0.0333f;
				GetMANGOState().SetOverrideConstantFrameTime( true );
				GetMANGOState().SetOverrideConstantTimePerFrame( fTimePerFrame );
				MANGOSetConstantFrameTime( true, fTimePerFrame );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"quitafterframe" ) )
			{
				if( MANGOGetCmdParam( strCmdLine, strFlag ) )
				{
					int nFrame = _wtoi(strFlag);
					GetMANGOState().SetOverrideQuitAfterFrame( nFrame );
					continue;
				}
			}

			if( MANGOIsNextArg( strCmdLine, L"noerrormsgboxes" ) )
			{
				GetMANGOState().SetShowMsgBoxOnError( false );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"nostats" ) )
			{
				GetMANGOState().SetNoStats( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"relaunchmce" ) )
			{
				GetMANGOState().SetOverrideRelaunchMCE( true );
				continue;
			}

			if( MANGOIsNextArg( strCmdLine, L"automation" ) )
			{
				GetMANGOState().SetAutomation( true );
				continue;
			}
		}

		// Unrecognized flag
		StringCchCopy( strFlag, 256, strCmdLine ); 
		WCHAR* strSpace = strFlag;
		while (*strSpace && (*strSpace > L' '))
			strSpace++;
		*strSpace = 0;

		MANGOOutputDebugString( L"Unrecognized flag: %s", strFlag );
		strCmdLine += wcslen(strFlag);
	}
}


//--------------------------------------------------------------------------------------
// Helper function for MANGOParseCommandLine
//--------------------------------------------------------------------------------------
bool MANGOIsNextArg( WCHAR*& strCmdLine, WCHAR* strArg )
{
	int nArgLen = (int) wcslen(strArg);
	int nCmdLen = (int) wcslen(strCmdLine);

	if( nCmdLen >= nArgLen && 
		_wcsnicmp( strCmdLine, strArg, nArgLen ) == 0 && 
		(strCmdLine[nArgLen] == 0 || strCmdLine[nArgLen] == L':') )
	{
		strCmdLine += nArgLen;
		return true;
	}

	return false;
}


//--------------------------------------------------------------------------------------
// Helper function for MANGOParseCommandLine.  Updates strCmdLine and strFlag 
//      Example: if strCmdLine=="-width:1024 -forceref"
// then after: strCmdLine==" -forceref" and strFlag=="1024"
//--------------------------------------------------------------------------------------
bool MANGOGetCmdParam( WCHAR*& strCmdLine, WCHAR* strFlag )
{
	if( *strCmdLine == L':' )
	{       
		strCmdLine++; // Skip ':'

		// Place NULL terminator in strFlag after current token
		StringCchCopy( strFlag, 256, strCmdLine );
		WCHAR* strSpace = strFlag;
		while (*strSpace && (*strSpace > L' '))
			strSpace++;
		*strSpace = 0;

		// Update strCmdLine
		strCmdLine += wcslen(strFlag);
		return true;
	}
	else
	{
		strFlag[0] = 0;
		return false;
	}
}


//--------------------------------------------------------------------------------------
// Creates a window with the specified window title, icon, menu, and 
// starting position.  If MANGOInit() has not already been called, it will
// call it with the default parameters.  Instead of calling this, you can 
// call MANGOSetWindow() to use an existing window.  
//--------------------------------------------------------------------------------------
HRESULT MANGOCreateWindow( const WCHAR* strWindowTitle, DWORD dwStyle, HINSTANCE hInstance, 
						 HICON hIcon, HMENU hMenu, int x, int y )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	GetMANGOState().SetWindowCreateCalled( true );

	if( !GetMANGOState().GetMANGOInited() ) 
	{
		// If MANGOInit() was already called and failed, then fail.
		// MANGOInit() must first succeed for this function to succeed
		if( GetMANGOState().GetMANGOInitCalled() )
			return E_FAIL; 

		// If MANGOInit() hasn't been called, then automatically call it
		// with default params
		hr = MANGOInit();
		if( FAILED(hr) )
			return hr;
	}

	if( MANGOGetHWNDFocus() == NULL )
	{
		if( hInstance == NULL ) 
			hInstance = (HINSTANCE)GetModuleHandle(NULL);
		GetMANGOState().SetHInstance( hInstance );

		WCHAR szExePath[MAX_PATH];
		GetModuleFileName( NULL, szExePath, MAX_PATH );
		if( hIcon == NULL ) // If the icon is NULL, then use the first one found in the exe
			hIcon = ExtractIcon( hInstance, szExePath, 0 ); 

		// Register the windows class
		WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS;
		wndClass.lpfnWndProc = MANGOStaticWndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = hIcon;
		wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = L"Direct3DWindowClass";

		if( !RegisterClass( &wndClass ) )
		{
			DWORD dwError = GetLastError();
			if( dwError != ERROR_CLASS_ALREADY_EXISTS )
				return MANGO_ERR_MSGBOX( L"RegisterClass", HRESULT_FROM_WIN32(dwError) );
		}

		RECT rc;
		// Override the window's initial & size position if there were cmd line args
		if( GetMANGOState().GetOverrideStartX() != -1 )
			x = GetMANGOState().GetOverrideStartX();
		if( GetMANGOState().GetOverrideStartY() != -1 )
			y = GetMANGOState().GetOverrideStartY();

		GetMANGOState().SetWindowCreatedWithDefaultPositions( false );
		if( x == CW_USEDEFAULT && y == CW_USEDEFAULT )
			GetMANGOState().SetWindowCreatedWithDefaultPositions( true );

		// Find the window's initial size, but it might be changed later
		int nDefaultWidth = 640;
		int nDefaultHeight = 480;
		if( GetMANGOState().GetOverrideWidth() != 0 )
			nDefaultWidth = GetMANGOState().GetOverrideWidth();
		if( GetMANGOState().GetOverrideHeight() != 0 )
			nDefaultHeight = GetMANGOState().GetOverrideHeight();
		SetRect( &rc, 0, 0, nDefaultWidth, nDefaultHeight );        
		AdjustWindowRect( &rc, dwStyle, ( hMenu != NULL ) ? true : false );

		WCHAR* strCachedWindowTitle = GetMANGOState().GetWindowTitle();
		StringCchCopy( strCachedWindowTitle, 256, strWindowTitle );

		// Create the render window
		HWND hWnd = CreateWindow( L"Direct3DWindowClass", strWindowTitle, dwStyle,
			x, y, (rc.right-rc.left), (rc.bottom-rc.top), 0,
			hMenu, hInstance, 0 );
		if( hWnd == NULL )
		{
			DWORD dwError = GetLastError();
			return MANGO_ERR_MSGBOX( L"CreateWindow", HRESULT_FROM_WIN32(dwError) );
		}

		GetMANGOState().SetWindowCreated( true );
		GetMANGOState().SetHWNDFocus( hWnd );
		GetMANGOState().SetHWNDDeviceFullScreen( hWnd );
		GetMANGOState().SetHWNDDeviceWindowed( hWnd );
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Sets a previously created window for the framework to use.  If MANGOInit() 
// has not already been called, it will call it with the default parameters.  
// Instead of calling this, you can call MANGOCreateWindow() to create a new window.  
//--------------------------------------------------------------------------------------
HRESULT MANGOSetWindow( HWND hWndFocus, HWND hWndDeviceFullScreen, HWND hWndDeviceWindowed, bool bHandleMessages )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	GetMANGOState().SetWindowCreateCalled( true );

	// To avoid confusion, we do not allow any HWND to be NULL here.  The
	// caller must pass in valid HWND for all three parameters.  The same
	// HWND may be used for more than one parameter.
	if( hWndFocus == NULL || hWndDeviceFullScreen == NULL || hWndDeviceWindowed == NULL )
		return MANGO_ERR_MSGBOX( L"MANGOSetWindow", E_INVALIDARG );

	// If subclassing the window, set the pointer to the local window procedure
	if( bHandleMessages )
	{
		// Switch window procedures
#ifdef _WIN64
		LONG_PTR nResult = SetWindowLongPtr( hWndFocus, GWLP_WNDPROC, (LONG_PTR)MANGOStaticWndProc );
#else
		LONG_PTR nResult = SetWindowLongPtr( hWndFocus, GWLP_WNDPROC, (LONG)(LONG_PTR)MANGOStaticWndProc );
#endif 

		DWORD dwError = GetLastError();
		if( nResult == 0 )
			return MANGO_ERR_MSGBOX( L"SetWindowLongPtr", HRESULT_FROM_WIN32(dwError) );
	}

	if( !GetMANGOState().GetMANGOInited() ) 
	{
		// If MANGOInit() was already called and failed, then fail.
		// MANGOInit() must first succeed for this function to succeed
		if( GetMANGOState().GetMANGOInitCalled() )
			return E_FAIL; 

		// If MANGOInit() hasn't been called, then automatically call it
		// with default params
		hr = MANGOInit();
		if( FAILED(hr) )
			return hr;
	}

	WCHAR* strCachedWindowTitle = GetMANGOState().GetWindowTitle();
	GetWindowText( hWndFocus, strCachedWindowTitle, 255 );
	strCachedWindowTitle[255] = 0;

	HINSTANCE hInstance = (HINSTANCE) (LONG_PTR) GetWindowLongPtr( hWndFocus, GWLP_HINSTANCE ); 
	GetMANGOState().SetHInstance( hInstance );
	GetMANGOState().SetWindowCreatedWithDefaultPositions( false );
	GetMANGOState().SetWindowCreated( true );
	GetMANGOState().SetHWNDFocus( hWndFocus );
	GetMANGOState().SetHWNDDeviceFullScreen( hWndDeviceFullScreen );
	GetMANGOState().SetHWNDDeviceWindowed( hWndDeviceWindowed );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Creates a Direct3D device. If MANGOCreateWindow() or MANGOSetWindow() has not already 
// been called, it will call MANGOCreateWindow() with the default parameters.  
// Instead of calling this, you can call MANGOSetDevice() or MANGOCreateDeviceFromSettings() 
//--------------------------------------------------------------------------------------
HRESULT MANGOCreateDevice( UINT AdapterOrdinal, bool bWindowed, 
						 int nSuggestedWidth, int nSuggestedHeight,
						 LPMANGOCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable,
						 LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings,
						 void* pUserContext )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	//设置回调函数
	// Record the function arguments in the global state 
	GetMANGOState().SetIsDeviceAcceptableFunc( pCallbackIsDeviceAcceptable );
	GetMANGOState().SetModifyDeviceSettingsFunc( pCallbackModifyDeviceSettings );
	GetMANGOState().SetIsDeviceAcceptableFuncUserContext( pUserContext );
	GetMANGOState().SetModifyDeviceSettingsFuncUserContext( pUserContext );

	GetMANGOState().SetDeviceCreateCalled( true );

	// If MANGOCreateWindow() or MANGOSetWindow() has not already been called, 
	// then call MANGOCreateWindow() with the default parameters.         
	if( !GetMANGOState().GetWindowCreated() ) 
	{
		// If MANGOCreateWindow() or MANGOSetWindow() was already called and failed, then fail.
		// MANGOCreateWindow() or MANGOSetWindow() must first succeed for this function to succeed
		if( GetMANGOState().GetWindowCreateCalled() )
			return E_FAIL; 

		// If MANGOCreateWindow() or MANGOSetWindow() hasn't been called, then 
		// automatically call MANGOCreateWindow() with default params
		hr = MANGOCreateWindow();
		if( FAILED(hr) )
			return hr;
	}

	// Force an enumeration with the new IsDeviceAcceptable callback
	MANGOPrepareEnumerationObject( true );

	MANGOMatchOptions matchOptions;
	matchOptions.eAdapterOrdinal     = MANGOMT_PRESERVE_INPUT;
	matchOptions.eDeviceType         = MANGOMT_IGNORE_INPUT;
	matchOptions.eWindowed           = MANGOMT_PRESERVE_INPUT;
	matchOptions.eAdapterFormat      = MANGOMT_IGNORE_INPUT;
	matchOptions.eVertexProcessing   = MANGOMT_IGNORE_INPUT;
	if( bWindowed || (nSuggestedWidth != 0 && nSuggestedHeight != 0) )
		matchOptions.eResolution     = MANGOMT_CLOSEST_TO_INPUT;
	else
		matchOptions.eResolution     = MANGOMT_IGNORE_INPUT;
	matchOptions.eBackBufferFormat   = MANGOMT_IGNORE_INPUT;
	matchOptions.eBackBufferCount    = MANGOMT_IGNORE_INPUT;
	matchOptions.eMultiSample        = MANGOMT_IGNORE_INPUT;
	matchOptions.eSwapEffect         = MANGOMT_IGNORE_INPUT;
	matchOptions.eDepthFormat        = MANGOMT_IGNORE_INPUT;
	matchOptions.eStencilFormat      = MANGOMT_IGNORE_INPUT;
	matchOptions.ePresentFlags       = MANGOMT_IGNORE_INPUT;
	matchOptions.eRefreshRate        = MANGOMT_IGNORE_INPUT;
	matchOptions.ePresentInterval    = MANGOMT_IGNORE_INPUT;

	MANGODeviceSettings deviceSettings;
	ZeroMemory( &deviceSettings, sizeof(MANGODeviceSettings) );
	deviceSettings.AdapterOrdinal      = AdapterOrdinal;
	deviceSettings.pp.Windowed         = bWindowed;
	deviceSettings.pp.BackBufferWidth  = nSuggestedWidth;
	deviceSettings.pp.BackBufferHeight = nSuggestedHeight;

	// Override with settings from the command line
	if( GetMANGOState().GetOverrideWidth() != 0 )
		deviceSettings.pp.BackBufferWidth = GetMANGOState().GetOverrideWidth();
	if( GetMANGOState().GetOverrideHeight() != 0 )
		deviceSettings.pp.BackBufferHeight = GetMANGOState().GetOverrideHeight();

	if( GetMANGOState().GetOverrideAdapterOrdinal() != -1 )
		deviceSettings.AdapterOrdinal = GetMANGOState().GetOverrideAdapterOrdinal();

	if( GetMANGOState().GetOverrideFullScreen() )
	{
		deviceSettings.pp.Windowed = FALSE;
		if( GetMANGOState().GetOverrideWidth() == 0 && GetMANGOState().GetOverrideHeight() == 0 )
			matchOptions.eResolution = MANGOMT_IGNORE_INPUT;
	}
	if( GetMANGOState().GetOverrideWindowed() )
		deviceSettings.pp.Windowed = TRUE;

	if( GetMANGOState().GetOverrideForceHAL() )
	{
		deviceSettings.DeviceType = D3DDEVTYPE_HAL;
		matchOptions.eDeviceType = MANGOMT_PRESERVE_INPUT;
	}
	if( GetMANGOState().GetOverrideForceREF() )
	{
		deviceSettings.DeviceType = D3DDEVTYPE_REF;
		matchOptions.eDeviceType = MANGOMT_PRESERVE_INPUT;
	}

	if( GetMANGOState().GetOverrideForcePureHWVP() )
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
		matchOptions.eVertexProcessing = MANGOMT_PRESERVE_INPUT;
	}
	else if( GetMANGOState().GetOverrideForceHWVP() )
	{
		deviceSettings.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		matchOptions.eVertexProcessing = MANGOMT_PRESERVE_INPUT;
	}
	else if( GetMANGOState().GetOverrideForceSWVP() )
	{
		deviceSettings.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		matchOptions.eVertexProcessing = MANGOMT_PRESERVE_INPUT;
	}

	if( GetMANGOState().GetOverrideForceVsync() == 0 )
	{
		deviceSettings.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		matchOptions.ePresentInterval = MANGOMT_PRESERVE_INPUT;
	}
	else if( GetMANGOState().GetOverrideForceVsync() == 1 )
	{
		deviceSettings.pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		matchOptions.ePresentInterval = MANGOMT_PRESERVE_INPUT;
	}

	hr = MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );
	if( FAILED(hr) ) // the call will fail if no valid devices were found
	{
		MANGODisplayErrorMessage( hr );
		return MANGO_ERR( L"MANGOFindValidDeviceSettings", hr );
	}

	// Change to a Direct3D device created from the new device settings.  
	// If there is an existing device, then either reset or recreated the scene
	hr = MANGOChangeDevice( &deviceSettings, NULL, false, true );
	if( FAILED(hr) )
		return hr;

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Passes a previously created Direct3D device for use by the framework.  
// If MANGOCreateWindow() has not already been called, it will call it with the 
// default parameters.  Instead of calling this, you can call MANGOCreateDevice() or 
// MANGOCreateDeviceFromSettings() 
//--------------------------------------------------------------------------------------
HRESULT MANGOSetDevice( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr;

	if( pd3dDevice == NULL )
		return MANGO_ERR_MSGBOX( L"MANGOSetDevice", E_INVALIDARG );

	// Not allowed to call this from inside the device callbacks
	if( GetMANGOState().GetInsideDeviceCallback() )
		return MANGO_ERR_MSGBOX( L"MANGOCreateWindow", E_FAIL );

	GetMANGOState().SetDeviceCreateCalled( true );

	// If MANGOCreateWindow() or MANGOSetWindow() has not already been called, 
	// then call MANGOCreateWindow() with the default parameters.         
	if( !GetMANGOState().GetWindowCreated() ) 
	{
		// If MANGOCreateWindow() or MANGOSetWindow() was already called and failed, then fail.
		// MANGOCreateWindow() or MANGOSetWindow() must first succeed for this function to succeed
		if( GetMANGOState().GetWindowCreateCalled() )
			return E_FAIL; 

		// If MANGOCreateWindow() or MANGOSetWindow() hasn't been called, then 
		// automatically call MANGOCreateWindow() with default params
		hr = MANGOCreateWindow();
		if( FAILED(hr) )
			return hr;
	}

	MANGODeviceSettings* pDeviceSettings = new MANGODeviceSettings;
	if( pDeviceSettings == NULL )
		return E_OUTOFMEMORY;
	ZeroMemory( pDeviceSettings, sizeof(MANGODeviceSettings) );

	// Get the present params from the swap chain
	IDirect3DSurface9* pBackBuffer = NULL;
	hr = pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	if( SUCCEEDED(hr) )
	{
		IDirect3DSwapChain9* pSwapChain = NULL;
		hr = pBackBuffer->GetContainer( IID_IDirect3DSwapChain9, (void**) &pSwapChain );
		if( SUCCEEDED(hr) )
		{
			pSwapChain->GetPresentParameters( &pDeviceSettings->pp );
			SAFE_RELEASE( pSwapChain );
		}

		SAFE_RELEASE( pBackBuffer );
	}

	D3DDEVICE_CREATION_PARAMETERS d3dCreationParams;
	pd3dDevice->GetCreationParameters( &d3dCreationParams );

	// Fill out the rest of the device settings struct
	pDeviceSettings->AdapterOrdinal = d3dCreationParams.AdapterOrdinal;
	pDeviceSettings->DeviceType     = d3dCreationParams.DeviceType;
	MANGOFindAdapterFormat( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, 
		pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed, 
		&pDeviceSettings->AdapterFormat );
	pDeviceSettings->BehaviorFlags  = d3dCreationParams.BehaviorFlags;

	// Change to the Direct3D device passed in
	hr = MANGOChangeDevice( pDeviceSettings, pd3dDevice, false, false );

	delete pDeviceSettings;

	if( FAILED(hr) ) 
		return hr;

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Tells the framework to change to a device created from the passed in device settings
// If MANGOCreateWindow() has not already been called, it will call it with the 
// default parameters.  Instead of calling this, you can call MANGOCreateDevice() 
// or MANGOSetDevice() 
//--------------------------------------------------------------------------------------
HRESULT MANGOCreateDeviceFromSettings( MANGODeviceSettings* pDeviceSettings, bool bPreserveInput, bool bClipWindowToSingleAdapter )
{
	HRESULT hr;

	GetMANGOState().SetDeviceCreateCalled( true );

	// If MANGOCreateWindow() or MANGOSetWindow() has not already been called, 
	// then call MANGOCreateWindow() with the default parameters.         
	if( !GetMANGOState().GetWindowCreated() ) 
	{
		// If MANGOCreateWindow() or MANGOSetWindow() was already called and failed, then fail.
		// MANGOCreateWindow() or MANGOSetWindow() must first succeed for this function to succeed
		if( GetMANGOState().GetWindowCreateCalled() )
			return E_FAIL; 

		// If MANGOCreateWindow() or MANGOSetWindow() hasn't been called, then 
		// automatically call MANGOCreateWindow() with default params
		hr = MANGOCreateWindow();
		if( FAILED(hr) )
			return hr;
	}

	if( !bPreserveInput )
	{
		// If not preserving the input, then find the closest valid to it
		MANGOMatchOptions matchOptions;
		matchOptions.eAdapterOrdinal     = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eDeviceType         = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eWindowed           = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eAdapterFormat      = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eVertexProcessing   = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eResolution         = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eBackBufferFormat   = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eBackBufferCount    = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eMultiSample        = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eSwapEffect         = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eDepthFormat        = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eStencilFormat      = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.ePresentFlags       = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.eRefreshRate        = MANGOMT_CLOSEST_TO_INPUT;
		matchOptions.ePresentInterval    = MANGOMT_CLOSEST_TO_INPUT;

		hr = MANGOFindValidDeviceSettings( pDeviceSettings, pDeviceSettings, &matchOptions );
		if( FAILED(hr) ) // the call will fail if no valid devices were found
		{
			MANGODisplayErrorMessage( hr );
			return MANGO_ERR( L"MANGOFindValidDeviceSettings", hr );
		}
	}

	// Change to a Direct3D device created from the new device settings.  
	// If there is an existing device, then either reset or recreate the scene
	hr = MANGOChangeDevice( pDeviceSettings, NULL, false, bClipWindowToSingleAdapter );
	if( FAILED(hr) )
		return hr;

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Toggle between full screen and windowed
//--------------------------------------------------------------------------------------
HRESULT MANGOToggleFullScreen()
{
	HRESULT hr;

	// Get the current device settings and flip the windowed state then
	// find the closest valid device settings with this change
	MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
	deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;

	MANGOMatchOptions matchOptions;
	matchOptions.eAdapterOrdinal     = MANGOMT_PRESERVE_INPUT;
	matchOptions.eDeviceType         = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eWindowed           = MANGOMT_PRESERVE_INPUT;
	matchOptions.eAdapterFormat      = MANGOMT_IGNORE_INPUT;
	matchOptions.eVertexProcessing   = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eBackBufferFormat   = MANGOMT_IGNORE_INPUT;
	matchOptions.eBackBufferCount    = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eMultiSample        = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eSwapEffect         = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eDepthFormat        = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eStencilFormat      = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.ePresentFlags       = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eRefreshRate        = MANGOMT_IGNORE_INPUT;
	matchOptions.ePresentInterval    = MANGOMT_CLOSEST_TO_INPUT;

	// Go back to previous state

	UINT nWidth  = ( deviceSettings.pp.Windowed ) ? GetMANGOState().GetWindowBackBufferWidthAtModeChange() : GetMANGOState().GetFullScreenBackBufferWidthAtModeChange();
	UINT nHeight = ( deviceSettings.pp.Windowed ) ? GetMANGOState().GetWindowBackBufferHeightAtModeChange() : GetMANGOState().GetFullScreenBackBufferHeightAtModeChange();
	if( nWidth > 0 && nHeight > 0 )
	{
		matchOptions.eResolution = MANGOMT_CLOSEST_TO_INPUT;
		deviceSettings.pp.BackBufferWidth = nWidth;
		deviceSettings.pp.BackBufferHeight = nHeight;
	}
	else
	{
		// No previous data, so just switch to defaults
		matchOptions.eResolution = MANGOMT_IGNORE_INPUT;
	}

	hr = MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );
	if( SUCCEEDED(hr) ) 
	{
		// Create a Direct3D device using the new device settings.  
		// If there is an existing device, then it will either reset or recreate the scene.
		hr = MANGOChangeDevice( &deviceSettings, NULL, false, false );

		// If hr == E_ABORT, this means the app rejected the device settings in the ModifySettingsCallback so nothing changed
		if( FAILED(hr) && (hr != E_ABORT) )
		{
			// Failed creating device, try to switch back.
			deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;
			UINT nWidth  = ( deviceSettings.pp.Windowed ) ? GetMANGOState().GetWindowBackBufferWidthAtModeChange() : GetMANGOState().GetFullScreenBackBufferWidthAtModeChange();
			UINT nHeight = ( deviceSettings.pp.Windowed ) ? GetMANGOState().GetWindowBackBufferHeightAtModeChange() : GetMANGOState().GetFullScreenBackBufferHeightAtModeChange();
			if( nWidth > 0 && nHeight > 0 )
			{
				matchOptions.eResolution = MANGOMT_CLOSEST_TO_INPUT;
				deviceSettings.pp.BackBufferWidth = nWidth;
				deviceSettings.pp.BackBufferHeight = nHeight;
			}
			else
			{
				matchOptions.eResolution = MANGOMT_IGNORE_INPUT;
			}

			MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );

			HRESULT hr2 = MANGOChangeDevice( &deviceSettings, NULL, false, false );
			if( FAILED(hr2) )
			{
				// If this failed, then shutdown
				MANGOShutdown();
			}
		}
	}

	return hr;
}


//--------------------------------------------------------------------------------------
// Toggle between HAL and REF
//--------------------------------------------------------------------------------------
HRESULT MANGOToggleREF()
{
	HRESULT hr;

	MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
	if( deviceSettings.DeviceType == D3DDEVTYPE_HAL )
		deviceSettings.DeviceType = D3DDEVTYPE_REF;
	else if( deviceSettings.DeviceType == D3DDEVTYPE_REF )
		deviceSettings.DeviceType = D3DDEVTYPE_HAL;

	MANGOMatchOptions matchOptions;
	matchOptions.eAdapterOrdinal     = MANGOMT_PRESERVE_INPUT;
	matchOptions.eDeviceType         = MANGOMT_PRESERVE_INPUT;
	matchOptions.eWindowed           = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eAdapterFormat      = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eVertexProcessing   = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eResolution         = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eBackBufferFormat   = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eBackBufferCount    = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eMultiSample        = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eSwapEffect         = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eDepthFormat        = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eStencilFormat      = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.ePresentFlags       = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.eRefreshRate        = MANGOMT_CLOSEST_TO_INPUT;
	matchOptions.ePresentInterval    = MANGOMT_CLOSEST_TO_INPUT;

	hr = MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );
	if( SUCCEEDED(hr) ) 
	{
		// Create a Direct3D device using the new device settings.  
		// If there is an existing device, then it will either reset or recreate the scene.
		hr = MANGOChangeDevice( &deviceSettings, NULL, false, false );

		// If hr == E_ABORT, this means the app rejected the device settings in the ModifySettingsCallback so nothing changed
		if( FAILED( hr ) && (hr != E_ABORT) )
		{
			// Failed creating device, try to switch back.
			if( deviceSettings.DeviceType == D3DDEVTYPE_HAL )
				deviceSettings.DeviceType = D3DDEVTYPE_REF;
			else if( deviceSettings.DeviceType == D3DDEVTYPE_REF )
				deviceSettings.DeviceType = D3DDEVTYPE_HAL;

			MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );

			HRESULT hr2 = MANGOChangeDevice( &deviceSettings, NULL, false, false );
			if( FAILED(hr2) )
			{
				// If this failed, then shutdown
				MANGOShutdown();
			}
		}
	}

	return hr;
}


//--------------------------------------------------------------------------------------
// Internal helper function to prepare the enumeration object by creating it if it 
// didn't already exist and enumerating if desired.
//--------------------------------------------------------------------------------------
CD3DEnumeration* MANGOPrepareEnumerationObject( bool bEnumerate )
{
	// Create a new CD3DEnumeration object and enumerate all devices unless its already been done
	CD3DEnumeration* pd3dEnum = GetMANGOState().GetD3DEnumeration();
	if( pd3dEnum == NULL )
	{
		pd3dEnum = MANGOGetEnumeration(); 
		GetMANGOState().SetD3DEnumeration( pd3dEnum );

		bEnumerate = true;
	}

	if( bEnumerate )
	{
		// Enumerate for each adapter all of the supported display modes, 
		// device types, adapter formats, back buffer formats, window/full screen support, 
		// depth stencil formats, multisampling types/qualities, and presentations intervals.
		//
		// For each combination of device type (HAL/REF), adapter format, back buffer format, and
		// IsWindowed it will call the app's ConfirmDevice callback.  This allows the app
		// to reject or allow that combination based on its caps/etc.  It also allows the 
		// app to change the BehaviorFlags.  The BehaviorFlags defaults non-pure HWVP 
		// if supported otherwise it will default to SWVP, however the app can change this 
		// through the ConfirmDevice callback.
		IDirect3D9* pD3D = MANGOGetD3DObject();
		pd3dEnum->Enumerate( pD3D, GetMANGOState().GetIsDeviceAcceptableFunc(), GetMANGOState().GetIsDeviceAcceptableFuncUserContext() );
	}

	return pd3dEnum;
}


//--------------------------------------------------------------------------------------
// This function tries to find valid device settings based upon the input device settings 
// struct and the match options.  For each device setting a match option in the 
// MANGOMatchOptions struct specifies how the function makes decisions.  For example, if 
// the caller wants a HAL device with a back buffer format of D3DFMT_A2B10G10R10 but the 
// HAL device on the system does not support D3DFMT_A2B10G10R10 however a REF device is 
// installed that does, then the function has a choice to either use REF or to change to 
// a back buffer format to compatible with the HAL device.  The match options lets the 
// caller control how these choices are made.
//
// Each match option must be one of the following types: 
//      MANGOMT_IGNORE_INPUT: Uses the closest valid value to a default 
//      MANGOMT_PRESERVE_INPUT: Uses the input without change, but may cause no valid device to be found
//      MANGOMT_CLOSEST_TO_INPUT: Uses the closest valid value to the input 
//
// If pMatchOptions is NULL then, all of the match options are assumed to be MANGOMT_IGNORE_INPUT.  
// The function returns failure if no valid device settings can be found otherwise 
// the function returns success and the valid device settings are written to pOut.
//--------------------------------------------------------------------------------------
HRESULT MANGOFindValidDeviceSettings( MANGODeviceSettings* pOut, MANGODeviceSettings* pIn, 
									MANGOMatchOptions* pMatchOptions )
{
	if( pOut == NULL )
		return MANGO_ERR_MSGBOX( L"MANGOFindValidDeviceSettings", E_INVALIDARG );

	CD3DEnumeration* pd3dEnum = MANGOPrepareEnumerationObject( false );
	IDirect3D9*      pD3D     = MANGOGetD3DObject();

	// Default to MANGOMT_IGNORE_INPUT for everything unless pMatchOptions isn't NULL
	MANGOMatchOptions defaultMatchOptions;
	if( NULL == pMatchOptions )
	{
		ZeroMemory( &defaultMatchOptions, sizeof(MANGOMatchOptions) );
		pMatchOptions = &defaultMatchOptions;
	}

	// Build an optimal device settings structure based upon the match 
	// options.  If the match option is set to ignore, then a optimal default value is used.
	// The default value may not exist on the system, but later this will be taken 
	// into account.
	MANGODeviceSettings optimalDeviceSettings;
	MANGOBuildOptimalDeviceSettings( &optimalDeviceSettings, pIn, pMatchOptions );

	// Find the best combination of:
	//      Adapter Ordinal
	//      Device Type
	//      Adapter Format
	//      Back Buffer Format
	//      Windowed
	// given what's available on the system and the match options combined with the device settings input.
	// This combination of settings is encapsulated by the CD3DEnumDeviceSettingsCombo class.
	float fBestRanking = -1.0f;
	CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo = NULL;
	D3DDISPLAYMODE adapterDesktopDisplayMode;

	CMangoArray<CD3DEnumAdapterInfo*>* pAdapterList = pd3dEnum->GetAdapterInfoList();
	for( int iAdapter=0; iAdapter<pAdapterList->GetSize(); iAdapter++ )
	{
		CD3DEnumAdapterInfo* pAdapterInfo = pAdapterList->GetAt(iAdapter);

		// Get the desktop display mode of adapter 
		pD3D->GetAdapterDisplayMode( pAdapterInfo->AdapterOrdinal, &adapterDesktopDisplayMode );

		// Enum all the device types supported by this adapter to find the best device settings
		for( int iDeviceInfo=0; iDeviceInfo<pAdapterInfo->deviceInfoList.GetSize(); iDeviceInfo++ )
		{
			CD3DEnumDeviceInfo* pDeviceInfo = pAdapterInfo->deviceInfoList.GetAt(iDeviceInfo);

			// Enum all the device settings combinations.  A device settings combination is 
			// a unique set of an adapter format, back buffer format, and IsWindowed.
			for( int iDeviceCombo=0; iDeviceCombo<pDeviceInfo->deviceSettingsComboList.GetSize(); iDeviceCombo++ )
			{
				CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pDeviceInfo->deviceSettingsComboList.GetAt(iDeviceCombo);

				// If windowed mode the adapter format has to be the same as the desktop 
				// display mode format so skip any that don't match
				if (pDeviceSettingsCombo->Windowed && (pDeviceSettingsCombo->AdapterFormat != adapterDesktopDisplayMode.Format))
					continue;

				// Skip any combo that doesn't meet the preserve match options
				if( false == MANGODoesDeviceComboMatchPreserveOptions( pDeviceSettingsCombo, pIn, pMatchOptions ) )
					continue;           

				// Get a ranking number that describes how closely this device combo matches the optimal combo
				float fCurRanking = MANGORankDeviceCombo( pDeviceSettingsCombo, &optimalDeviceSettings, &adapterDesktopDisplayMode );

				// If this combo better matches the input device settings then save it
				if( fCurRanking > fBestRanking )
				{
					pBestDeviceSettingsCombo = pDeviceSettingsCombo;
					fBestRanking = fCurRanking;
				}                
			}
		}
	}

	// If no best device combination was found then fail
	if( pBestDeviceSettingsCombo == NULL ) 
		return MANGOERR_NOCOMPATIBLEDEVICES;

	// Using the best device settings combo found, build valid device settings taking heed of 
	// the match options and the input device settings
	MANGODeviceSettings validDeviceSettings;
	MANGOBuildValidDeviceSettings( &validDeviceSettings, pBestDeviceSettingsCombo, pIn, pMatchOptions );
	*pOut = validDeviceSettings;

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Internal helper function to build a device settings structure based upon the match 
// options.  If the match option is set to ignore, then a optimal default value is used.
// The default value may not exist on the system, but later this will be taken 
// into account.
//--------------------------------------------------------------------------------------
void MANGOBuildOptimalDeviceSettings( MANGODeviceSettings* pOptimalDeviceSettings, 
									MANGODeviceSettings* pDeviceSettingsIn, 
									MANGOMatchOptions* pMatchOptions )
{
	IDirect3D9* pD3D = MANGOGetD3DObject();
	D3DDISPLAYMODE adapterDesktopDisplayMode;

	ZeroMemory( pOptimalDeviceSettings, sizeof(MANGODeviceSettings) ); 

	//---------------------
	// Adapter ordinal
	//---------------------    
	if( pMatchOptions->eAdapterOrdinal == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->AdapterOrdinal = D3DADAPTER_DEFAULT; 
	else
		pOptimalDeviceSettings->AdapterOrdinal = pDeviceSettingsIn->AdapterOrdinal;      

	//---------------------
	// Device type
	//---------------------
	if( pMatchOptions->eDeviceType == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->DeviceType = D3DDEVTYPE_HAL; 
	else
		pOptimalDeviceSettings->DeviceType = pDeviceSettingsIn->DeviceType;

	//---------------------
	// Windowed
	//---------------------
	if( pMatchOptions->eWindowed == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->pp.Windowed = TRUE; 
	else
		pOptimalDeviceSettings->pp.Windowed = pDeviceSettingsIn->pp.Windowed;

	//---------------------
	// Adapter format
	//---------------------
	if( pMatchOptions->eAdapterFormat == MANGOMT_IGNORE_INPUT )
	{
		// If windowed, default to the desktop display mode
		// If fullscreen, default to the desktop display mode for quick mode change or 
		// default to D3DFMT_X8R8G8B8 if the desktop display mode is < 32bit
		pD3D->GetAdapterDisplayMode( pOptimalDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode );
		if( pOptimalDeviceSettings->pp.Windowed || MANGOColorChannelBits(adapterDesktopDisplayMode.Format) >= 8 )
			pOptimalDeviceSettings->AdapterFormat = adapterDesktopDisplayMode.Format;
		else
			pOptimalDeviceSettings->AdapterFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		pOptimalDeviceSettings->AdapterFormat = pDeviceSettingsIn->AdapterFormat;
	}

	//---------------------
	// Vertex processing
	//---------------------
	if( pMatchOptions->eVertexProcessing == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING; 
	else
		pOptimalDeviceSettings->BehaviorFlags = pDeviceSettingsIn->BehaviorFlags;

	//---------------------
	// Resolution
	//---------------------
	if( pMatchOptions->eResolution == MANGOMT_IGNORE_INPUT )
	{
		// If windowed, default to 640x480
		// If fullscreen, default to the desktop res for quick mode change
		if( pOptimalDeviceSettings->pp.Windowed )
		{
			pOptimalDeviceSettings->pp.BackBufferWidth = 640;
			pOptimalDeviceSettings->pp.BackBufferHeight = 480;
		}
		else
		{
			pD3D->GetAdapterDisplayMode( pOptimalDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode );
			pOptimalDeviceSettings->pp.BackBufferWidth = adapterDesktopDisplayMode.Width;
			pOptimalDeviceSettings->pp.BackBufferHeight = adapterDesktopDisplayMode.Height;
		}
	}
	else
	{
		pOptimalDeviceSettings->pp.BackBufferWidth = pDeviceSettingsIn->pp.BackBufferWidth;
		pOptimalDeviceSettings->pp.BackBufferHeight = pDeviceSettingsIn->pp.BackBufferHeight;
	}

	//---------------------
	// Back buffer format
	//---------------------
	if( pMatchOptions->eBackBufferFormat == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->pp.BackBufferFormat = pOptimalDeviceSettings->AdapterFormat; // Default to match the adapter format
	else
		pOptimalDeviceSettings->pp.BackBufferFormat = pDeviceSettingsIn->pp.BackBufferFormat;

	//---------------------
	// Back buffer count
	//---------------------
	if( pMatchOptions->eBackBufferCount == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->pp.BackBufferCount = 2; // Default to triple buffering for perf gain
	else
		pOptimalDeviceSettings->pp.BackBufferCount = pDeviceSettingsIn->pp.BackBufferCount;

	//---------------------
	// Multisample
	//---------------------
	if( pMatchOptions->eMultiSample == MANGOMT_IGNORE_INPUT )
	{
		// Default to no multisampling 
		pOptimalDeviceSettings->pp.MultiSampleType = D3DMULTISAMPLE_NONE;
		pOptimalDeviceSettings->pp.MultiSampleQuality = 0; 
	}
	else
	{
		pOptimalDeviceSettings->pp.MultiSampleType = pDeviceSettingsIn->pp.MultiSampleType;
		pOptimalDeviceSettings->pp.MultiSampleQuality = pDeviceSettingsIn->pp.MultiSampleQuality;
	}

	//---------------------
	// Swap effect
	//---------------------
	if( pMatchOptions->eSwapEffect == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->pp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
	else
		pOptimalDeviceSettings->pp.SwapEffect = pDeviceSettingsIn->pp.SwapEffect;

	//---------------------
	// Depth stencil 
	//---------------------
	if( pMatchOptions->eDepthFormat == MANGOMT_IGNORE_INPUT &&
		pMatchOptions->eStencilFormat == MANGOMT_IGNORE_INPUT )
	{
		UINT nBackBufferBits = MANGOColorChannelBits( pOptimalDeviceSettings->pp.BackBufferFormat );
		if( nBackBufferBits >= 8 )
			pOptimalDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D32; 
		else
			pOptimalDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D16; 
	}
	else
	{
		pOptimalDeviceSettings->pp.AutoDepthStencilFormat = pDeviceSettingsIn->pp.AutoDepthStencilFormat;
	}

	//---------------------
	// Present flags
	//---------------------
	if( pMatchOptions->ePresentFlags == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->pp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	else
		pOptimalDeviceSettings->pp.Flags = pDeviceSettingsIn->pp.Flags;

	//---------------------
	// Refresh rate
	//---------------------
	if( pMatchOptions->eRefreshRate == MANGOMT_IGNORE_INPUT )
		pOptimalDeviceSettings->pp.FullScreen_RefreshRateInHz = 0;
	else
		pOptimalDeviceSettings->pp.FullScreen_RefreshRateInHz = pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz;

	//---------------------
	// Present interval
	//---------------------
	if( pMatchOptions->ePresentInterval == MANGOMT_IGNORE_INPUT )
	{
		// For windowed and fullscreen, default to D3DPRESENT_INTERVAL_DEFAULT
		// which will wait for the vertical retrace period to prevent tearing.
		// For benchmarking, use D3DPRESENT_INTERVAL_DEFAULT  which will
		// will wait not for the vertical retrace period but may introduce tearing.
		pOptimalDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	else
	{
		pOptimalDeviceSettings->pp.PresentationInterval = pDeviceSettingsIn->pp.PresentationInterval;
	}
}


//--------------------------------------------------------------------------------------
// Returns false for any CD3DEnumDeviceSettingsCombo that doesn't meet the preserve 
// match options against the input pDeviceSettingsIn.
//--------------------------------------------------------------------------------------
bool MANGODoesDeviceComboMatchPreserveOptions( CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, 
											 MANGODeviceSettings* pDeviceSettingsIn, 
											 MANGOMatchOptions* pMatchOptions )
{
	//---------------------
	// Adapter ordinal
	//---------------------
	if( pMatchOptions->eAdapterOrdinal == MANGOMT_PRESERVE_INPUT && 
		(pDeviceSettingsCombo->AdapterOrdinal != pDeviceSettingsIn->AdapterOrdinal) )
		return false;

	//---------------------
	// Device type
	//---------------------
	if( pMatchOptions->eDeviceType == MANGOMT_PRESERVE_INPUT && 
		(pDeviceSettingsCombo->DeviceType != pDeviceSettingsIn->DeviceType) )
		return false;

	//---------------------
	// Windowed
	//---------------------
	if( pMatchOptions->eWindowed == MANGOMT_PRESERVE_INPUT && 
		(pDeviceSettingsCombo->Windowed != pDeviceSettingsIn->pp.Windowed) )
		return false;

	//---------------------
	// Adapter format
	//---------------------
	if( pMatchOptions->eAdapterFormat == MANGOMT_PRESERVE_INPUT && 
		(pDeviceSettingsCombo->AdapterFormat != pDeviceSettingsIn->AdapterFormat) )
		return false;

	//---------------------
	// Vertex processing
	//---------------------
	// If keep VP and input has HWVP, then skip if this combo doesn't have HWTL 
	if( pMatchOptions->eVertexProcessing == MANGOMT_PRESERVE_INPUT && 
		((pDeviceSettingsIn->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0) && 
		((pDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0) )
		return false;

	//---------------------
	// Resolution
	//---------------------
	// If keep resolution then check that width and height supported by this combo
	if( pMatchOptions->eResolution == MANGOMT_PRESERVE_INPUT )
	{
		bool bFound = false;
		for( int i=0; i< pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); i++ )
		{
			D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt( i );
			if( displayMode.Format != pDeviceSettingsCombo->AdapterFormat )
				continue; // Skip this display mode if it doesn't match the combo's adapter format

			if( displayMode.Width == pDeviceSettingsIn->pp.BackBufferWidth &&
				displayMode.Height == pDeviceSettingsIn->pp.BackBufferHeight )
			{
				bFound = true;
				break;
			}
		}

		// If the width and height are not supported by this combo, return false
		if( !bFound )
			return false;
	}

	//---------------------
	// Back buffer format
	//---------------------
	if( pMatchOptions->eBackBufferFormat == MANGOMT_PRESERVE_INPUT && 
		pDeviceSettingsCombo->BackBufferFormat != pDeviceSettingsIn->pp.BackBufferFormat )
		return false;

	//---------------------
	// Back buffer count
	//---------------------
	// No caps for the back buffer count

	//---------------------
	// Multisample
	//---------------------
	if( pMatchOptions->eMultiSample == MANGOMT_PRESERVE_INPUT )
	{
		bool bFound = false;
		for( int i=0; i<pDeviceSettingsCombo->multiSampleTypeList.GetSize(); i++ )
		{
			D3DMULTISAMPLE_TYPE msType = pDeviceSettingsCombo->multiSampleTypeList.GetAt(i);
			DWORD msQuality  = pDeviceSettingsCombo->multiSampleQualityList.GetAt(i);

			if( msType == pDeviceSettingsIn->pp.MultiSampleType &&
				msQuality >= pDeviceSettingsIn->pp.MultiSampleQuality )
			{
				bFound = true;
				break;
			}
		}

		// If multisample type/quality not supported by this combo, then return false
		if( !bFound )
			return false;
	}

	//---------------------
	// Swap effect
	//---------------------
	// No caps for swap effects

	//---------------------
	// Depth stencil 
	//---------------------
	// If keep depth stencil format then check that the depth stencil format is supported by this combo
	if( pMatchOptions->eDepthFormat == MANGOMT_PRESERVE_INPUT &&
		pMatchOptions->eStencilFormat == MANGOMT_PRESERVE_INPUT )
	{
		if( pDeviceSettingsIn->pp.AutoDepthStencilFormat != D3DFMT_UNKNOWN &&
			!pDeviceSettingsCombo->depthStencilFormatList.Contains( pDeviceSettingsIn->pp.AutoDepthStencilFormat ) )
			return false;
	}

	// If keep depth format then check that the depth format is supported by this combo
	if( pMatchOptions->eDepthFormat == MANGOMT_PRESERVE_INPUT &&
		pDeviceSettingsIn->pp.AutoDepthStencilFormat != D3DFMT_UNKNOWN )
	{
		bool bFound = false;
		UINT dwDepthBits = MANGODepthBits( pDeviceSettingsIn->pp.AutoDepthStencilFormat );
		for( int i=0; i<pDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++ )
		{
			D3DFORMAT depthStencilFmt = pDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
			UINT dwCurDepthBits = MANGODepthBits( depthStencilFmt );
			if( dwCurDepthBits - dwDepthBits == 0)
				bFound = true;
		}

		if( !bFound )
			return false;
	}

	// If keep depth format then check that the depth format is supported by this combo
	if( pMatchOptions->eStencilFormat == MANGOMT_PRESERVE_INPUT &&
		pDeviceSettingsIn->pp.AutoDepthStencilFormat != D3DFMT_UNKNOWN )
	{
		bool bFound = false;
		UINT dwStencilBits = MANGOStencilBits( pDeviceSettingsIn->pp.AutoDepthStencilFormat );
		for( int i=0; i<pDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++ )
		{
			D3DFORMAT depthStencilFmt = pDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
			UINT dwCurStencilBits = MANGOStencilBits( depthStencilFmt );
			if( dwCurStencilBits - dwStencilBits == 0)
				bFound = true;
		}

		if( !bFound )
			return false;
	}

	//---------------------
	// Present flags
	//---------------------
	// No caps for the present flags

	//---------------------
	// Refresh rate
	//---------------------
	// If keep refresh rate then check that the resolution is supported by this combo
	if( pMatchOptions->eRefreshRate == MANGOMT_PRESERVE_INPUT )
	{
		bool bFound = false;
		for( int i=0; i<pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); i++ )
		{
			D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt( i );
			if( displayMode.Format != pDeviceSettingsCombo->AdapterFormat )
				continue;
			if( displayMode.RefreshRate == pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz )
			{
				bFound = true;
				break;
			}
		}

		// If refresh rate not supported by this combo, then return false
		if( !bFound )
			return false;
	}

	//---------------------
	// Present interval
	//---------------------
	// If keep present interval then check that the present interval is supported by this combo
	if( pMatchOptions->ePresentInterval == MANGOMT_PRESERVE_INPUT &&
		!pDeviceSettingsCombo->presentIntervalList.Contains( pDeviceSettingsIn->pp.PresentationInterval ) )
		return false;

	return true;
}


//--------------------------------------------------------------------------------------
// Returns a ranking number that describes how closely this device 
// combo matches the optimal combo based on the match options and the optimal device settings
//--------------------------------------------------------------------------------------
float MANGORankDeviceCombo( CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo, 
						  MANGODeviceSettings* pOptimalDeviceSettings,
						  D3DDISPLAYMODE* pAdapterDesktopDisplayMode )
{
	float fCurRanking = 0.0f; 

	// Arbitrary weights.  Gives preference to the ordinal, device type, and windowed
	const float fAdapterOrdinalWeight   = 1000.0f;
	const float fDeviceTypeWeight       = 100.0f;
	const float fWindowWeight           = 10.0f;
	const float fAdapterFormatWeight    = 1.0f;
	const float fVertexProcessingWeight = 1.0f;
	const float fResolutionWeight       = 1.0f;
	const float fBackBufferFormatWeight = 1.0f;
	const float fMultiSampleWeight      = 1.0f;
	const float fDepthStencilWeight     = 1.0f;
	const float fRefreshRateWeight      = 1.0f;
	const float fPresentIntervalWeight  = 1.0f;

	//---------------------
	// Adapter ordinal
	//---------------------
	if( pDeviceSettingsCombo->AdapterOrdinal == pOptimalDeviceSettings->AdapterOrdinal )
		fCurRanking += fAdapterOrdinalWeight;

	//---------------------
	// Device type
	//---------------------
	if( pDeviceSettingsCombo->DeviceType == pOptimalDeviceSettings->DeviceType )
		fCurRanking += fDeviceTypeWeight;
	// Slightly prefer HAL 
	if( pDeviceSettingsCombo->DeviceType == D3DDEVTYPE_HAL )
		fCurRanking += 0.1f; 

	//---------------------
	// Windowed
	//---------------------
	if( pDeviceSettingsCombo->Windowed == pOptimalDeviceSettings->pp.Windowed )
		fCurRanking += fWindowWeight;

	//---------------------
	// Adapter format
	//---------------------
	if( pDeviceSettingsCombo->AdapterFormat == pOptimalDeviceSettings->AdapterFormat )
	{
		fCurRanking += fAdapterFormatWeight;
	}
	else
	{
		int nBitDepthDelta = abs( (long) MANGOColorChannelBits(pDeviceSettingsCombo->AdapterFormat) -
			(long) MANGOColorChannelBits(pOptimalDeviceSettings->AdapterFormat) );
		float fScale = __max(0.9f - (float)nBitDepthDelta*0.2f, 0.0f);
		fCurRanking += fScale * fAdapterFormatWeight;
	}

	if( !pDeviceSettingsCombo->Windowed )
	{
		// Slightly prefer when it matches the desktop format or is D3DFMT_X8R8G8B8
		bool bAdapterOptimalMatch;
		if( MANGOColorChannelBits(pAdapterDesktopDisplayMode->Format) >= 8 )
			bAdapterOptimalMatch = (pDeviceSettingsCombo->AdapterFormat == pAdapterDesktopDisplayMode->Format);
		else
			bAdapterOptimalMatch = (pDeviceSettingsCombo->AdapterFormat == D3DFMT_X8R8G8B8);

		if( bAdapterOptimalMatch )
			fCurRanking += 0.1f;
	}

	//---------------------
	// Vertex processing
	//---------------------
	if( (pOptimalDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 || 
		(pOptimalDeviceSettings->BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0 )
	{
		if( (pDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0 )
			fCurRanking += fVertexProcessingWeight;
	}
	// Slightly prefer HW T&L
	if( (pDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0 )
		fCurRanking += 0.1f;

	//---------------------
	// Resolution
	//---------------------
	bool bResolutionFound = false;
	for( int idm = 0; idm < pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); idm++ )
	{
		D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt( idm );
		if( displayMode.Format != pDeviceSettingsCombo->AdapterFormat )
			continue;
		if( displayMode.Width == pOptimalDeviceSettings->pp.BackBufferWidth &&
			displayMode.Height == pOptimalDeviceSettings->pp.BackBufferHeight )
			bResolutionFound = true;
	}
	if( bResolutionFound )
		fCurRanking += fResolutionWeight;

	//---------------------
	// Back buffer format
	//---------------------
	if( pDeviceSettingsCombo->BackBufferFormat == pOptimalDeviceSettings->pp.BackBufferFormat )
	{
		fCurRanking += fBackBufferFormatWeight;
	}
	else
	{
		int nBitDepthDelta = abs( (long) MANGOColorChannelBits(pDeviceSettingsCombo->BackBufferFormat) -
			(long) MANGOColorChannelBits(pOptimalDeviceSettings->pp.BackBufferFormat) );
		float fScale = __max(0.9f - (float)nBitDepthDelta*0.2f, 0.0f);
		fCurRanking += fScale * fBackBufferFormatWeight;
	}

	// Check if this back buffer format is the same as 
	// the adapter format since this is preferred.
	bool bAdapterMatchesBB = (pDeviceSettingsCombo->BackBufferFormat == pDeviceSettingsCombo->AdapterFormat);
	if( bAdapterMatchesBB )
		fCurRanking += 0.1f;

	//---------------------
	// Back buffer count
	//---------------------
	// No caps for the back buffer count

	//---------------------
	// Multisample
	//---------------------
	bool bMultiSampleFound = false;
	for( int i=0; i<pDeviceSettingsCombo->multiSampleTypeList.GetSize(); i++ )
	{
		D3DMULTISAMPLE_TYPE msType = pDeviceSettingsCombo->multiSampleTypeList.GetAt(i);
		DWORD msQuality  = pDeviceSettingsCombo->multiSampleQualityList.GetAt(i);

		if( msType == pOptimalDeviceSettings->pp.MultiSampleType &&
			msQuality >= pOptimalDeviceSettings->pp.MultiSampleQuality )
		{
			bMultiSampleFound = true;
			break;
		}
	}
	if( bMultiSampleFound )
		fCurRanking += fMultiSampleWeight;

	//---------------------
	// Swap effect
	//---------------------
	// No caps for swap effects

	//---------------------
	// Depth stencil 
	//---------------------
	if( pDeviceSettingsCombo->depthStencilFormatList.Contains( pOptimalDeviceSettings->pp.AutoDepthStencilFormat ) )
		fCurRanking += fDepthStencilWeight;

	//---------------------
	// Present flags
	//---------------------
	// No caps for the present flags

	//---------------------
	// Refresh rate
	//---------------------
	bool bRefreshFound = false;
	for( int idm = 0; idm < pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetSize(); idm++ )
	{
		D3DDISPLAYMODE displayMode = pDeviceSettingsCombo->pAdapterInfo->displayModeList.GetAt( idm );
		if( displayMode.Format != pDeviceSettingsCombo->AdapterFormat )
			continue;
		if( displayMode.RefreshRate == pOptimalDeviceSettings->pp.FullScreen_RefreshRateInHz )
			bRefreshFound = true;
	}
	if( bRefreshFound )
		fCurRanking += fRefreshRateWeight;

	//---------------------
	// Present interval
	//---------------------
	// If keep present interval then check that the present interval is supported by this combo
	if( pDeviceSettingsCombo->presentIntervalList.Contains( pOptimalDeviceSettings->pp.PresentationInterval ) )
		fCurRanking += fPresentIntervalWeight;

	return fCurRanking;
}


//--------------------------------------------------------------------------------------
// Builds valid device settings using the match options, the input device settings, and the 
// best device settings combo found.
//--------------------------------------------------------------------------------------
void MANGOBuildValidDeviceSettings( MANGODeviceSettings* pValidDeviceSettings, 
								  CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, 
								  MANGODeviceSettings* pDeviceSettingsIn, 
								  MANGOMatchOptions* pMatchOptions )
{
	IDirect3D9* pD3D = MANGOGetD3DObject();
	D3DDISPLAYMODE adapterDesktopDisplayMode;
	pD3D->GetAdapterDisplayMode( pBestDeviceSettingsCombo->AdapterOrdinal, &adapterDesktopDisplayMode );

	// For each setting pick the best, taking into account the match options and 
	// what's supported by the device

	//---------------------
	// Adapter Ordinal
	//---------------------
	// Just using pBestDeviceSettingsCombo->AdapterOrdinal

	//---------------------
	// Device Type
	//---------------------
	// Just using pBestDeviceSettingsCombo->DeviceType

	//---------------------
	// Windowed 
	//---------------------
	// Just using pBestDeviceSettingsCombo->Windowed

	//---------------------
	// Adapter Format
	//---------------------
	// Just using pBestDeviceSettingsCombo->AdapterFormat

	//---------------------
	// Vertex processing
	//---------------------
	DWORD dwBestBehaviorFlags = 0;
	if( pMatchOptions->eVertexProcessing == MANGOMT_PRESERVE_INPUT )   
	{
		dwBestBehaviorFlags = pDeviceSettingsIn->BehaviorFlags;
	}
	else if( pMatchOptions->eVertexProcessing == MANGOMT_IGNORE_INPUT )    
	{
		// The framework defaults to HWVP if available otherwise use SWVP
		if ((pBestDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
			dwBestBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			dwBestBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else // if( pMatchOptions->eVertexProcessing == MANGOMT_CLOSEST_TO_INPUT )    
	{
		// Default to input, and fallback to SWVP if HWVP not available 
		dwBestBehaviorFlags = pDeviceSettingsIn->BehaviorFlags;
		if ((pBestDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 && 
			( (dwBestBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 || 
			(dwBestBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0) )
		{
			dwBestBehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
			dwBestBehaviorFlags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;
			dwBestBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}

		// One of these must be selected
		if( (dwBestBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) == 0 &&
			(dwBestBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) == 0 &&
			(dwBestBehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0 )
		{
			if ((pBestDeviceSettingsCombo->pDeviceInfo->Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
				dwBestBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
			else
				dwBestBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
	}

	//---------------------
	// Resolution
	//---------------------
	D3DDISPLAYMODE bestDisplayMode;  
	if( pMatchOptions->eResolution == MANGOMT_PRESERVE_INPUT )   
	{
		bestDisplayMode.Width = pDeviceSettingsIn->pp.BackBufferWidth;
		bestDisplayMode.Height = pDeviceSettingsIn->pp.BackBufferHeight;
	}
	else 
	{
		D3DDISPLAYMODE displayModeIn;  
		if( pMatchOptions->eResolution == MANGOMT_CLOSEST_TO_INPUT &&
			pDeviceSettingsIn )   
		{
			displayModeIn.Width = pDeviceSettingsIn->pp.BackBufferWidth;
			displayModeIn.Height = pDeviceSettingsIn->pp.BackBufferHeight;
		}
		else // if( pMatchOptions->eResolution == MANGOMT_IGNORE_INPUT )   
		{
			if( pBestDeviceSettingsCombo->Windowed )
			{
				// The framework defaults to 640x480 for windowed
				displayModeIn.Width = 640;
				displayModeIn.Height = 480;
			}
			else
			{
				// The framework defaults to desktop resolution for fullscreen to try to avoid slow mode change
				displayModeIn.Width = adapterDesktopDisplayMode.Width;
				displayModeIn.Height = adapterDesktopDisplayMode.Height;
			}
		}

		// Call a helper function to find the closest valid display mode to the optimal 
		MANGOFindValidResolution( pBestDeviceSettingsCombo, displayModeIn, &bestDisplayMode );
	}

	//---------------------
	// Back Buffer Format
	//---------------------
	// Just using pBestDeviceSettingsCombo->BackBufferFormat

	//---------------------
	// Back buffer count
	//---------------------
	UINT bestBackBufferCount;
	if( pMatchOptions->eBackBufferCount == MANGOMT_PRESERVE_INPUT )   
	{
		bestBackBufferCount = pDeviceSettingsIn->pp.BackBufferCount;
	}
	else if( pMatchOptions->eBackBufferCount == MANGOMT_IGNORE_INPUT )   
	{
		// The framework defaults to triple buffering 
		bestBackBufferCount = 2;
	}
	else // if( pMatchOptions->eBackBufferCount == MANGOMT_CLOSEST_TO_INPUT )   
	{
		bestBackBufferCount = pDeviceSettingsIn->pp.BackBufferCount;
		if( bestBackBufferCount > 3 )
			bestBackBufferCount = 3;
		if( bestBackBufferCount < 1 )
			bestBackBufferCount = 1;
	}

	//---------------------
	// Multisample
	//---------------------
	D3DMULTISAMPLE_TYPE bestMultiSampleType;
	DWORD bestMultiSampleQuality;
	if( pDeviceSettingsIn && pDeviceSettingsIn->pp.SwapEffect != D3DSWAPEFFECT_DISCARD )
	{
		// Swap effect is not set to discard so multisampling has to off
		bestMultiSampleType = D3DMULTISAMPLE_NONE;
		bestMultiSampleQuality = 0;
	}
	else
	{
		if( pMatchOptions->eMultiSample == MANGOMT_PRESERVE_INPUT )   
		{
			bestMultiSampleType    = pDeviceSettingsIn->pp.MultiSampleType;
			bestMultiSampleQuality = pDeviceSettingsIn->pp.MultiSampleQuality;
		}
		else if( pMatchOptions->eMultiSample == MANGOMT_IGNORE_INPUT )   
		{
			// Default to no multisampling (always supported)
			bestMultiSampleType = D3DMULTISAMPLE_NONE;
			bestMultiSampleQuality = 0;
		}
		else if( pMatchOptions->eMultiSample == MANGOMT_CLOSEST_TO_INPUT )   
		{
			// Default to no multisampling (always supported)
			bestMultiSampleType = D3DMULTISAMPLE_NONE;
			bestMultiSampleQuality = 0;

			for( int i=0; i < pBestDeviceSettingsCombo->multiSampleTypeList.GetSize(); i++ )
			{
				D3DMULTISAMPLE_TYPE type = pBestDeviceSettingsCombo->multiSampleTypeList.GetAt(i);
				DWORD qualityLevels = pBestDeviceSettingsCombo->multiSampleQualityList.GetAt(i);

				// Check whether supported type is closer to the input than our current best
				if( abs(type - pDeviceSettingsIn->pp.MultiSampleType) < abs(bestMultiSampleType - pDeviceSettingsIn->pp.MultiSampleType) )
				{
					bestMultiSampleType = type; 
					bestMultiSampleQuality = __min( qualityLevels-1, pDeviceSettingsIn->pp.MultiSampleQuality );
				}
			}
		}
		else
		{
			// Error case
			bestMultiSampleType = D3DMULTISAMPLE_NONE;
			bestMultiSampleQuality = 0;
		}
	}

	//---------------------
	// Swap effect
	//---------------------
	D3DSWAPEFFECT bestSwapEffect;
	if( pMatchOptions->eSwapEffect == MANGOMT_PRESERVE_INPUT )   
	{
		bestSwapEffect = pDeviceSettingsIn->pp.SwapEffect;
	}
	else if( pMatchOptions->eSwapEffect == MANGOMT_IGNORE_INPUT )   
	{
		bestSwapEffect = D3DSWAPEFFECT_DISCARD;
	}
	else // if( pMatchOptions->eSwapEffect == MANGOMT_CLOSEST_TO_INPUT )   
	{
		bestSwapEffect = pDeviceSettingsIn->pp.SwapEffect;

		// Swap effect has to be one of these 3
		if( bestSwapEffect != D3DSWAPEFFECT_DISCARD &&
			bestSwapEffect != D3DSWAPEFFECT_FLIP &&
			bestSwapEffect != D3DSWAPEFFECT_COPY )
		{
			bestSwapEffect = D3DSWAPEFFECT_DISCARD;
		}
	}

	//---------------------
	// Depth stencil 
	//---------------------
	D3DFORMAT bestDepthStencilFormat;
	BOOL bestEnableAutoDepthStencil;

	CMangoArray< int > depthStencilRanking;
	depthStencilRanking.SetSize( pBestDeviceSettingsCombo->depthStencilFormatList.GetSize() );

	UINT dwBackBufferBitDepth = MANGOColorChannelBits( pBestDeviceSettingsCombo->BackBufferFormat );       
	UINT dwInputDepthBitDepth = 0;
	if( pDeviceSettingsIn )
		dwInputDepthBitDepth = MANGODepthBits( pDeviceSettingsIn->pp.AutoDepthStencilFormat );

	for( int i=0; i<pBestDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++ )
	{
		D3DFORMAT curDepthStencilFmt = pBestDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
		DWORD dwCurDepthBitDepth = MANGODepthBits( curDepthStencilFmt );
		int nRanking;

		if( pMatchOptions->eDepthFormat == MANGOMT_PRESERVE_INPUT )
		{                       
			// Need to match bit depth of input
			if(dwCurDepthBitDepth == dwInputDepthBitDepth)
				nRanking = 0;
			else
				nRanking = 10000;
		}
		else if( pMatchOptions->eDepthFormat == MANGOMT_IGNORE_INPUT )
		{
			// Prefer match of backbuffer bit depth
			nRanking = abs((int)dwCurDepthBitDepth - (int)dwBackBufferBitDepth*4);
		}
		else // if( pMatchOptions->eDepthFormat == MANGOMT_CLOSEST_TO_INPUT )
		{
			// Prefer match of input depth format bit depth
			nRanking = abs((int)dwCurDepthBitDepth - (int)dwInputDepthBitDepth);
		}

		depthStencilRanking.Add( nRanking );
	}

	UINT dwInputStencilBitDepth = 0;
	if( pDeviceSettingsIn )
		dwInputStencilBitDepth = MANGOStencilBits( pDeviceSettingsIn->pp.AutoDepthStencilFormat );

	for( int i=0; i<pBestDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++ )
	{
		D3DFORMAT curDepthStencilFmt = pBestDeviceSettingsCombo->depthStencilFormatList.GetAt(i);
		int nRanking = depthStencilRanking.GetAt(i);
		DWORD dwCurStencilBitDepth = MANGOStencilBits( curDepthStencilFmt );

		if( pMatchOptions->eStencilFormat == MANGOMT_PRESERVE_INPUT )
		{                       
			// Need to match bit depth of input
			if(dwCurStencilBitDepth == dwInputStencilBitDepth)
				nRanking += 0;
			else
				nRanking += 10000;
		}
		else if( pMatchOptions->eStencilFormat == MANGOMT_IGNORE_INPUT )
		{
			// Prefer 0 stencil bit depth
			nRanking += dwCurStencilBitDepth;
		}
		else // if( pMatchOptions->eStencilFormat == MANGOMT_CLOSEST_TO_INPUT )
		{
			// Prefer match of input stencil format bit depth
			nRanking += abs((int)dwCurStencilBitDepth - (int)dwInputStencilBitDepth);
		}

		depthStencilRanking.SetAt( i, nRanking );
	}

	int nBestRanking = 100000;
	int nBestIndex = -1;
	for( int i=0; i<pBestDeviceSettingsCombo->depthStencilFormatList.GetSize(); i++ )
	{
		int nRanking = depthStencilRanking.GetAt(i);
		if( nRanking < nBestRanking )
		{
			nBestRanking = nRanking;
			nBestIndex = i;
		}
	}

	if( nBestIndex >= 0 )
	{
		bestDepthStencilFormat = pBestDeviceSettingsCombo->depthStencilFormatList.GetAt(nBestIndex);
		bestEnableAutoDepthStencil = true;
	}
	else
	{
		bestDepthStencilFormat = D3DFMT_UNKNOWN;
		bestEnableAutoDepthStencil = false;
	}


	//---------------------
	// Present flags
	//---------------------
	DWORD dwBestFlags;
	if( pMatchOptions->ePresentFlags == MANGOMT_PRESERVE_INPUT )   
	{
		dwBestFlags = pDeviceSettingsIn->pp.Flags;
	}
	else if( pMatchOptions->ePresentFlags == MANGOMT_IGNORE_INPUT )   
	{
		dwBestFlags = 0;
		if( bestEnableAutoDepthStencil )
			dwBestFlags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;            
	}
	else // if( pMatchOptions->ePresentFlags == MANGOMT_CLOSEST_TO_INPUT )   
	{
		dwBestFlags = pDeviceSettingsIn->pp.Flags;
		if( bestEnableAutoDepthStencil )
			dwBestFlags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	}

	//---------------------
	// Refresh rate
	//---------------------
	if( pBestDeviceSettingsCombo->Windowed )
	{
		// Must be 0 for windowed
		bestDisplayMode.RefreshRate = 0;
	}
	else
	{
		if( pMatchOptions->eRefreshRate == MANGOMT_PRESERVE_INPUT )   
		{
			bestDisplayMode.RefreshRate = pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz;
		}
		else 
		{
			UINT refreshRateMatch;
			if( pMatchOptions->eRefreshRate == MANGOMT_CLOSEST_TO_INPUT )   
			{
				refreshRateMatch = pDeviceSettingsIn->pp.FullScreen_RefreshRateInHz;
			}
			else // if( pMatchOptions->eRefreshRate == MANGOMT_IGNORE_INPUT )   
			{
				refreshRateMatch = adapterDesktopDisplayMode.RefreshRate;
			}

			bestDisplayMode.RefreshRate = 0;

			if( refreshRateMatch != 0 )
			{
				int nBestRefreshRanking = 100000;
				CMangoArray<D3DDISPLAYMODE>* pDisplayModeList = &pBestDeviceSettingsCombo->pAdapterInfo->displayModeList;
				for( int iDisplayMode=0; iDisplayMode<pDisplayModeList->GetSize(); iDisplayMode++ )
				{
					D3DDISPLAYMODE displayMode = pDisplayModeList->GetAt(iDisplayMode);                
					if( displayMode.Format != pBestDeviceSettingsCombo->AdapterFormat || 
						displayMode.Height != bestDisplayMode.Height ||
						displayMode.Width != bestDisplayMode.Width )
						continue; // Skip display modes that don't match 

					// Find the delta between the current refresh rate and the optimal refresh rate 
					int nCurRanking = abs((int)displayMode.RefreshRate - (int)refreshRateMatch);

					if( nCurRanking < nBestRefreshRanking )
					{
						bestDisplayMode.RefreshRate = displayMode.RefreshRate;
						nBestRefreshRanking = nCurRanking;

						// Stop if perfect match found
						if( nBestRefreshRanking == 0 )
							break;
					}
				}
			}
		}
	}

	//---------------------
	// Present interval
	//---------------------
	UINT bestPresentInterval;
	if( pMatchOptions->ePresentInterval == MANGOMT_PRESERVE_INPUT )   
	{
		bestPresentInterval = pDeviceSettingsIn->pp.PresentationInterval;
	}
	else if( pMatchOptions->ePresentInterval == MANGOMT_IGNORE_INPUT )   
	{
		// For windowed and fullscreen, default to D3DPRESENT_INTERVAL_DEFAULT
		// which will wait for the vertical retrace period to prevent tearing.
		// For benchmarking, use D3DPRESENT_INTERVAL_DEFAULT  which will
		// will wait not for the vertical retrace period but may introduce tearing.
		bestPresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	else // if( pMatchOptions->ePresentInterval == MANGOMT_CLOSEST_TO_INPUT )   
	{
		if( pBestDeviceSettingsCombo->presentIntervalList.Contains( pDeviceSettingsIn->pp.PresentationInterval ) )
		{
			bestPresentInterval = pDeviceSettingsIn->pp.PresentationInterval;
		}
		else
		{
			bestPresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
	}

	// Fill the device settings struct
	ZeroMemory( pValidDeviceSettings, sizeof(MANGODeviceSettings) );
	pValidDeviceSettings->AdapterOrdinal                 = pBestDeviceSettingsCombo->AdapterOrdinal;
	pValidDeviceSettings->DeviceType                     = pBestDeviceSettingsCombo->DeviceType;
	pValidDeviceSettings->AdapterFormat                  = pBestDeviceSettingsCombo->AdapterFormat;
	pValidDeviceSettings->BehaviorFlags                  = dwBestBehaviorFlags;
	pValidDeviceSettings->pp.BackBufferWidth             = bestDisplayMode.Width;
	pValidDeviceSettings->pp.BackBufferHeight            = bestDisplayMode.Height;
	pValidDeviceSettings->pp.BackBufferFormat            = pBestDeviceSettingsCombo->BackBufferFormat;
	pValidDeviceSettings->pp.BackBufferCount             = bestBackBufferCount;
	pValidDeviceSettings->pp.MultiSampleType             = bestMultiSampleType;  
	pValidDeviceSettings->pp.MultiSampleQuality          = bestMultiSampleQuality;
	pValidDeviceSettings->pp.SwapEffect                  = bestSwapEffect;
	pValidDeviceSettings->pp.hDeviceWindow               = pBestDeviceSettingsCombo->Windowed ? MANGOGetHWNDDeviceWindowed() : MANGOGetHWNDDeviceFullScreen();
	pValidDeviceSettings->pp.Windowed                    = pBestDeviceSettingsCombo->Windowed;
	pValidDeviceSettings->pp.EnableAutoDepthStencil      = bestEnableAutoDepthStencil;  
	pValidDeviceSettings->pp.AutoDepthStencilFormat      = bestDepthStencilFormat;
	pValidDeviceSettings->pp.Flags                       = dwBestFlags;                   
	pValidDeviceSettings->pp.FullScreen_RefreshRateInHz  = bestDisplayMode.RefreshRate;
	pValidDeviceSettings->pp.PresentationInterval        = bestPresentInterval;
}


//--------------------------------------------------------------------------------------
// Internal helper function to find the closest allowed display mode to the optimal 
//--------------------------------------------------------------------------------------
HRESULT MANGOFindValidResolution( CD3DEnumDeviceSettingsCombo* pBestDeviceSettingsCombo, 
								D3DDISPLAYMODE displayModeIn, D3DDISPLAYMODE* pBestDisplayMode )
{
	D3DDISPLAYMODE bestDisplayMode;
	ZeroMemory( &bestDisplayMode, sizeof(D3DDISPLAYMODE) );

	if( pBestDeviceSettingsCombo->Windowed )
	{
		// In windowed mode, all resolutions are valid but restritions still apply 
		// on the size of the window.  See MANGOChangeDevice() for details
		*pBestDisplayMode = displayModeIn;
	}
	else
	{
		int nBestRanking = 100000;
		int nCurRanking;
		CMangoArray<D3DDISPLAYMODE>* pDisplayModeList = &pBestDeviceSettingsCombo->pAdapterInfo->displayModeList;
		for( int iDisplayMode=0; iDisplayMode<pDisplayModeList->GetSize(); iDisplayMode++ )
		{
			D3DDISPLAYMODE displayMode = pDisplayModeList->GetAt(iDisplayMode);

			// Skip display modes that don't match the combo's adapter format
			if( displayMode.Format != pBestDeviceSettingsCombo->AdapterFormat )
				continue;

			// Find the delta between the current width/height and the optimal width/height
			nCurRanking = abs((int)displayMode.Width - (int)displayModeIn.Width) + 
				abs((int)displayMode.Height- (int)displayModeIn.Height);

			if( nCurRanking < nBestRanking )
			{
				bestDisplayMode = displayMode;
				nBestRanking = nCurRanking;

				// Stop if perfect match found
				if( nBestRanking == 0 )
					break;
			}
		}

		if( bestDisplayMode.Width == 0 )
		{
			*pBestDisplayMode = displayModeIn;
			return E_FAIL; // No valid display modes found
		}

		*pBestDisplayMode = bestDisplayMode;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Internal helper function to return the adapter format from the first device settings 
// combo that matches the passed adapter ordinal, device type, backbuffer format, and windowed.  
//--------------------------------------------------------------------------------------
HRESULT MANGOFindAdapterFormat( UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT BackBufferFormat, 
							  BOOL Windowed, D3DFORMAT* pAdapterFormat )
{
	CD3DEnumeration* pd3dEnum = MANGOPrepareEnumerationObject();
	CD3DEnumDeviceInfo* pDeviceInfo = pd3dEnum->GetDeviceInfo( AdapterOrdinal, DeviceType );
	if( pDeviceInfo )
	{
		for( int iDeviceCombo=0; iDeviceCombo<pDeviceInfo->deviceSettingsComboList.GetSize(); iDeviceCombo++ )
		{
			CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pDeviceInfo->deviceSettingsComboList.GetAt(iDeviceCombo);
			if( pDeviceSettingsCombo->BackBufferFormat == BackBufferFormat &&
				pDeviceSettingsCombo->Windowed == Windowed )
			{
				// Return the adapter format from the first match
				*pAdapterFormat = pDeviceSettingsCombo->AdapterFormat;
				return S_OK;
			}
		}
	}

	*pAdapterFormat = BackBufferFormat;
	return E_FAIL;
}


//--------------------------------------------------------------------------------------
// Change to a Direct3D device created from the device settings or passed in.
// The framework will only reset if the device is similar to the previous device 
// otherwise it will cleanup the previous device (if there is one) and recreate the 
// scene using the app's device callbacks.
//--------------------------------------------------------------------------------------
HRESULT MANGOChangeDevice( MANGODeviceSettings* pNewDeviceSettings, IDirect3DDevice9* pd3dDeviceFromApp, bool bForceRecreate, bool bClipWindowToSingleAdapter )
{
	HRESULT hr;
	MANGODeviceSettings* pOldDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();

	if( MANGOGetD3DObject() == NULL )
		return S_FALSE;

	// Make a copy of the pNewDeviceSettings on the heap
	MANGODeviceSettings* pNewDeviceSettingsOnHeap = new MANGODeviceSettings;
	if( pNewDeviceSettingsOnHeap == NULL )
		return E_OUTOFMEMORY;
	memcpy( pNewDeviceSettingsOnHeap, pNewDeviceSettings, sizeof(MANGODeviceSettings) );
	pNewDeviceSettings = pNewDeviceSettingsOnHeap;

	// If the ModifyDeviceSettings callback is non-NULL, then call it to let the app 
	// change the settings or reject the device change by returning false.
	LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings = GetMANGOState().GetModifyDeviceSettingsFunc();
	if( pCallbackModifyDeviceSettings )
	{
		D3DCAPS9 caps;
		IDirect3D9* pD3D = MANGOGetD3DObject();
		pD3D->GetDeviceCaps( pNewDeviceSettings->AdapterOrdinal, pNewDeviceSettings->DeviceType, &caps );

		bool bContinue = pCallbackModifyDeviceSettings( pNewDeviceSettings, &caps, GetMANGOState().GetModifyDeviceSettingsFuncUserContext() );
		if( !bContinue )
		{
			// The app rejected the device change by returning false, so just use the current device if there is one.
			if( pOldDeviceSettings == NULL )
				MANGODisplayErrorMessage( MANGOERR_NOCOMPATIBLEDEVICES );
			SAFE_DELETE( pNewDeviceSettings );
			return E_ABORT;
		}
		if( GetMANGOState().GetD3D() == NULL )
		{
			SAFE_DELETE( pNewDeviceSettings );
			return S_FALSE;
		}
	}

	GetMANGOState().SetCurrentDeviceSettings( pNewDeviceSettings );

	MANGOPause( true, true );

	// When a WM_SIZE message is received, it calls MANGOCheckForWindowSizeChange().
	// A WM_SIZE message might be sent when adjusting the window, so tell 
	// MANGOCheckForWindowSizeChange() to ignore size changes temporarily
	GetMANGOState().SetIgnoreSizeChange( true );

	// Update thread safety on/off depending on Direct3D device's thread safety
	g_bThreadSafe = ((pNewDeviceSettings->BehaviorFlags & D3DCREATE_MULTITHREADED) != 0 );

	// Only apply the cmd line overrides if this is the first device created
	// and MANGOSetDevice() isn't used
	if( NULL == pd3dDeviceFromApp && NULL == pOldDeviceSettings )
	{
		// Updates the device settings struct based on the cmd line args.  
		// Warning: if the device doesn't support these new settings then CreateDevice() will fail.
		MANGOUpdateDeviceSettingsWithOverrides( pNewDeviceSettings );
	}

	// Take note if the backbuffer width & height are 0 now as they will change after pd3dDevice->Reset()
	bool bKeepCurrentWindowSize = false;
	if( pNewDeviceSettings->pp.BackBufferWidth == 0 && pNewDeviceSettings->pp.BackBufferHeight == 0 )
		bKeepCurrentWindowSize = true;

	//////////////////////////
	// Before reset
	/////////////////////////
	if( pNewDeviceSettings->pp.Windowed )
	{
		// Going to windowed mode

		if( pOldDeviceSettings && !pOldDeviceSettings->pp.Windowed )
		{
			// Going from fullscreen -> windowed
			GetMANGOState().SetFullScreenBackBufferWidthAtModeChange( pOldDeviceSettings->pp.BackBufferWidth );
			GetMANGOState().SetFullScreenBackBufferHeightAtModeChange( pOldDeviceSettings->pp.BackBufferHeight );

			// Restore windowed mode style
			SetWindowLong( MANGOGetHWNDDeviceWindowed(), GWL_STYLE, GetMANGOState().GetWindowedStyleAtModeChange() );
		}

		// If different device windows are used for windowed mode and fullscreen mode,
		// hide the fullscreen window so that it doesn't obscure the screen.
		if( MANGOGetHWNDDeviceFullScreen() != MANGOGetHWNDDeviceWindowed() )
			ShowWindow( MANGOGetHWNDDeviceFullScreen(), SW_HIDE );

		// If using the same window for windowed and fullscreen mode, reattach menu if one exists
		if( MANGOGetHWNDDeviceFullScreen() == MANGOGetHWNDDeviceWindowed() )
		{
			if( GetMANGOState().GetMenu() != NULL )
				SetMenu( MANGOGetHWNDDeviceWindowed(), GetMANGOState().GetMenu() );
		}
	}
	else 
	{
		// Going to fullscreen mode

		if( pOldDeviceSettings == NULL || (pOldDeviceSettings && pOldDeviceSettings->pp.Windowed) )
		{
			// Transistioning to full screen mode from a standard window so 
			// save current window position/size/style now in case the user toggles to windowed mode later 
			WINDOWPLACEMENT* pwp = GetMANGOState().GetWindowedPlacement();
			ZeroMemory( pwp, sizeof(WINDOWPLACEMENT) );
			pwp->length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement( MANGOGetHWNDDeviceWindowed(), pwp );
			bool bIsTopmost = ( (GetWindowLong(MANGOGetHWNDDeviceWindowed(),GWL_EXSTYLE) & WS_EX_TOPMOST) != 0); 
			GetMANGOState().SetTopmostWhileWindowed( bIsTopmost );
			DWORD dwStyle = GetWindowLong( MANGOGetHWNDDeviceWindowed(), GWL_STYLE );
			dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			GetMANGOState().SetWindowedStyleAtModeChange( dwStyle );
			if( pOldDeviceSettings )
			{
				GetMANGOState().SetWindowBackBufferWidthAtModeChange( pOldDeviceSettings->pp.BackBufferWidth );
				GetMANGOState().SetWindowBackBufferHeightAtModeChange( pOldDeviceSettings->pp.BackBufferHeight );
			}
		}

		// Hide the window to avoid animation of blank windows
		ShowWindow( MANGOGetHWNDDeviceFullScreen(), SW_HIDE );

		// Set FS window style
		SetWindowLong( MANGOGetHWNDDeviceFullScreen(), GWL_STYLE, WS_POPUP|WS_SYSMENU );

		// If using the same window for windowed and fullscreen mode, save and remove menu 
		if( MANGOGetHWNDDeviceFullScreen() == MANGOGetHWNDDeviceWindowed() )
		{
			HMENU hMenu = GetMenu( MANGOGetHWNDDeviceFullScreen() );
			GetMANGOState().SetMenu( hMenu );
			SetMenu( MANGOGetHWNDDeviceFullScreen(), NULL );
		}

		WINDOWPLACEMENT wpFullscreen;
		ZeroMemory( &wpFullscreen, sizeof(WINDOWPLACEMENT) );
		wpFullscreen.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement( MANGOGetHWNDDeviceFullScreen(), &wpFullscreen );
		if( (wpFullscreen.flags & WPF_RESTORETOMAXIMIZED) != 0 )
		{
			// Restore the window to normal if the window was maximized then minimized.  This causes the 
			// WPF_RESTORETOMAXIMIZED flag to be set which will cause SW_RESTORE to restore the 
			// window from minimized to maxmized which isn't what we want
			wpFullscreen.flags &= ~WPF_RESTORETOMAXIMIZED;
			wpFullscreen.showCmd = SW_RESTORE;
			SetWindowPlacement( MANGOGetHWNDDeviceFullScreen(), &wpFullscreen );
		}
	}

	// If AdapterOrdinal and DeviceType are the same, we can just do a Reset().
	// If they've changed, we need to do a complete device tear down/rebuild.
	// Also only allow a reset if pd3dDevice is the same as the current device 
	if( !bForceRecreate && 
		(pd3dDeviceFromApp == NULL || pd3dDeviceFromApp == MANGOGetD3DDevice()) && 
		MANGOGetD3DDevice() &&
		pOldDeviceSettings &&
		pOldDeviceSettings->AdapterOrdinal == pNewDeviceSettings->AdapterOrdinal &&
		pOldDeviceSettings->DeviceType == pNewDeviceSettings->DeviceType &&
		pOldDeviceSettings->BehaviorFlags == pNewDeviceSettings->BehaviorFlags )
	{
		// Reset the Direct3D device and call the app's device callbacks
		hr = MANGOReset3DEnvironment();
		if( FAILED(hr) )
		{
			if( D3DERR_DEVICELOST == hr )
			{
				// The device is lost, just mark it as so and continue on with 
				// capturing the state and resizing the window/etc.
				GetMANGOState().SetDeviceLost( true );
			}
			else if( MANGOERR_RESETTINGDEVICEOBJECTS == hr || 
				MANGOERR_MEDIANOTFOUND == hr )
			{
				// Something bad happened in the app callbacks
				SAFE_DELETE( pOldDeviceSettings );
				MANGODisplayErrorMessage( hr );
				MANGOShutdown();
				return hr;
			}
			else // MANGOERR_RESETTINGDEVICE
			{
				// Reset failed and the device wasn't lost and it wasn't the apps fault, 
				// so recreate the device to try to recover
				GetMANGOState().SetCurrentDeviceSettings( pOldDeviceSettings );
				if( FAILED( MANGOChangeDevice( pNewDeviceSettings, pd3dDeviceFromApp, true, bClipWindowToSingleAdapter ) ) )
				{
					// If that fails, then shutdown
					SAFE_DELETE( pOldDeviceSettings );
					MANGOShutdown();
					return MANGOERR_CREATINGDEVICE;
				}
				else
				{
					SAFE_DELETE( pOldDeviceSettings );
					return S_OK;
				}
			}
		}
	}
	else
	{
		// Cleanup if not first device created
		if( pOldDeviceSettings )
			MANGOCleanup3DEnvironment( false );

		// Create the D3D device and call the app's device callbacks
		hr = MANGOCreate3DEnvironment( pd3dDeviceFromApp );
		if( FAILED(hr) )
		{
			SAFE_DELETE( pOldDeviceSettings );
			MANGOCleanup3DEnvironment();
			MANGODisplayErrorMessage( hr );
			MANGOPause( false, false );
			GetMANGOState().SetIgnoreSizeChange( false );
			return hr;
		}
	}

	// Enable/disable StickKeys shortcut, ToggleKeys shortcut, FilterKeys shortcut, and Windows key 
	// to prevent accidental task switching
	MANGOAllowShortcutKeys( ( pNewDeviceSettings->pp.Windowed  ) ? GetMANGOState().GetAllowShortcutKeysWhenWindowed() : GetMANGOState().GetAllowShortcutKeysWhenFullscreen() );

	IDirect3D9* pD3D = MANGOGetD3DObject();
	HMONITOR hAdapterMonitor = pD3D->GetAdapterMonitor( pNewDeviceSettings->AdapterOrdinal );
	GetMANGOState().SetAdapterMonitor( hAdapterMonitor );

	// Update the device stats text
	MANGOUpdateStaticFrameStats();

	if( pOldDeviceSettings && !pOldDeviceSettings->pp.Windowed && pNewDeviceSettings->pp.Windowed )
	{
		// Going from fullscreen -> windowed

		// Restore the show state, and positions/size of the window to what it was
		// It is important to adjust the window size 
		// after resetting the device rather than beforehand to ensure 
		// that the monitor resolution is correct and does not limit the size of the new window.
		WINDOWPLACEMENT* pwp = GetMANGOState().GetWindowedPlacement();
		SetWindowPlacement( MANGOGetHWNDDeviceWindowed(), pwp );

		// Also restore the z-order of window to previous state
		HWND hWndInsertAfter = GetMANGOState().GetTopmostWhileWindowed() ? HWND_TOPMOST : HWND_NOTOPMOST;
		SetWindowPos( MANGOGetHWNDDeviceWindowed(), hWndInsertAfter, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE );
	}

	// Check to see if the window needs to be resized.  
	// Handle cases where the window is minimized and maxmimized as well.
	bool bNeedToResize = false;
	if( pNewDeviceSettings->pp.Windowed && // only resize if in windowed mode
		!bKeepCurrentWindowSize )          // only resize if pp.BackbufferWidth/Height were not 0
	{
		UINT nClientWidth;
		UINT nClientHeight;    
		if( IsIconic(MANGOGetHWNDDeviceWindowed()) )
		{
			// Window is currently minimized. To tell if it needs to resize, 
			// get the client rect of window when its restored the 
			// hard way using GetWindowPlacement()
			WINDOWPLACEMENT wp;
			ZeroMemory( &wp, sizeof(WINDOWPLACEMENT) );
			wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement( MANGOGetHWNDDeviceWindowed(), &wp );

			if( (wp.flags & WPF_RESTORETOMAXIMIZED) != 0 && wp.showCmd == SW_SHOWMINIMIZED )
			{
				// WPF_RESTORETOMAXIMIZED means that when the window is restored it will
				// be maximized.  So maximize the window temporarily to get the client rect 
				// when the window is maximized.  GetSystemMetrics( SM_CXMAXIMIZED ) will give this 
				// information if the window is on the primary but this will work on multimon.
				ShowWindow( MANGOGetHWNDDeviceWindowed(), SW_RESTORE );
				RECT rcClient;
				GetClientRect( MANGOGetHWNDDeviceWindowed(), &rcClient );
				nClientWidth  = (UINT)(rcClient.right - rcClient.left);
				nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
				ShowWindow( MANGOGetHWNDDeviceWindowed(), SW_MINIMIZE );
			}
			else
			{
				// Use wp.rcNormalPosition to get the client rect, but wp.rcNormalPosition 
				// includes the window frame so subtract it
				RECT rcFrame = {0};
				AdjustWindowRect( &rcFrame, GetMANGOState().GetWindowedStyleAtModeChange(), GetMANGOState().GetMenu() != NULL );
				LONG nFrameWidth = rcFrame.right - rcFrame.left;
				LONG nFrameHeight = rcFrame.bottom - rcFrame.top;
				nClientWidth  = (UINT)(wp.rcNormalPosition.right - wp.rcNormalPosition.left - nFrameWidth);
				nClientHeight = (UINT)(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top - nFrameHeight);
			}
		}
		else
		{
			// Window is restored or maximized so just get its client rect
			RECT rcClient;
			GetClientRect( MANGOGetHWNDDeviceWindowed(), &rcClient );
			nClientWidth  = (UINT)(rcClient.right - rcClient.left);
			nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
		}

		// Now that we know the client rect, compare it against the back buffer size
		// to see if the client rect is already the right size
		if( nClientWidth  != pNewDeviceSettings->pp.BackBufferWidth ||
			nClientHeight != pNewDeviceSettings->pp.BackBufferHeight )
		{
			bNeedToResize = true;
		}       

		if( bClipWindowToSingleAdapter && !IsIconic(MANGOGetHWNDDeviceWindowed()) )
		{
			// Get the rect of the monitor attached to the adapter
			MONITORINFO miAdapter;
			miAdapter.cbSize = sizeof(MONITORINFO);
			HMONITOR hAdapterMonitor = MANGOGetD3DObject()->GetAdapterMonitor( pNewDeviceSettings->AdapterOrdinal );
			MANGOGetMonitorInfo( hAdapterMonitor, &miAdapter );
			HMONITOR hWindowMonitor = MANGOMonitorFromWindow( MANGOGetHWND(), MONITOR_DEFAULTTOPRIMARY );

			// Get the rect of the window
			RECT rcWindow;
			GetWindowRect( MANGOGetHWNDDeviceWindowed(), &rcWindow );

			// Check if the window rect is fully inside the adapter's vitural screen rect
			if( (rcWindow.left   < miAdapter.rcWork.left  ||
				rcWindow.right  > miAdapter.rcWork.right ||
				rcWindow.top    < miAdapter.rcWork.top   ||
				rcWindow.bottom > miAdapter.rcWork.bottom) )
			{
				if( hWindowMonitor == hAdapterMonitor && IsZoomed(MANGOGetHWNDDeviceWindowed()) )
				{
					// If the window is maximized and on the same monitor as the adapter, then 
					// no need to clip to single adapter as the window is already clipped 
					// even though the rcWindow rect is outside of the miAdapter.rcWork
				}
				else
				{
					bNeedToResize = true;
				}
			}
		}
	}

	// Only resize window if needed 
	if( bNeedToResize ) 
	{
		// Need to resize, so if window is maximized or minimized then restore the window
		if( IsIconic(MANGOGetHWNDDeviceWindowed()) ) 
			ShowWindow( MANGOGetHWNDDeviceWindowed(), SW_RESTORE );
		if( IsZoomed(MANGOGetHWNDDeviceWindowed()) ) // doing the IsIconic() check first also handles the WPF_RESTORETOMAXIMIZED case
			ShowWindow( MANGOGetHWNDDeviceWindowed(), SW_RESTORE );

		if( bClipWindowToSingleAdapter )
		{
			// Get the rect of the monitor attached to the adapter
			MONITORINFO miAdapter;
			miAdapter.cbSize = sizeof(MONITORINFO);
			MANGOGetMonitorInfo( MANGOGetD3DObject()->GetAdapterMonitor( pNewDeviceSettings->AdapterOrdinal ), &miAdapter );

			// Get the rect of the monitor attached to the window
			MONITORINFO miWindow;
			miWindow.cbSize = sizeof(MONITORINFO);
			MANGOGetMonitorInfo( MANGOMonitorFromWindow( MANGOGetHWND(), MONITOR_DEFAULTTOPRIMARY ), &miWindow );

			// Do something reasonable if the BackBuffer size is greater than the monitor size
			int nAdapterMonitorWidth = miAdapter.rcWork.right - miAdapter.rcWork.left;
			int nAdapterMonitorHeight = miAdapter.rcWork.bottom - miAdapter.rcWork.top;

			int nClientWidth = pNewDeviceSettings->pp.BackBufferWidth;
			int nClientHeight = pNewDeviceSettings->pp.BackBufferHeight;

			// Get the rect of the window
			RECT rcWindow;
			GetWindowRect( MANGOGetHWNDDeviceWindowed(), &rcWindow );

			// Make a window rect with a client rect that is the same size as the backbuffer
			RECT rcResizedWindow;
			rcResizedWindow.left = 0;
			rcResizedWindow.right = nClientWidth;
			rcResizedWindow.top = 0;
			rcResizedWindow.bottom = nClientHeight;
			AdjustWindowRect( &rcResizedWindow, GetWindowLong( MANGOGetHWNDDeviceWindowed(), GWL_STYLE ), GetMANGOState().GetMenu() != NULL );

			int nWindowWidth = rcResizedWindow.right - rcResizedWindow.left;
			int nWindowHeight = rcResizedWindow.bottom - rcResizedWindow.top;

			if( nWindowWidth > nAdapterMonitorWidth )
				nWindowWidth = (nAdapterMonitorWidth - 0);
			if( nWindowHeight > nAdapterMonitorHeight )
				nWindowHeight = (nAdapterMonitorHeight - 0);

			if( rcResizedWindow.left < miAdapter.rcWork.left ||
				rcResizedWindow.top < miAdapter.rcWork.top ||
				rcResizedWindow.right > miAdapter.rcWork.right ||
				rcResizedWindow.bottom > miAdapter.rcWork.bottom )
			{
				int nWindowOffsetX = (nAdapterMonitorWidth - nWindowWidth) / 2;
				int nWindowOffsetY = (nAdapterMonitorHeight - nWindowHeight) / 2;

				rcResizedWindow.left = miAdapter.rcWork.left + nWindowOffsetX;
				rcResizedWindow.top = miAdapter.rcWork.top + nWindowOffsetY;
				rcResizedWindow.right = miAdapter.rcWork.left + nWindowOffsetX + nWindowWidth;
				rcResizedWindow.bottom = miAdapter.rcWork.top + nWindowOffsetY + nWindowHeight;
			}

			// Resize the window.  It is important to adjust the window size 
			// after resetting the device rather than beforehand to ensure 
			// that the monitor resolution is correct and does not limit the size of the new window.
			SetWindowPos( MANGOGetHWNDDeviceWindowed(), 0, rcResizedWindow.left, rcResizedWindow.top, nWindowWidth, nWindowHeight, SWP_NOZORDER );
		}        
		else
		{      
			// Make a window rect with a client rect that is the same size as the backbuffer
			RECT rcWindow = {0};
			rcWindow.right = (long)(pNewDeviceSettings->pp.BackBufferWidth);
			rcWindow.bottom = (long)(pNewDeviceSettings->pp.BackBufferHeight);
			AdjustWindowRect( &rcWindow, GetWindowLong( MANGOGetHWNDDeviceWindowed(), GWL_STYLE ), GetMANGOState().GetMenu() != NULL );

			// Resize the window.  It is important to adjust the window size 
			// after resetting the device rather than beforehand to ensure 
			// that the monitor resolution is correct and does not limit the size of the new window.
			int cx = (int)(rcWindow.right - rcWindow.left);
			int cy = (int)(rcWindow.bottom - rcWindow.top);
			SetWindowPos( MANGOGetHWNDDeviceWindowed(), 0, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOMOVE );
		}

		// Its possible that the new window size is not what we asked for.  
		// No window can be sized larger than the desktop, so see see if the Windows OS resized the 
		// window to something smaller to fit on the desktop.  Also if WM_GETMINMAXINFO
		// will put a limit on the smallest/largest window size.
		RECT rcClient;
		GetClientRect( MANGOGetHWNDDeviceWindowed(), &rcClient );
		UINT nClientWidth  = (UINT)(rcClient.right - rcClient.left);
		UINT nClientHeight = (UINT)(rcClient.bottom - rcClient.top);
		if( nClientWidth  != pNewDeviceSettings->pp.BackBufferWidth ||
			nClientHeight != pNewDeviceSettings->pp.BackBufferHeight )
		{
			// If its different, then resize the backbuffer again.  This time create a backbuffer that matches the 
			// client rect of the current window w/o resizing the window.
			MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
			deviceSettings.pp.BackBufferWidth  = 0; 
			deviceSettings.pp.BackBufferHeight = 0;
			hr = MANGOChangeDevice( &deviceSettings, NULL, false, bClipWindowToSingleAdapter );
			if( FAILED( hr ) )
			{
				SAFE_DELETE( pOldDeviceSettings );
				MANGOCleanup3DEnvironment();
				MANGOPause( false, false );
				GetMANGOState().SetIgnoreSizeChange( false );
				return hr;
			}
		}
	}

	// Make the window visible
	if( !IsWindowVisible( MANGOGetHWND() ) )
		ShowWindow( MANGOGetHWND(), SW_SHOW );

	// Make the window visible
	if( !IsWindowVisible( MANGOGetHWND() ) )
		ShowWindow( MANGOGetHWND(), SW_SHOW );

	// Ensure that the display doesn't power down when fullscreen but does when windowed
	if( !MANGOIsWindowed() )
		SetThreadExecutionState( ES_DISPLAY_REQUIRED | ES_CONTINUOUS ); 
	else
		SetThreadExecutionState( ES_CONTINUOUS );   

	SAFE_DELETE( pOldDeviceSettings );
	GetMANGOState().SetIgnoreSizeChange( false );
	MANGOPause( false, false );
	GetMANGOState().SetDeviceCreated( true );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Low level keyboard hook to disable Windows key to prevent accidental task switching.  
//--------------------------------------------------------------------------------------
LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	if (nCode < 0 || nCode != HC_ACTION)  // do not process message 
		return CallNextHookEx( GetMANGOState().GetKeyboardHook(), nCode, wParam, lParam); 

	bool bEatKeystroke = false;
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
	switch (wParam) 
	{
	case WM_KEYDOWN:  
	case WM_KEYUP:    
		{
			bEatKeystroke = ( !GetMANGOState().GetAllowShortcutKeys() && 
				(p->vkCode == VK_LWIN || p->vkCode == VK_RWIN) );
			break;
		}
	}

	if( bEatKeystroke )
		return 1;
	else
		return CallNextHookEx( GetMANGOState().GetKeyboardHook(), nCode, wParam, lParam );
}



//--------------------------------------------------------------------------------------
// Controls how MANGO behaves when fullscreen and windowed mode with regard to 
// shortcut keys (Windows keys, StickyKeys shortcut, ToggleKeys shortcut, FilterKeys shortcut) 
//--------------------------------------------------------------------------------------
void MANGOSetShortcutKeySettings( bool bAllowWhenFullscreen, bool bAllowWhenWindowed )
{
	GetMANGOState().SetAllowShortcutKeysWhenWindowed( bAllowWhenWindowed );
	GetMANGOState().SetAllowShortcutKeysWhenFullscreen( bAllowWhenFullscreen );

	// MANGOInit() records initial accessibility states so don't change them until then
	if( GetMANGOState().GetMANGOInited() )
	{
		if( MANGOIsWindowed() )
			MANGOAllowShortcutKeys( GetMANGOState().GetAllowShortcutKeysWhenWindowed() );
		else
			MANGOAllowShortcutKeys( GetMANGOState().GetAllowShortcutKeysWhenFullscreen() );
	}
}


//--------------------------------------------------------------------------------------
// Enables/disables Windows keys, and disables or restores the StickyKeys/ToggleKeys/FilterKeys 
// shortcut to help prevent accidental task switching
//--------------------------------------------------------------------------------------
void MANGOAllowShortcutKeys( bool bAllowKeys )
{
	GetMANGOState().SetAllowShortcutKeys( bAllowKeys );

	if( bAllowKeys )
	{
		// Restore StickyKeys/etc to original state and enable Windows key      
		STICKYKEYS sk = GetMANGOState().GetStartupStickyKeys();
		TOGGLEKEYS tk = GetMANGOState().GetStartupToggleKeys();
		FILTERKEYS fk = GetMANGOState().GetStartupFilterKeys();

		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sk, 0);
		SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tk, 0);
		SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, 0);

		// Remove the keyboard hoook when it isn't needed to prevent any slow down of other apps
		if( GetMANGOState().GetKeyboardHook() )
		{
			UnhookWindowsHookEx( GetMANGOState().GetKeyboardHook() );
			GetMANGOState().SetKeyboardHook( NULL );
		}                
	}
	else
	{
		// Set low level keyboard hook if haven't already
		if( GetMANGOState().GetKeyboardHook() == NULL )
		{
			// Set the low-level hook procedure.  Only works on Windows 2000 and above
			OSVERSIONINFO OSVersionInfo;
			OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);
			GetVersionEx(&OSVersionInfo);
			if( OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && OSVersionInfo.dwMajorVersion > 4 )
			{
				HHOOK hKeyboardHook = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0 );
				GetMANGOState().SetKeyboardHook( hKeyboardHook );
			}
		}

		// Disable StickyKeys/etc shortcuts but if the accessibility feature is on, 
		// then leave the settings alone as its probably being usefully used

		STICKYKEYS skOff = GetMANGOState().GetStartupStickyKeys();
		if( (skOff.dwFlags & SKF_STICKYKEYSON) == 0 )
		{
			// Disable the hotkey and the confirmation
			skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
			skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

			SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
		}

		TOGGLEKEYS tkOff = GetMANGOState().GetStartupToggleKeys();
		if( (tkOff.dwFlags & TKF_TOGGLEKEYSON) == 0 )
		{
			// Disable the hotkey and the confirmation
			tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
			tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;

			SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
		}

		FILTERKEYS fkOff = GetMANGOState().GetStartupFilterKeys();
		if( (fkOff.dwFlags & FKF_FILTERKEYSON) == 0 )
		{
			// Disable the hotkey and the confirmation
			fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
			fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;

			SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
		}
	}
}


//--------------------------------------------------------------------------------------
// Updates the device settings struct based on the cmd line args.  
//--------------------------------------------------------------------------------------
void MANGOUpdateDeviceSettingsWithOverrides( MANGODeviceSettings* pDeviceSettings )
{
	if( GetMANGOState().GetOverrideAdapterOrdinal() != -1 )
		pDeviceSettings->AdapterOrdinal = GetMANGOState().GetOverrideAdapterOrdinal();

	if( GetMANGOState().GetOverrideFullScreen() )
		pDeviceSettings->pp.Windowed = false;
	if( GetMANGOState().GetOverrideWindowed() )
		pDeviceSettings->pp.Windowed = true;

	if( GetMANGOState().GetOverrideForceREF() )
		pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
	else if( GetMANGOState().GetOverrideForceHAL() )
		pDeviceSettings->DeviceType = D3DDEVTYPE_HAL;

	if( GetMANGOState().GetOverrideWidth() != 0 )
		pDeviceSettings->pp.BackBufferWidth = GetMANGOState().GetOverrideWidth();
	if( GetMANGOState().GetOverrideHeight() != 0 )
		pDeviceSettings->pp.BackBufferHeight = GetMANGOState().GetOverrideHeight();

	if( GetMANGOState().GetOverrideForcePureHWVP() )
	{
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags |= D3DCREATE_PUREDEVICE;
	}
	else if( GetMANGOState().GetOverrideForceHWVP() )
	{
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		pDeviceSettings->BehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else if( GetMANGOState().GetOverrideForceSWVP() )
	{
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
}


//--------------------------------------------------------------------------------------
// Creates the 3D environment
//--------------------------------------------------------------------------------------
HRESULT MANGOCreate3DEnvironment( IDirect3DDevice9* pd3dDeviceFromApp )
{
	HRESULT hr = S_OK;

	IDirect3DDevice9* pd3dDevice = NULL;
	MANGODeviceSettings* pNewDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();

	// Only create a Direct3D device if one hasn't been supplied by the app
	if( pd3dDeviceFromApp == NULL )
	{
		// Try to create the device with the chosen settings
		IDirect3D9* pD3D = MANGOGetD3DObject();
		hr = pD3D->CreateDevice( pNewDeviceSettings->AdapterOrdinal, pNewDeviceSettings->DeviceType, 
			MANGOGetHWNDFocus(), pNewDeviceSettings->BehaviorFlags,
			&pNewDeviceSettings->pp, &pd3dDevice );
		if( hr == D3DERR_DEVICELOST ) 
		{
			GetMANGOState().SetDeviceLost( true );
			return S_OK;
		}
		else if( FAILED(hr) )
		{
			MANGO_ERR( L"CreateDevice", hr );
			return MANGOERR_CREATINGDEVICE;
		}
	}
	else
	{
		pd3dDeviceFromApp->AddRef();
		pd3dDevice = pd3dDeviceFromApp;
	}

	GetMANGOState().SetD3DDevice( pd3dDevice );

	// If switching to REF, set the exit code to 11.  If switching to HAL and exit code was 11, then set it back to 0.
	if( pNewDeviceSettings->DeviceType == D3DDEVTYPE_REF && GetMANGOState().GetExitCode() == 0 )
		GetMANGOState().SetExitCode(11);
	else if( pNewDeviceSettings->DeviceType == D3DDEVTYPE_HAL && GetMANGOState().GetExitCode() == 11 )
		GetMANGOState().SetExitCode(0);

	// Update back buffer desc before calling app's device callbacks
	MANGOUpdateBackBufferDesc();

	// Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
	MANGOSetupCursor();

	// Update GetMANGOState()'s copy of D3D caps 
	D3DCAPS9* pd3dCaps = GetMANGOState().GetCaps();
	MANGOGetD3DDevice()->GetDeviceCaps( pd3dCaps );

	// Update the device stats text
	CD3DEnumeration* pd3dEnum = MANGOPrepareEnumerationObject();
	CD3DEnumAdapterInfo* pAdapterInfo = pd3dEnum->GetAdapterInfo( pNewDeviceSettings->AdapterOrdinal );
	MANGOUpdateDeviceStats( pNewDeviceSettings->DeviceType, 
		pNewDeviceSettings->BehaviorFlags, 
		&pAdapterInfo->AdapterIdentifier );

	// Call the resource cache created function
	hr = MANGOGetGlobalResourceCache().OnCreateDevice( pd3dDevice );
	if( FAILED(hr) )
		return MANGO_ERR( L"OnCreateDevice", ( hr == MANGOERR_MEDIANOTFOUND ) ? MANGOERR_MEDIANOTFOUND : MANGOERR_CREATINGDEVICEOBJECTS );

	// Call the app's device created callback if non-NULL
	const D3DSURFACE_DESC* pbackBufferSurfaceDesc = MANGOGetBackBufferSurfaceDesc();
	GetMANGOState().SetInsideDeviceCallback( true );
	//回调函数CreatedFunc
	LPMANGOCALLBACKDEVICECREATED pCallbackDeviceCreated = GetMANGOState().GetDeviceCreatedFunc();
	hr = S_OK;
	if( pCallbackDeviceCreated != NULL )
		hr = pCallbackDeviceCreated( MANGOGetD3DDevice(), pbackBufferSurfaceDesc, GetMANGOState().GetDeviceCreatedFuncUserContext() );
	GetMANGOState().SetInsideDeviceCallback( false );
	if( MANGOGetD3DDevice() == NULL ) // Handle MANGOShutdown from inside callback
		return E_FAIL;
	if( FAILED(hr) )  
	{
		MANGO_ERR( L"DeviceCreated callback", hr );        
		return ( hr == MANGOERR_MEDIANOTFOUND ) ? MANGOERR_MEDIANOTFOUND : MANGOERR_CREATINGDEVICEOBJECTS;
	}
	GetMANGOState().SetDeviceObjectsCreated( true );

	// Call the resource cache device reset function
	hr = MANGOGetGlobalResourceCache().OnResetDevice( pd3dDevice );
	if( FAILED(hr) )
		return MANGO_ERR( L"OnResetDevice", MANGOERR_RESETTINGDEVICEOBJECTS );

	// Call the app's device reset callback if non-NULL
	//回调函数ResetFunc
	GetMANGOState().SetInsideDeviceCallback( true );
	LPMANGOCALLBACKDEVICERESET pCallbackDeviceReset = GetMANGOState().GetDeviceResetFunc();
	hr = S_OK;
	if( pCallbackDeviceReset != NULL )
		hr = pCallbackDeviceReset( MANGOGetD3DDevice(), pbackBufferSurfaceDesc, GetMANGOState().GetDeviceResetFuncUserContext() );
	GetMANGOState().SetInsideDeviceCallback( false );
	if( MANGOGetD3DDevice() == NULL ) // Handle MANGOShutdown from inside callback
		return E_FAIL;
	if( FAILED(hr) )
	{
		MANGO_ERR( L"DeviceReset callback", hr );
		return ( hr == MANGOERR_MEDIANOTFOUND ) ? MANGOERR_MEDIANOTFOUND : MANGOERR_RESETTINGDEVICEOBJECTS;
	}
	GetMANGOState().SetDeviceObjectsReset( true );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Resets the 3D environment by:
//      - Calls the device lost callback 
//      - Resets the device
//      - Stores the back buffer description
//      - Sets up the full screen Direct3D cursor if requested
//      - Calls the device reset callback 
//--------------------------------------------------------------------------------------
HRESULT MANGOReset3DEnvironment()
{
	HRESULT hr;

	IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
	assert( pd3dDevice != NULL );

	// Call the app's device lost callback
	//回调函数LostFunc
	if( GetMANGOState().GetDeviceObjectsReset() == true )
	{
		GetMANGOState().SetInsideDeviceCallback( true );
		LPMANGOCALLBACKDEVICELOST pCallbackDeviceLost = GetMANGOState().GetDeviceLostFunc();
		if( pCallbackDeviceLost != NULL )
			pCallbackDeviceLost( GetMANGOState().GetDeviceLostFuncUserContext() );
		GetMANGOState().SetDeviceObjectsReset( false );
		GetMANGOState().SetInsideDeviceCallback( false );

		// Call the resource cache device lost function
		MANGOGetGlobalResourceCache().OnLostDevice();
	}

	// Reset the device
	// 回调函数DeviceSettings
	MANGODeviceSettings* pDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();
	hr = pd3dDevice->Reset( &pDeviceSettings->pp );
	if( FAILED(hr) )  
	{
		if( hr == D3DERR_DEVICELOST )
			return D3DERR_DEVICELOST; // Reset could legitimately fail if the device is lost
		else
			return MANGO_ERR( L"Reset", MANGOERR_RESETTINGDEVICE );
	}

	// Update back buffer desc before calling app's device callbacks
	MANGOUpdateBackBufferDesc();

	// Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
	MANGOSetupCursor();

	hr = MANGOGetGlobalResourceCache().OnResetDevice( pd3dDevice );
	if( FAILED(hr) )
		return MANGO_ERR( L"OnResetDevice", MANGOERR_RESETTINGDEVICEOBJECTS );

	// Call the app's OnDeviceReset callback
	GetMANGOState().SetInsideDeviceCallback( true );
	const D3DSURFACE_DESC* pbackBufferSurfaceDesc = MANGOGetBackBufferSurfaceDesc();
	// 回调函数ResetFunc 
	LPMANGOCALLBACKDEVICERESET pCallbackDeviceReset = GetMANGOState().GetDeviceResetFunc();
	hr = S_OK;
	if( pCallbackDeviceReset != NULL )
		hr = pCallbackDeviceReset( pd3dDevice, pbackBufferSurfaceDesc, GetMANGOState().GetDeviceResetFuncUserContext() );
	GetMANGOState().SetInsideDeviceCallback( false );
	if( FAILED(hr) )
	{
		// If callback failed, cleanup
		MANGO_ERR( L"DeviceResetCallback", hr );
		if( hr != MANGOERR_MEDIANOTFOUND )
			hr = MANGOERR_RESETTINGDEVICEOBJECTS;

		GetMANGOState().SetInsideDeviceCallback( true );
		// 回调函数LostFunc
		LPMANGOCALLBACKDEVICELOST pCallbackDeviceLost = GetMANGOState().GetDeviceLostFunc();
		if( pCallbackDeviceLost != NULL )
			pCallbackDeviceLost( GetMANGOState().GetDeviceLostFuncUserContext() );
		GetMANGOState().SetInsideDeviceCallback( false );

		MANGOGetGlobalResourceCache().OnLostDevice();       
		return hr;
	}

	// Success
	GetMANGOState().SetDeviceObjectsReset( true );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Pauses time or rendering.  Keeps a ref count so pausing can be layered
//--------------------------------------------------------------------------------------
void MANGOPause( bool bPauseTime, bool bPauseRendering )
{
	int nPauseTimeCount = GetMANGOState().GetPauseTimeCount();
	nPauseTimeCount += ( bPauseTime ? +1 : -1 );
	if( nPauseTimeCount < 0 )
		nPauseTimeCount = 0;
	GetMANGOState().SetPauseTimeCount( nPauseTimeCount );

	int nPauseRenderingCount = GetMANGOState().GetPauseRenderingCount();
	nPauseRenderingCount += ( bPauseRendering ? +1 : -1 );
	if( nPauseRenderingCount < 0 )
		nPauseRenderingCount = 0;
	GetMANGOState().SetPauseRenderingCount( nPauseRenderingCount );

	if( nPauseTimeCount > 0 )
	{
		// Stop the scene from animating
		MANGOGetGlobalTimer()->Stop();
	}
	else
	{
		// Restart the timer
		MANGOGetGlobalTimer()->Start();
	}

	GetMANGOState().SetRenderingPaused( nPauseRenderingCount > 0 );
	GetMANGOState().SetTimePaused( nPauseTimeCount > 0 );
}


//--------------------------------------------------------------------------------------
// Checks if the window client rect has changed and if it has, then reset the device
//--------------------------------------------------------------------------------------
void MANGOCheckForWindowSizeChange()
{
	// Skip the check for various reasons
	if( GetMANGOState().GetIgnoreSizeChange() || 
		!GetMANGOState().GetDeviceCreated() || 
		!GetMANGOState().GetCurrentDeviceSettings()->pp.Windowed )
		return;

	RECT rcCurrentClient;
	GetClientRect( MANGOGetHWND(), &rcCurrentClient );

	if( (UINT)rcCurrentClient.right != GetMANGOState().GetCurrentDeviceSettings()->pp.BackBufferWidth ||
		(UINT)rcCurrentClient.bottom != GetMANGOState().GetCurrentDeviceSettings()->pp.BackBufferHeight )
	{
		// A new window size will require a new backbuffer size
		// size, so the device must be reset and the D3D structures updated accordingly.

		// Tell MANGOChangeDevice and D3D to size according to the HWND's client rect
		MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
		deviceSettings.pp.BackBufferWidth  = 0; 
		deviceSettings.pp.BackBufferHeight = 0;
		MANGOChangeDevice( &deviceSettings, NULL, false, false );
	}
}


//--------------------------------------------------------------------------------------
// Handles app's message loop and rendering when idle.  If MANGOCreateDevice*() or MANGOSetDevice() 
// has not already been called, it will call MANGOCreateWindow() with the default parameters.  
//--------------------------------------------------------------------------------------
HRESULT MANGOMainLoop( HACCEL hAccel )
{
	HRESULT hr;

	// Not allowed to call this from inside the device callbacks or reenter
	if( GetMANGOState().GetInsideDeviceCallback() || GetMANGOState().GetInsideMainloop() )
	{
		if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
			GetMANGOState().SetExitCode(1);
		return MANGO_ERR_MSGBOX( L"MANGOMainLoop", E_FAIL );
	}

	GetMANGOState().SetInsideMainloop( true );

	// If MANGOCreateDevice*() or MANGOSetDevice() has not already been called, 
	// then call MANGOCreateDevice() with the default parameters.         
	if( !GetMANGOState().GetDeviceCreated() ) 
	{
		if( GetMANGOState().GetDeviceCreateCalled() )
		{
			if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
				GetMANGOState().SetExitCode(1);
			return E_FAIL; // MANGOCreateDevice() must first succeed for this function to succeed
		}

		hr = MANGOCreateDevice();
		if( FAILED(hr) )
		{
			if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
				GetMANGOState().SetExitCode(1);
			return hr;
		}
	}

	HWND hWnd = MANGOGetHWND();

	// MANGOInit() must have been called and succeeded for this function to proceed
	// MANGOCreateWindow() or MANGOSetWindow() must have been called and succeeded for this function to proceed
	// MANGOCreateDevice() or MANGOCreateDeviceFromSettings() or MANGOSetDevice() must have been called and succeeded for this function to proceed
	if( !GetMANGOState().GetMANGOInited() || !GetMANGOState().GetWindowCreated() || !GetMANGOState().GetDeviceCreated() )
	{
		if( (GetMANGOState().GetExitCode() == 0) || (GetMANGOState().GetExitCode() == 11) )
			GetMANGOState().SetExitCode(1);
		return MANGO_ERR_MSGBOX( L"MANGOMainLoop", E_FAIL );
	}

	// Now we're ready to receive and process Windows messages.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message  )
	{
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

		if( bGotMsg )
		{
			// Translate and dispatch the message
			if( hAccel == NULL || hWnd == NULL || 
				0 == TranslateAccelerator( hWnd, hAccel, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
		else
		{
			// Render a frame during idle time (no messages are waiting)
			MANGORender3DEnvironment();
		}
	}

	// Cleanup the accelerator table
	if( hAccel != NULL )
		DestroyAcceleratorTable( hAccel );

	GetMANGOState().SetInsideMainloop( false );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the 3D environment by:
//      - Checking if the device is lost and trying to reset it if it is
//      - Get the elapsed time since the last frame
//      - Calling the app's framemove and render callback
//      - Calling Present()
//--------------------------------------------------------------------------------------
void MANGORender3DEnvironment()
{
	HRESULT hr;

	if( GetMANGOState().GetDeviceLost() || MANGOIsRenderingPaused() || !MANGOIsActive() )
	{
		// Window is minimized or paused so yield CPU time to other processes
		Sleep( 50 ); 
	}

	IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
	if( NULL == pd3dDevice )
	{
		if( GetMANGOState().GetDeviceLost() )
		{
			MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
			MANGOChangeDevice( &deviceSettings, NULL, false, true );
		}

		return;
	}

	if( GetMANGOState().GetDeviceLost() && !GetMANGOState().GetRenderingPaused() )
	{
		// Test the cooperative level to see if it's okay to render
		if( FAILED( hr = pd3dDevice->TestCooperativeLevel() ) )
		{
			if( D3DERR_DEVICELOST == hr )
			{
				// The device has been lost but cannot be reset at this time.  
				// So wait until it can be reset.
				return;
			}

			// If we are windowed, read the desktop format and 
			// ensure that the Direct3D device is using the same format 
			// since the user could have changed the desktop bitdepth 
			if( MANGOIsWindowed() )
			{
				D3DDISPLAYMODE adapterDesktopDisplayMode;
				IDirect3D9* pD3D = MANGOGetD3DObject();
				MANGODeviceSettings* pDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();
				pD3D->GetAdapterDisplayMode( pDeviceSettings->AdapterOrdinal, &adapterDesktopDisplayMode );
				if( pDeviceSettings->AdapterFormat != adapterDesktopDisplayMode.Format )
				{
					MANGOMatchOptions matchOptions;
					matchOptions.eAdapterOrdinal     = MANGOMT_PRESERVE_INPUT;
					matchOptions.eDeviceType         = MANGOMT_PRESERVE_INPUT;
					matchOptions.eWindowed           = MANGOMT_PRESERVE_INPUT;
					matchOptions.eAdapterFormat      = MANGOMT_PRESERVE_INPUT;
					matchOptions.eVertexProcessing   = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eResolution         = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eBackBufferFormat   = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eBackBufferCount    = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eMultiSample        = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eSwapEffect         = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eDepthFormat        = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eStencilFormat      = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.ePresentFlags       = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.eRefreshRate        = MANGOMT_CLOSEST_TO_INPUT;
					matchOptions.ePresentInterval    = MANGOMT_CLOSEST_TO_INPUT;

					MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
					deviceSettings.AdapterFormat = adapterDesktopDisplayMode.Format;

					hr = MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );
					if( FAILED(hr) ) // the call will fail if no valid devices were found
					{
						MANGODisplayErrorMessage( MANGOERR_NOCOMPATIBLEDEVICES );
						MANGOShutdown();
					}

					// Change to a Direct3D device created from the new device settings.  
					// If there is an existing device, then either reset or recreate the scene
					hr = MANGOChangeDevice( &deviceSettings, NULL, false, false );
					if( FAILED(hr) )  
					{
						// If this fails, try to go fullscreen and if this fails also shutdown.
						if( FAILED(MANGOToggleFullScreen()) )
							MANGOShutdown();
					}

					return;
				}
			}

			// Try to reset the device
			if( FAILED( hr = MANGOReset3DEnvironment() ) )
			{
				if( D3DERR_DEVICELOST == hr )
				{
					// The device was lost again, so continue waiting until it can be reset.
					return;
				}
				else if( MANGOERR_RESETTINGDEVICEOBJECTS == hr || 
					MANGOERR_MEDIANOTFOUND == hr )
				{
					MANGODisplayErrorMessage( hr );
					MANGOShutdown();
					return;
				}
				else
				{
					// Reset failed, but the device wasn't lost so something bad happened, 
					// so recreate the device to try to recover
					MANGODeviceSettings* pDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();
					if( FAILED( MANGOChangeDevice( pDeviceSettings, NULL, true, false ) ) )
					{
						MANGOShutdown();
						return;
					}
				}
			}
		}

		GetMANGOState().SetDeviceLost( false );
	}

	// Get the app's time, in seconds. Skip rendering if no time elapsed
	double fTime, fAbsTime; float fElapsedTime;
	MANGOGetGlobalTimer()->GetTimeValues( &fTime, &fAbsTime, &fElapsedTime );

	// Store the time for the app
	if( GetMANGOState().GetConstantFrameTime() )
	{        
		fElapsedTime = GetMANGOState().GetTimePerFrame();
		fTime     = MANGOGetTime() + fElapsedTime;
	}

	GetMANGOState().SetTime( fTime );
	GetMANGOState().SetAbsoluteTime( fAbsTime );
	GetMANGOState().SetElapsedTime( fElapsedTime );

	// Update the FPS stats
	MANGOUpdateFrameStats();

	MANGOHandleTimers();

	// Animate the scene by calling the app's frame move callback
	LPMANGOCALLBACKFRAMEMOVE pCallbackFrameMove = GetMANGOState().GetFrameMoveFunc();
	if( pCallbackFrameMove != NULL )
	{
		pCallbackFrameMove( pd3dDevice, fTime, fElapsedTime, GetMANGOState().GetFrameMoveFuncUserContext() );
		pd3dDevice = MANGOGetD3DDevice();
		if( NULL == pd3dDevice ) // Handle MANGOShutdown from inside callback
			return;
	}

	if( !GetMANGOState().GetRenderingPaused() )
	{
		// Render the scene by calling the app's render callback
		LPMANGOCALLBACKFRAMERENDER pCallbackFrameRender = GetMANGOState().GetFrameRenderFunc();
		if( pCallbackFrameRender != NULL )
		{
			pCallbackFrameRender( pd3dDevice, fTime, fElapsedTime, GetMANGOState().GetFrameRenderFuncUserContext() );
			pd3dDevice = MANGOGetD3DDevice();
			if( NULL == pd3dDevice ) // Handle MANGOShutdown from inside callback
				return;
		}

#if defined(DEBUG) || defined(_DEBUG)
		// The back buffer should always match the client rect 
		// if the Direct3D backbuffer covers the entire window
		RECT rcClient;
		GetClientRect( MANGOGetHWND(), &rcClient );
		if( !IsIconic( MANGOGetHWND() ) )
		{
			GetClientRect( MANGOGetHWND(), &rcClient );
			assert( MANGOGetBackBufferSurfaceDesc()->Width == (UINT)rcClient.right );
			assert( MANGOGetBackBufferSurfaceDesc()->Height == (UINT)rcClient.bottom );
		}        
#endif

		// Show the frame on the primary surface.
		hr = pd3dDevice->Present( NULL, NULL, NULL, NULL );
		if( FAILED(hr) )
		{
			if( D3DERR_DEVICELOST == hr )
			{
				GetMANGOState().SetDeviceLost( true );
			}
			else if( D3DERR_DRIVERINTERNALERROR == hr )
			{
				// When D3DERR_DRIVERINTERNALERROR is returned from Present(),
				// the application can do one of the following:
				// 
				// - End, with the pop-up window saying that the application cannot continue 
				//   because of problems in the display adapter and that the user should 
				//   contact the adapter manufacturer.
				//
				// - Attempt to restart by calling IDirect3DDevice9::Reset, which is essentially the same 
				//   path as recovering from a lost device. If IDirect3DDevice9::Reset fails with 
				//   D3DERR_DRIVERINTERNALERROR, the application should end immediately with the message 
				//   that the user should contact the adapter manufacturer.
				// 
				// The framework attempts the path of resetting the device
				// 
				GetMANGOState().SetDeviceLost( true );
			}
		}
	}

	// Update current frame #
	int nFrame = GetMANGOState().GetCurrentFrameNumber();
	nFrame++;
	GetMANGOState().SetCurrentFrameNumber( nFrame );

	// Check to see if the app should shutdown due to cmdline
	if( GetMANGOState().GetOverrideQuitAfterFrame() != 0 )
	{
		if( nFrame > GetMANGOState().GetOverrideQuitAfterFrame() )
			MANGOShutdown();
	}

	return;
}


//--------------------------------------------------------------------------------------
// Updates the string which describes the device 
//--------------------------------------------------------------------------------------
void MANGOUpdateDeviceStats( D3DDEVTYPE DeviceType, DWORD BehaviorFlags, D3DADAPTER_IDENTIFIER9* pAdapterIdentifier )
{
	if( GetMANGOState().GetNoStats() )
		return;

	// Store device description
	WCHAR* pstrDeviceStats = GetMANGOState().GetDeviceStats();
	if( DeviceType == D3DDEVTYPE_REF )
		StringCchCopy( pstrDeviceStats, 256, L"REF" );
	else if( DeviceType == D3DDEVTYPE_HAL )
		StringCchCopy( pstrDeviceStats, 256, L"HAL" );
	else if( DeviceType == D3DDEVTYPE_SW )
		StringCchCopy( pstrDeviceStats, 256, L"SW" );

	if( BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING &&
		BehaviorFlags & D3DCREATE_PUREDEVICE )
	{
		if( DeviceType == D3DDEVTYPE_HAL )
			StringCchCat( pstrDeviceStats, 256, L" (pure hw vp)" );
		else
			StringCchCat( pstrDeviceStats, 256, L" (simulated pure hw vp)" );
	}
	else if( BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
	{
		if( DeviceType == D3DDEVTYPE_HAL )
			StringCchCat( pstrDeviceStats, 256, L" (hw vp)" );
		else
			StringCchCat( pstrDeviceStats, 256, L" (simulated hw vp)" );
	}
	else if( BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
	{
		if( DeviceType == D3DDEVTYPE_HAL )
			StringCchCat( pstrDeviceStats, 256, L" (mixed vp)" );
		else
			StringCchCat( pstrDeviceStats, 256, L" (simulated mixed vp)" );
	}
	else if( BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
	{
		StringCchCat( pstrDeviceStats, 256, L" (sw vp)" );
	}

	if( DeviceType == D3DDEVTYPE_HAL )
	{
		// Be sure not to overflow m_strDeviceStats when appending the adapter 
		// description, since it can be long.  
		StringCchCat( pstrDeviceStats, 256, L": " );

		// Try to get a unique description from the CD3DEnumDeviceSettingsCombo
		MANGODeviceSettings* pDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();
		CD3DEnumeration* pd3dEnum = MANGOPrepareEnumerationObject();
		CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pd3dEnum->GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed );
		if( pDeviceSettingsCombo )
		{
			StringCchCat( pstrDeviceStats, 256, pDeviceSettingsCombo->pAdapterInfo->szUniqueDescription );
		}
		else
		{
			const int cchDesc = sizeof(pAdapterIdentifier->Description);
			WCHAR szDescription[cchDesc];
			MultiByteToWideChar( CP_ACP, 0, pAdapterIdentifier->Description, -1, szDescription, cchDesc );
			szDescription[cchDesc-1] = 0;
			StringCchCat( pstrDeviceStats, 256, szDescription );
		}
	}
}


//--------------------------------------------------------------------------------------
// Updates the frames/sec stat once per second
//--------------------------------------------------------------------------------------
void MANGOUpdateFrameStats()
{
	if( GetMANGOState().GetNoStats() )
		return;

	// Keep track of the frame count
	double fLastTime = GetMANGOState().GetLastStatsUpdateTime();
	DWORD dwFrames  = GetMANGOState().GetLastStatsUpdateFrames();
	double fAbsTime = GetMANGOState().GetAbsoluteTime();
	dwFrames++;
	GetMANGOState().SetLastStatsUpdateFrames( dwFrames );

	// Update the scene stats once per second
	if( fAbsTime - fLastTime > 1.0f )
	{
		float fFPS = (float) (dwFrames / (fAbsTime - fLastTime));
		GetMANGOState().SetFPS( fFPS );
		GetMANGOState().SetLastStatsUpdateTime( fAbsTime );
		GetMANGOState().SetLastStatsUpdateFrames( 0 );

		WCHAR* pstrFPS = GetMANGOState().GetFPSStats();
		StringCchPrintf( pstrFPS, 64, L"%0.2f fps ", fFPS );
	}
}


//--------------------------------------------------------------------------------------
// Updates the static part of the frame stats so it doesn't have be generated every frame
//--------------------------------------------------------------------------------------
void MANGOUpdateStaticFrameStats()
{
	if( GetMANGOState().GetNoStats() )
		return;

	MANGODeviceSettings* pDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();
	if( NULL == pDeviceSettings )
		return;
	CD3DEnumeration* pd3dEnum = MANGOPrepareEnumerationObject();
	if( NULL == pd3dEnum )
		return;

	CD3DEnumDeviceSettingsCombo* pDeviceSettingsCombo = pd3dEnum->GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed );
	if( NULL == pDeviceSettingsCombo )
		return;

	WCHAR strFmt[100];
	D3DPRESENT_PARAMETERS* pPP = &pDeviceSettings->pp;

	if( pDeviceSettingsCombo->AdapterFormat == pDeviceSettingsCombo->BackBufferFormat )
	{
		StringCchCopy( strFmt, 100, MANGOD3DFormatToString( pDeviceSettingsCombo->AdapterFormat, false ) );
	}
	else
	{
		StringCchPrintf( strFmt, 100, L"backbuf %s, adapter %s", 
			MANGOD3DFormatToString( pDeviceSettingsCombo->BackBufferFormat, false ), 
			MANGOD3DFormatToString( pDeviceSettingsCombo->AdapterFormat, false ) );
	}

	WCHAR strDepthFmt[100];
	if( pPP->EnableAutoDepthStencil )
	{
		StringCchPrintf( strDepthFmt, 100, L" (%s)", MANGOD3DFormatToString( pPP->AutoDepthStencilFormat, false ) );
	}
	else
	{
		// No depth buffer
		strDepthFmt[0] = 0;
	}

	WCHAR strMultiSample[100];
	switch( pPP->MultiSampleType )
	{
	case D3DMULTISAMPLE_NONMASKABLE: StringCchCopy( strMultiSample, 100, L" (Nonmaskable Multisample)" ); break;
	case D3DMULTISAMPLE_NONE:        StringCchCopy( strMultiSample, 100, L"" ); break;
	default:                         StringCchPrintf( strMultiSample, 100, L" (%dx Multisample)", pPP->MultiSampleType ); break;
	}

	WCHAR* pstrStaticFrameStats = GetMANGOState().GetStaticFrameStats();
	StringCchPrintf( pstrStaticFrameStats, 256, L"%%sVsync %s (%dx%d), %s%s%s", 
		( pPP->PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE ) ? L"off" : L"on", 
		pPP->BackBufferWidth, pPP->BackBufferHeight,
		strFmt, strDepthFmt, strMultiSample );
}


//--------------------------------------------------------------------------------------
LPCWSTR MANGOGetFrameStats( bool bShowFPS )                         
{ 
	WCHAR* pstrFrameStats = GetMANGOState().GetFrameStats();
	WCHAR* pstrFPS = ( bShowFPS ) ? GetMANGOState().GetFPSStats() : L"";
	StringCchPrintf( pstrFrameStats, 256, GetMANGOState().GetStaticFrameStats(), pstrFPS );
	return pstrFrameStats;
}


//--------------------------------------------------------------------------------------
// Handles window messages 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MANGOStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Consolidate the keyboard messages and pass them to the app's keyboard callback
	if( uMsg == WM_KEYDOWN ||
		uMsg == WM_SYSKEYDOWN || 
		uMsg == WM_KEYUP ||
		uMsg == WM_SYSKEYUP )
	{
		bool bKeyDown = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
		DWORD dwMask = (1 << 29);
		bool bAltDown = ( (lParam & dwMask) != 0 );

		bool* bKeys = GetMANGOState().GetKeys();
		bKeys[ (BYTE) (wParam & 0xFF) ] = bKeyDown;

		LPMANGOCALLBACKKEYBOARD pCallbackKeyboard = GetMANGOState().GetKeyboardFunc();
		if( pCallbackKeyboard )
			pCallbackKeyboard( (UINT)wParam, bKeyDown, bAltDown, GetMANGOState().GetKeyboardFuncUserContext() );           
	}

	// Consolidate the mouse button messages and pass them to the app's mouse callback
	if( uMsg == WM_LBUTTONDOWN ||
		uMsg == WM_LBUTTONUP ||
		uMsg == WM_LBUTTONDBLCLK ||
		uMsg == WM_MBUTTONDOWN ||
		uMsg == WM_MBUTTONUP ||
		uMsg == WM_MBUTTONDBLCLK ||
		uMsg == WM_RBUTTONDOWN ||
		uMsg == WM_RBUTTONUP ||
		uMsg == WM_RBUTTONDBLCLK ||
		uMsg == WM_XBUTTONDOWN ||
		uMsg == WM_XBUTTONUP ||
		uMsg == WM_XBUTTONDBLCLK ||
		uMsg == WM_MOUSEWHEEL || 
		(GetMANGOState().GetNotifyOnMouseMove() && uMsg == WM_MOUSEMOVE) )
	{
		int xPos = (short)LOWORD(lParam);
		int yPos = (short)HIWORD(lParam);

		if( uMsg == WM_MOUSEWHEEL )
		{
			// WM_MOUSEWHEEL passes screen mouse coords
			// so convert them to client coords
			POINT pt;
			pt.x = xPos; pt.y = yPos;
			ScreenToClient( hWnd, &pt );
			xPos = pt.x; yPos = pt.y;
		}

		int nMouseWheelDelta = 0;
		if( uMsg == WM_MOUSEWHEEL ) 
			nMouseWheelDelta = (short) HIWORD(wParam);

		int nMouseButtonState = LOWORD(wParam);
		bool bLeftButton  = ((nMouseButtonState & MK_LBUTTON) != 0);
		bool bRightButton = ((nMouseButtonState & MK_RBUTTON) != 0);
		bool bMiddleButton = ((nMouseButtonState & MK_MBUTTON) != 0);
		bool bSideButton1 = ((nMouseButtonState & MK_XBUTTON1) != 0);
		bool bSideButton2 = ((nMouseButtonState & MK_XBUTTON2) != 0);

		bool* bMouseButtons = GetMANGOState().GetMouseButtons();
		bMouseButtons[0] = bLeftButton;
		bMouseButtons[1] = bMiddleButton;
		bMouseButtons[2] = bRightButton;
		bMouseButtons[3] = bSideButton1;
		bMouseButtons[4] = bSideButton2;

		LPMANGOCALLBACKMOUSE pCallbackMouse = GetMANGOState().GetMouseFunc();
		if( pCallbackMouse )
			pCallbackMouse( bLeftButton, bRightButton, bMiddleButton, bSideButton1, bSideButton2, nMouseWheelDelta, xPos, yPos, GetMANGOState().GetMouseFuncUserContext() );
	}

	// Pass all messages to the app's MsgProc callback, and don't 
	// process further messages if the apps says not to.
	LPMANGOCALLBACKMSGPROC pCallbackMsgProc = GetMANGOState().GetWindowMsgFunc();
	if( pCallbackMsgProc )
	{
		bool bNoFurtherProcessing = false;
		LRESULT nResult = pCallbackMsgProc( hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing, GetMANGOState().GetWindowMsgFuncUserContext() );
		if( bNoFurtherProcessing )
			return nResult;
	}

	switch( uMsg )
	{
	case WM_PAINT:
		{
			IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();

			// Handle paint messages when the app is paused
			if( pd3dDevice && MANGOIsRenderingPaused() && 
				GetMANGOState().GetDeviceObjectsCreated() && GetMANGOState().GetDeviceObjectsReset() )
			{
				HRESULT hr;
				double fTime = MANGOGetTime();
				float fElapsedTime = MANGOGetElapsedTime();

				LPMANGOCALLBACKFRAMERENDER pCallbackFrameRender = GetMANGOState().GetFrameRenderFunc();
				if( pCallbackFrameRender != NULL )
					pCallbackFrameRender( pd3dDevice, fTime, fElapsedTime, GetMANGOState().GetFrameRenderFuncUserContext() );

				hr = pd3dDevice->Present( NULL, NULL, NULL, NULL );
				if( D3DERR_DEVICELOST == hr )
				{
					GetMANGOState().SetDeviceLost( true );
				}
				else if( D3DERR_DRIVERINTERNALERROR == hr )
				{
					// When D3DERR_DRIVERINTERNALERROR is returned from Present(),
					// the application can do one of the following:
					// 
					// - End, with the pop-up window saying that the application cannot continue 
					//   because of problems in the display adapter and that the user should 
					//   contact the adapter manufacturer.
					//
					// - Attempt to restart by calling IDirect3DDevice9::Reset, which is essentially the same 
					//   path as recovering from a lost device. If IDirect3DDevice9::Reset fails with 
					//   D3DERR_DRIVERINTERNALERROR, the application should end immediately with the message 
					//   that the user should contact the adapter manufacturer.
					// 
					// The framework attempts the path of resetting the device
					// 
					GetMANGOState().SetDeviceLost( true );
				}
			}
			break;
		}

	case WM_SIZE:
		if( SIZE_MINIMIZED == wParam )
		{
			MANGOPause( true, true ); // Pause while we're minimized

			GetMANGOState().SetMinimized( true );
			GetMANGOState().SetMaximized( false );
		}
		else
		{
			RECT rcCurrentClient;
			GetClientRect( MANGOGetHWND(), &rcCurrentClient );
			if( rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0 )
			{
				// Rapidly clicking the task bar to minimize and restore a window
				// can cause a WM_SIZE message with SIZE_RESTORED when 
				// the window has actually become minimized due to rapid change
				// so just ignore this message
			}
			else if( SIZE_MAXIMIZED == wParam )
			{
				if( GetMANGOState().GetMinimized() )
					MANGOPause( false, false ); // Unpause since we're no longer minimized
				GetMANGOState().SetMinimized( false );
				GetMANGOState().SetMaximized( true );
				MANGOCheckForWindowSizeChange();
				MANGOCheckForWindowChangingMonitors();
			}
			else if( SIZE_RESTORED == wParam )
			{      
				if( GetMANGOState().GetMaximized() )
				{
					GetMANGOState().SetMaximized( false );
					MANGOCheckForWindowSizeChange();
					MANGOCheckForWindowChangingMonitors();
				}
				else if( GetMANGOState().GetMinimized() )
				{
					MANGOPause( false, false ); // Unpause since we're no longer minimized
					GetMANGOState().SetMinimized( false );
					MANGOCheckForWindowSizeChange();
					MANGOCheckForWindowChangingMonitors();
				}
				else if( GetMANGOState().GetInSizeMove() )
				{
					// If we're neither maximized nor minimized, the window size 
					// is changing by the user dragging the window edges.  In this 
					// case, we don't reset the device yet -- we wait until the 
					// user stops dragging, and a WM_EXITSIZEMOVE message comes.
				}
				else
				{
					// This WM_SIZE come from resizing the window via an API like SetWindowPos() so 
					// resize and reset the device now.
					MANGOCheckForWindowSizeChange();
					MANGOCheckForWindowChangingMonitors();
				}
			}
		}
		break;

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = MANGO_MIN_WINDOW_SIZE_X;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = MANGO_MIN_WINDOW_SIZE_Y;
		break;

	case WM_ENTERSIZEMOVE:
		// Halt frame movement while the app is sizing or moving
		MANGOPause( true, true );
		GetMANGOState().SetInSizeMove( true );
		break;

	case WM_EXITSIZEMOVE:
		MANGOPause( false, false );
		MANGOCheckForWindowSizeChange();
		MANGOCheckForWindowChangingMonitors();
		GetMANGOState().SetInSizeMove( false );
		break;

	case WM_MOUSEMOVE:
		if( MANGOIsActive() && !MANGOIsWindowed() )
		{
			IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
			if( pd3dDevice )
			{
				POINT ptCursor;
				GetCursorPos( &ptCursor );
				pd3dDevice->SetCursorPosition( ptCursor.x, ptCursor.y, 0 );
			}
		}
		break;

	case WM_SETCURSOR:
		if( MANGOIsActive() && !MANGOIsWindowed() )
		{
			IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
			if( pd3dDevice && GetMANGOState().GetShowCursorWhenFullScreen() )
				pd3dDevice->ShowCursor( true );
			return true; // prevent Windows from setting cursor to window class cursor
		}
		break;

	case WM_ACTIVATEAPP:
		if( wParam == TRUE && !MANGOIsActive() ) // Handle only if previously not active 
		{
			GetMANGOState().SetActive( true );

			// Disable any controller rumble & input when de-activating app
			MANGOEnableXInput( true );

			// The GetMinimizedWhileFullscreen() varible is used instead of !MANGOIsWindowed()
			// to handle the rare case toggling to windowed mode while the fullscreen application 
			// is minimized and thus making the pause count wrong
			if( GetMANGOState().GetMinimizedWhileFullscreen() ) 
			{
				MANGOPause( false, false ); // Unpause since we're no longer minimized
				GetMANGOState().SetMinimizedWhileFullscreen( false );
			}

			// Upon returning to this app, potentially disable shortcut keys 
			// (Windows key, accessibility shortcuts) 
			MANGOAllowShortcutKeys( ( MANGOIsWindowed() ) ? GetMANGOState().GetAllowShortcutKeysWhenWindowed() : 
			GetMANGOState().GetAllowShortcutKeysWhenFullscreen() );

		}
		else if( wParam == FALSE && MANGOIsActive() ) // Handle only if previously active 
		{               
			GetMANGOState().SetActive( false );

			// Disable any controller rumble & input when de-activating app
			MANGOEnableXInput( false );

			if( !MANGOIsWindowed() )
			{
				// Going from full screen to a minimized state 
				ClipCursor( NULL );      // don't limit the cursor anymore
				MANGOPause( true, true ); // Pause while we're minimized (take care not to pause twice by handling this message twice)
				GetMANGOState().SetMinimizedWhileFullscreen( true ); 
			}

			// Restore shortcut keys (Windows key, accessibility shortcuts) to original state
			//
			// This is important to call here if the shortcuts are disabled, 
			// because if this is not done then the Windows key will continue to 
			// be disabled while this app is running which is very bad.
			// If the app crashes, the Windows key will return to normal.
			MANGOAllowShortcutKeys( true );
		}
		break;

	case WM_ENTERMENULOOP:
		// Pause the app when menus are displayed
		MANGOPause( true, true );
		break;

	case WM_EXITMENULOOP:
		MANGOPause( false, false );
		break;

	case WM_MENUCHAR:
		// A menu is active and the user presses a key that does not correspond to any mnemonic or accelerator key
		// So just ignore and don't beep
		return MAKELRESULT(0,MNC_CLOSE);
		break;

	case WM_NCHITTEST:
		// Prevent the user from selecting the menu in full screen mode
		if( !MANGOIsWindowed() )
			return HTCLIENT;
		break;

	case WM_POWERBROADCAST:
		switch( wParam )
		{
#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND 0x0000
#endif
	case PBT_APMQUERYSUSPEND:
		// At this point, the app should save any data for open
		// network connections, files, etc., and prepare to go into
		// a suspended mode.  The app can use the MsgProc callback
		// to handle this if desired.
		return true;

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif
	case PBT_APMRESUMESUSPEND:
		// At this point, the app should recover any data, network
		// connections, files, etc., and resume running from when
		// the app was suspended. The app can use the MsgProc callback
		// to handle this if desired.

		// QPC may lose consistency when suspending, so reset the timer
		// upon resume.
		MANGOGetGlobalTimer()->Reset();                   
		GetMANGOState().SetLastStatsUpdateTime( 0 );
		return true;
		}
		break;

	case WM_SYSCOMMAND:
		// Prevent moving/sizing in full screen mode
		switch( wParam )
		{
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
			if( !MANGOIsWindowed() )
				return 0;
			break;
		}
		break;

	case WM_SYSKEYDOWN:
		{
			switch( wParam )
			{
			case VK_RETURN:
				{
					if( GetMANGOState().GetHandleAltEnter() )
					{
						// Toggle full screen upon alt-enter 
						DWORD dwMask = (1 << 29);
						if( (lParam & dwMask) != 0 ) // Alt is down also
						{
							// Toggle the full screen/window mode
							MANGOPause( true, true );
							MANGOToggleFullScreen();
							MANGOPause( false, false );                        
							return 0;
						}
					}
				}
			}
			break;
		}

	case WM_KEYDOWN:
		{
			if( GetMANGOState().GetHandleDefaultHotkeys() )
			{
				switch( wParam )
				{
				case VK_F3:
					{
						MANGOPause( true, true );
						MANGOToggleREF();
						MANGOPause( false, false );                        
						break;
					}

				case VK_F8:
					{
						bool bWireFrame = GetMANGOState().GetWireframeMode();
						bWireFrame = !bWireFrame; 
						GetMANGOState().SetWireframeMode( bWireFrame );

						IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
						if( pd3dDevice )
							pd3dDevice->SetRenderState( D3DRS_FILLMODE, (bWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID ); 
						break;
					}

				case VK_ESCAPE:
					{
						// Received key to exit app
						SendMessage( hWnd, WM_CLOSE, 0, 0 );
					}

				case VK_PAUSE: 
					{
						bool bTimePaused = MANGOIsTimePaused();
						bTimePaused = !bTimePaused;
						if( bTimePaused ) 
							MANGOPause( true, false ); 
						else
							MANGOPause( false, false ); 
						break; 
					}
				}
			}
			break;
		}

	case WM_CLOSE:
		{
			HMENU hMenu;
			hMenu = GetMenu(hWnd);
			if( hMenu != NULL )
				DestroyMenu( hMenu );
			DestroyWindow( hWnd );
			UnregisterClass( L"Direct3DWindowClass", NULL );
			GetMANGOState().SetHWNDFocus( NULL );
			GetMANGOState().SetHWNDDeviceFullScreen( NULL );
			GetMANGOState().SetHWNDDeviceWindowed( NULL );
			return 0;
		}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	// Don't allow the F10 key to act as a shortcut to the menu bar
	// by not passing these messages to the DefWindowProc only when
	// there's no menu present
	if( !GetMANGOState().GetCallDefWindowProc() || GetMANGOState().GetMenu() == NULL && (uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP) && wParam == VK_F10 )
		return 0;
	else
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


//--------------------------------------------------------------------------------------
// Resets the state associated with MANGO 
//--------------------------------------------------------------------------------------
void MANGOResetFrameworkState()
{
	GetMANGOState().Destroy();
	GetMANGOState().Create();
}


//--------------------------------------------------------------------------------------
// Closes down the window.  When the window closes, it will cleanup everything
//--------------------------------------------------------------------------------------
void MANGOShutdown( int nExitCode )
{
	HWND hWnd = MANGOGetHWND();
	if( hWnd != NULL )
		SendMessage( hWnd, WM_CLOSE, 0, 0 );

	GetMANGOState().SetExitCode(nExitCode);

	MANGOCleanup3DEnvironment( true );

	// Restore shortcut keys (Windows key, accessibility shortcuts) to original state
	// This is important to call here if the shortcuts are disabled, 
	// because accessibility setting changes are permanent.
	// This means that if this is not done then the accessibility settings 
	// might not be the same as when the app was started. 
	// If the app crashes without restoring the settings, this is also true so it
	// would be wise to backup/restore the settings from a file so they can be 
	// restored when the crashed app is run again.
	MANGOAllowShortcutKeys( true );

	GetMANGOState().SetD3DEnumeration( NULL );

	IDirect3D9* pD3D = MANGOGetD3DObject();
	SAFE_RELEASE( pD3D );
	GetMANGOState().SetD3D( NULL );

	if( GetMANGOState().GetOverrideRelaunchMCE() )
		MANGOReLaunchMediaCenter();
}


//--------------------------------------------------------------------------------------
// Cleans up the 3D environment by:
//      - Calls the device lost callback 
//      - Calls the device destroyed callback 
//      - Releases the D3D device
//--------------------------------------------------------------------------------------
void MANGOCleanup3DEnvironment( bool bReleaseSettings )
{
	IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
	if( pd3dDevice != NULL )
	{
		GetMANGOState().SetInsideDeviceCallback( true );

		// Call the app's device lost callback
		if( GetMANGOState().GetDeviceObjectsReset() == true )
		{
			LPMANGOCALLBACKDEVICELOST pCallbackDeviceLost = GetMANGOState().GetDeviceLostFunc();
			if( pCallbackDeviceLost != NULL )
				pCallbackDeviceLost( GetMANGOState().GetDeviceLostFuncUserContext() );
			GetMANGOState().SetDeviceObjectsReset( false );

			// Call the resource cache device lost function
			MANGOGetGlobalResourceCache().OnLostDevice();
		}

		// Call the app's device destroyed callback
		if( GetMANGOState().GetDeviceObjectsCreated() == true )
		{
			LPMANGOCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed = GetMANGOState().GetDeviceDestroyedFunc();
			if( pCallbackDeviceDestroyed != NULL )
				pCallbackDeviceDestroyed( GetMANGOState().GetDeviceDestroyedFuncUserContext() );
			GetMANGOState().SetDeviceObjectsCreated( false );

			// Call the resource cache device destory function
			MANGOGetGlobalResourceCache().OnDestroyDevice();
		}

		GetMANGOState().SetInsideDeviceCallback( false );

		// Release the D3D device and in debug configs, displays a message box if there 
		// are unrelease objects.
		if( pd3dDevice )
		{
			if( pd3dDevice->Release() > 0 )  
			{
				MANGODisplayErrorMessage( MANGOERR_NONZEROREFCOUNT );
				MANGO_ERR( L"MANGOCleanup3DEnvironment", MANGOERR_NONZEROREFCOUNT );
			}
		}
		GetMANGOState().SetD3DDevice( NULL );

		if( bReleaseSettings )
		{
			MANGODeviceSettings* pOldDeviceSettings = GetMANGOState().GetCurrentDeviceSettings();
			SAFE_DELETE(pOldDeviceSettings);  
			GetMANGOState().SetCurrentDeviceSettings( NULL );
		}

		D3DSURFACE_DESC* pbackBufferSurfaceDesc = GetMANGOState().GetBackBufferSurfaceDesc();
		ZeroMemory( pbackBufferSurfaceDesc, sizeof(D3DSURFACE_DESC) );

		D3DCAPS9* pd3dCaps = GetMANGOState().GetCaps();
		ZeroMemory( pd3dCaps, sizeof(D3DCAPS9) );

		GetMANGOState().SetDeviceCreated( false );
	}
}


//--------------------------------------------------------------------------------------
// Stores back buffer surface desc in GetMANGOState().GetBackBufferSurfaceDesc()
//--------------------------------------------------------------------------------------
void MANGOUpdateBackBufferDesc()
{
	HRESULT hr;
	IDirect3DSurface9* pBackBuffer;
	hr = GetMANGOState().GetD3DDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	D3DSURFACE_DESC* pBBufferSurfaceDesc = GetMANGOState().GetBackBufferSurfaceDesc();
	ZeroMemory( pBBufferSurfaceDesc, sizeof(D3DSURFACE_DESC) );
	if( SUCCEEDED(hr) )
	{
		pBackBuffer->GetDesc( pBBufferSurfaceDesc );
		SAFE_RELEASE( pBackBuffer );
	}
}


//--------------------------------------------------------------------------------------
// Starts a user defined timer callback
//--------------------------------------------------------------------------------------
HRESULT MANGOSetTimer( LPMANGOCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs, UINT* pnIDEvent, void* pCallbackUserContext ) 
{ 
	if( pCallbackTimer == NULL )
		return MANGO_ERR_MSGBOX( L"MANGOSetTimer", E_INVALIDARG ); 

	HRESULT hr;
	MANGO_TIMER MANGOTimer;
	MANGOTimer.pCallbackTimer = pCallbackTimer;
	MANGOTimer.pCallbackUserContext = pCallbackUserContext;
	MANGOTimer.fTimeoutInSecs = fTimeoutInSecs;
	MANGOTimer.fCountdown = fTimeoutInSecs;
	MANGOTimer.bEnabled = true;
	MANGOTimer.nID = GetMANGOState().GetTimerLastID() + 1;
	GetMANGOState().SetTimerLastID( MANGOTimer.nID );

	CMangoArray<MANGO_TIMER>* pTimerList = GetMANGOState().GetTimerList();
	if( pTimerList == NULL )
	{
		pTimerList = new CMangoArray<MANGO_TIMER>;
		if( pTimerList == NULL )
			return E_OUTOFMEMORY; 
		GetMANGOState().SetTimerList( pTimerList );
	}

	if( FAILED( hr = pTimerList->Add( MANGOTimer ) ) )
		return hr;

	if( pnIDEvent )
		*pnIDEvent = MANGOTimer.nID;

	return S_OK; 
}


//--------------------------------------------------------------------------------------
// Stops a user defined timer callback
//--------------------------------------------------------------------------------------
HRESULT MANGOKillTimer( UINT nIDEvent ) 
{ 
	CMangoArray<MANGO_TIMER>* pTimerList = GetMANGOState().GetTimerList();
	if( pTimerList == NULL )
		return S_FALSE;

	bool bFound = false;

	for( int i=0; i<pTimerList->GetSize(); i++ )
	{
		MANGO_TIMER MANGOTimer = pTimerList->GetAt(i);
		if( MANGOTimer.nID == nIDEvent )
		{
			MANGOTimer.bEnabled = false;
			pTimerList->SetAt(i, MANGOTimer);
			bFound = true;
			break;
		}
	}

	if( !bFound ) 
		return MANGO_ERR_MSGBOX( L"MANGOKillTimer", E_INVALIDARG );

	return S_OK; 
}


//--------------------------------------------------------------------------------------
// Internal helper function to handle calling the user defined timer callbacks
//--------------------------------------------------------------------------------------
void MANGOHandleTimers()
{
	float fElapsedTime = MANGOGetElapsedTime();

	CMangoArray<MANGO_TIMER>* pTimerList = GetMANGOState().GetTimerList();
	if( pTimerList == NULL )
		return;

	// Walk through the list of timer callbacks
	for( int i=0; i<pTimerList->GetSize(); i++ )
	{
		MANGO_TIMER MANGOTimer = pTimerList->GetAt(i);
		if( MANGOTimer.bEnabled )
		{
			MANGOTimer.fCountdown -= fElapsedTime;

			// Call the callback if count down expired
			if( MANGOTimer.fCountdown < 0 )
			{
				MANGOTimer.pCallbackTimer( i, MANGOTimer.pCallbackUserContext );
				MANGOTimer.fCountdown = MANGOTimer.fTimeoutInSecs;
			}
			pTimerList->SetAt(i, MANGOTimer);
		}
	}
}


//--------------------------------------------------------------------------------------
// External state access functions
//--------------------------------------------------------------------------------------
IDirect3D9* MANGOGetD3DObject()                      { return GetMANGOState().GetD3D(); }        
IDirect3DDevice9* MANGOGetD3DDevice()                { return GetMANGOState().GetD3DDevice(); }  
const D3DSURFACE_DESC* MANGOGetBackBufferSurfaceDesc() { return GetMANGOState().GetBackBufferSurfaceDesc(); }
const D3DCAPS9* MANGOGetDeviceCaps()                 { return GetMANGOState().GetCaps(); }
HINSTANCE MANGOGetHINSTANCE()                        { return GetMANGOState().GetHInstance(); }
HWND MANGOGetHWND()                                  { return MANGOIsWindowed() ? GetMANGOState().GetHWNDDeviceWindowed() : GetMANGOState().GetHWNDDeviceFullScreen(); }
HWND MANGOGetHWNDFocus()                             { return GetMANGOState().GetHWNDFocus(); }
HWND MANGOGetHWNDDeviceFullScreen()                  { return GetMANGOState().GetHWNDDeviceFullScreen(); }
HWND MANGOGetHWNDDeviceWindowed()                    { return GetMANGOState().GetHWNDDeviceWindowed(); }
RECT MANGOGetWindowClientRect()                      { RECT rc; GetClientRect( MANGOGetHWND(), &rc ); return rc; }
RECT MANGOGetWindowClientRectAtModeChange()          { RECT rc = { 0, 0, GetMANGOState().GetWindowBackBufferWidthAtModeChange(), GetMANGOState().GetWindowBackBufferHeightAtModeChange() }; return rc; }
RECT MANGOGetFullsceenClientRectAtModeChange()       { RECT rc = { 0, 0, GetMANGOState().GetFullScreenBackBufferWidthAtModeChange(), GetMANGOState().GetFullScreenBackBufferHeightAtModeChange() }; return rc; }
double MANGOGetTime()                                { return GetMANGOState().GetTime(); }
float MANGOGetElapsedTime()                          { return GetMANGOState().GetElapsedTime(); }
float MANGOGetFPS()                                  { return GetMANGOState().GetFPS(); }
LPCWSTR MANGOGetWindowTitle()                        { return GetMANGOState().GetWindowTitle(); }
LPCWSTR MANGOGetDeviceStats()                        { return GetMANGOState().GetDeviceStats(); }
bool MANGOIsRenderingPaused()                        { return GetMANGOState().GetPauseRenderingCount() > 0; }
bool MANGOIsTimePaused()                             { return GetMANGOState().GetPauseTimeCount() > 0; }
bool MANGOIsActive()                                 { return GetMANGOState().GetActive(); }
int MANGOGetExitCode()                               { return GetMANGOState().GetExitCode(); }
bool MANGOGetShowMsgBoxOnError()                     { return GetMANGOState().GetShowMsgBoxOnError(); }
bool MANGOGetAutomation()                            { return GetMANGOState().GetAutomation(); }
bool MANGOGetHandleDefaultHotkeys()                  { return GetMANGOState().GetHandleDefaultHotkeys(); }
bool MANGOIsKeyDown( BYTE vKey )
{ 
	bool* bKeys = GetMANGOState().GetKeys(); 
	if( vKey >= 0xA0 && vKey <= 0xA5 )  // VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU
		return GetAsyncKeyState( vKey ) != 0; // these keys only are tracked via GetAsyncKeyState()
	else if( vKey >= 0x01 && vKey <= 0x06 && vKey != 0x03 ) // mouse buttons (VK_*BUTTON)
		return MANGOIsMouseButtonDown(vKey);
	else
		return bKeys[vKey];
}
bool MANGOIsMouseButtonDown( BYTE vButton )          
{ 
	bool* bMouseButtons = GetMANGOState().GetMouseButtons(); 
	int nIndex = MANGOMapButtonToArrayIndex(vButton); 
	return bMouseButtons[nIndex]; 
}
void MANGOSetMultimonSettings( bool bAutoChangeAdapter )
{
	GetMANGOState().SetAutoChangeAdapter( bAutoChangeAdapter );
}
void MANGOSetCursorSettings( bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen ) 
{ 
	GetMANGOState().SetClipCursorWhenFullScreen(bClipCursorWhenFullScreen); 
	GetMANGOState().SetShowCursorWhenFullScreen(bShowCursorWhenFullScreen); 
	MANGOSetupCursor();
}
void MANGOSetWindowSettings( bool bCallDefWindowProc )
{
	GetMANGOState().SetCallDefWindowProc( bCallDefWindowProc );
}
void MANGOSetConstantFrameTime( bool bEnabled, float fTimePerFrame ) 
{ 
	if( GetMANGOState().GetOverrideConstantFrameTime() ) 
	{ 
		bEnabled = GetMANGOState().GetOverrideConstantFrameTime(); 
		fTimePerFrame = GetMANGOState().GetOverrideConstantTimePerFrame(); 
	} 
	GetMANGOState().SetConstantFrameTime(bEnabled);
	GetMANGOState().SetTimePerFrame(fTimePerFrame); 
}


//--------------------------------------------------------------------------------------
// Return if windowed in the current device.  If no device exists yet, then returns false
//--------------------------------------------------------------------------------------
bool MANGOIsWindowed()                               
{ 
	MANGODeviceSettings* pDeviceSettings = GetMANGOState().GetCurrentDeviceSettings(); 
	if(pDeviceSettings) 
		return (pDeviceSettings->pp.Windowed != 0); 
	else 
		return false; 
}


//--------------------------------------------------------------------------------------
// Return the present params of the current device.  If no device exists yet, then
// return blank present params
//--------------------------------------------------------------------------------------
D3DPRESENT_PARAMETERS MANGOGetPresentParameters()    
{ 
	MANGODeviceSettings* pDS = GetMANGOState().GetCurrentDeviceSettings(); 
	if( pDS ) 
	{
		return pDS->pp; 
	}
	else 
	{
		D3DPRESENT_PARAMETERS pp;
		ZeroMemory( &pp, sizeof(D3DPRESENT_PARAMETERS) );
		return pp; 
	}
}


//--------------------------------------------------------------------------------------
// Return the device settings of the current device.  If no device exists yet, then
// return blank device settings 
//--------------------------------------------------------------------------------------
MANGODeviceSettings MANGOGetDeviceSettings()   
{ 
	MANGODeviceSettings* pDS = GetMANGOState().GetCurrentDeviceSettings();
	if( pDS )
	{
		return *pDS;
	}
	else
	{
		MANGODeviceSettings ds;
		ZeroMemory( &ds, sizeof(MANGODeviceSettings) );
		return ds;
	}
} 

#ifndef SM_REMOTESESSION  // needs WINVER >= 0x0500
#define SM_REMOTESESSION  0x1000
#endif


//--------------------------------------------------------------------------------------
// Display an custom error msg box 
//--------------------------------------------------------------------------------------
void MANGODisplayErrorMessage( HRESULT hr )
{
	WCHAR strBuffer[512];

	int nExitCode;
	bool bFound = true; 
	switch( hr )
	{
	case MANGOERR_NODIRECT3D:             nExitCode = 2; StringCchCopy( strBuffer, 512, L"Could not initialize Direct3D. You may want to check that the latest version of DirectX is correctly installed on your system.  Also make sure that this program was compiled with header files that match the installed DirectX DLLs." ); break;
	case MANGOERR_INCORRECTVERSION:       nExitCode = 10; StringCchCopy( strBuffer, 512, L"Incorrect version of Direct3D and/or D3DX." ); break;
	case MANGOERR_MEDIANOTFOUND:          nExitCode = 4; StringCchCopy( strBuffer, 512, L"Could not find required media. Ensure that the DirectX SDK is correctly installed." ); break;
	case MANGOERR_NONZEROREFCOUNT:        nExitCode = 5; StringCchCopy( strBuffer, 512, L"The D3D device has a non-zero reference count, meaning some objects were not released." ); break;
	case MANGOERR_CREATINGDEVICE:         nExitCode = 6; StringCchCopy( strBuffer, 512, L"Failed creating the Direct3D device." ); break;
	case MANGOERR_RESETTINGDEVICE:        nExitCode = 7; StringCchCopy( strBuffer, 512, L"Failed resetting the Direct3D device." ); break;
	case MANGOERR_CREATINGDEVICEOBJECTS:  nExitCode = 8; StringCchCopy( strBuffer, 512, L"Failed creating Direct3D device objects." ); break;
	case MANGOERR_RESETTINGDEVICEOBJECTS: nExitCode = 9; StringCchCopy( strBuffer, 512, L"Failed resetting Direct3D device objects." ); break;
	case MANGOERR_NOCOMPATIBLEDEVICES:    
		nExitCode = 3; 
		if( GetSystemMetrics(SM_REMOTESESSION) != 0 )
			StringCchCopy( strBuffer, 512, L"Direct3D does not work over a remote session." ); 
		else
			StringCchCopy( strBuffer, 512, L"Could not find any compatible Direct3D devices." ); 
		break;
	default: bFound = false; nExitCode = 1;break;
	}   

	GetMANGOState().SetExitCode(nExitCode);

	bool bShowMsgBoxOnError = GetMANGOState().GetShowMsgBoxOnError();
	if( bFound && bShowMsgBoxOnError )
	{
		if( MANGOGetWindowTitle()[0] == 0 )
			MessageBox( MANGOGetHWND(), strBuffer, L"DirectX Application", MB_ICONERROR|MB_OK );
		else
			MessageBox( MANGOGetHWND(), strBuffer, MANGOGetWindowTitle(), MB_ICONERROR|MB_OK );
	}
}


//--------------------------------------------------------------------------------------
// Display error msg box to help debug 
//--------------------------------------------------------------------------------------
HRESULT WINAPI MANGOTrace( const CHAR* strFile, DWORD dwLine, HRESULT hr,
						 const WCHAR* strMsg, bool bPopMsgBox )
{
	bool bShowMsgBoxOnError = GetMANGOState().GetShowMsgBoxOnError();
	if( bPopMsgBox && bShowMsgBoxOnError == false )
		bPopMsgBox = false;

	return DXTrace( strFile, dwLine, hr, strMsg, bPopMsgBox );
}


//--------------------------------------------------------------------------------------
// Checks to see if the HWND changed monitors, and if it did it creates a device 
// from the monitor's adapter and recreates the scene.
//--------------------------------------------------------------------------------------
void MANGOCheckForWindowChangingMonitors()
{
	// Skip this check for various reasons
	if( !GetMANGOState().GetAutoChangeAdapter() || 
		GetMANGOState().GetIgnoreSizeChange() ||
		!GetMANGOState().GetDeviceCreated() ||
		!GetMANGOState().GetCurrentDeviceSettings()->pp.Windowed )
	{
		return;
	}

	HRESULT hr;
	HMONITOR hWindowMonitor = MANGOMonitorFromWindow( MANGOGetHWND(), MONITOR_DEFAULTTOPRIMARY );
	HMONITOR hAdapterMonitor = GetMANGOState().GetAdapterMonitor();
	if( hWindowMonitor != hAdapterMonitor )
	{
		UINT newOrdinal;
		if( SUCCEEDED( MANGOGetAdapterOrdinalFromMonitor( hWindowMonitor, &newOrdinal ) ) )
		{
			// Find the closest valid device settings with the new ordinal
			MANGODeviceSettings deviceSettings = MANGOGetDeviceSettings();
			deviceSettings.AdapterOrdinal = newOrdinal;

			MANGOMatchOptions matchOptions;
			matchOptions.eAdapterOrdinal     = MANGOMT_PRESERVE_INPUT;
			matchOptions.eDeviceType         = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eWindowed           = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eAdapterFormat      = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eVertexProcessing   = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eResolution         = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eBackBufferFormat   = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eBackBufferCount    = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eMultiSample        = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eSwapEffect         = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eDepthFormat        = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eStencilFormat      = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.ePresentFlags       = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.eRefreshRate        = MANGOMT_CLOSEST_TO_INPUT;
			matchOptions.ePresentInterval    = MANGOMT_CLOSEST_TO_INPUT;

			hr = MANGOFindValidDeviceSettings( &deviceSettings, &deviceSettings, &matchOptions );
			if( SUCCEEDED(hr) ) 
			{
				// Create a Direct3D device using the new device settings.  
				// If there is an existing device, then it will either reset or recreate the scene.
				hr = MANGOChangeDevice( &deviceSettings, NULL, false, false );

				// If hr == E_ABORT, this means the app rejected the device settings in the ModifySettingsCallback
				if( hr == E_ABORT )
				{
					// so nothing changed and keep from attempting to switch adapters next time
					GetMANGOState().SetAutoChangeAdapter( false );
				}
				else if( FAILED(hr) )
				{
					MANGOShutdown();
					MANGOPause( false, false );
					return;
				}
			}
		}
	}    
}


//--------------------------------------------------------------------------------------
// Look for an adapter ordinal that is tied to a HMONITOR
//--------------------------------------------------------------------------------------
HRESULT MANGOGetAdapterOrdinalFromMonitor( HMONITOR hMonitor, UINT* pAdapterOrdinal )
{
	*pAdapterOrdinal = 0;

	CD3DEnumeration* pd3dEnum = MANGOPrepareEnumerationObject();
	IDirect3D9*      pD3D     = MANGOGetD3DObject();

	CMangoArray<CD3DEnumAdapterInfo*>* pAdapterList = pd3dEnum->GetAdapterInfoList();
	for( int iAdapter=0; iAdapter<pAdapterList->GetSize(); iAdapter++ )
	{
		CD3DEnumAdapterInfo* pAdapterInfo = pAdapterList->GetAt(iAdapter);
		HMONITOR hAdapterMonitor = pD3D->GetAdapterMonitor( pAdapterInfo->AdapterOrdinal );
		if( hAdapterMonitor == hMonitor )
		{
			*pAdapterOrdinal = pAdapterInfo->AdapterOrdinal;
			return S_OK;
		}
	}

	return E_FAIL;
}


//--------------------------------------------------------------------------------------
// Internal function to map MK_* to an array index
//--------------------------------------------------------------------------------------
int MANGOMapButtonToArrayIndex( BYTE vButton )
{
	switch( vButton )
	{
	case MK_LBUTTON: return 0;
	case VK_MBUTTON: 
	case MK_MBUTTON: return 1;
	case MK_RBUTTON: return 2;
	case VK_XBUTTON1:
	case MK_XBUTTON1: return 3;
	case VK_XBUTTON2:
	case MK_XBUTTON2: return 4;
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Setup cursor based on current settings (window/fullscreen mode, show cursor state, clip cursor state)
//--------------------------------------------------------------------------------------
void MANGOSetupCursor()
{
	// Show the cursor again if returning to fullscreen 
	IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();
	if( !MANGOIsWindowed() && pd3dDevice )
	{
		if( GetMANGOState().GetShowCursorWhenFullScreen() )
		{
			SetCursor( NULL ); // Turn off Windows cursor in full screen mode
			HCURSOR hCursor = (HCURSOR)(ULONG_PTR)GetClassLongPtr( MANGOGetHWNDDeviceFullScreen(), GCLP_HCURSOR );
			MANGOSetDeviceCursor( pd3dDevice, hCursor, false );
			MANGOGetD3DDevice()->ShowCursor( true );
		}
		else
		{
			SetCursor( NULL ); // Turn off Windows cursor in full screen mode
			MANGOGetD3DDevice()->ShowCursor( false );
		}
	}

	// Clip cursor if requested
	if( !MANGOIsWindowed() && GetMANGOState().GetClipCursorWhenFullScreen() )
	{
		// Confine cursor to full screen window
		RECT rcWindow;
		GetWindowRect( MANGOGetHWNDDeviceFullScreen(), &rcWindow );
		ClipCursor( &rcWindow );
	}
	else
	{
		ClipCursor( NULL );
	}
}

//--------------------------------------------------------------------------------------
// Gives the D3D device a cursor with image and hotspot from hCursor.
//--------------------------------------------------------------------------------------
HRESULT MANGOSetDeviceCursor( IDirect3DDevice9* pd3dDevice, HCURSOR hCursor, bool bAddWatermark )
{
	HRESULT hr = E_FAIL;
	ICONINFO iconinfo;
	bool bBWCursor;
	LPDIRECT3DSURFACE9 pCursorSurface = NULL;
	HDC hdcColor = NULL;
	HDC hdcMask = NULL;
	HDC hdcScreen = NULL;
	BITMAP bm;
	DWORD dwWidth;
	DWORD dwHeightSrc;
	DWORD dwHeightDest;
	COLORREF crColor;
	COLORREF crMask;
	UINT x;
	UINT y;
	BITMAPINFO bmi;
	COLORREF* pcrArrayColor = NULL;
	COLORREF* pcrArrayMask = NULL;
	DWORD* pBitmap;
	HGDIOBJ hgdiobjOld;

	ZeroMemory( &iconinfo, sizeof(iconinfo) );
	if( !GetIconInfo( hCursor, &iconinfo ) )
		goto End;

	if (0 == GetObject((HGDIOBJ)iconinfo.hbmMask, sizeof(BITMAP), (LPVOID)&bm))
		goto End;
	dwWidth = bm.bmWidth;
	dwHeightSrc = bm.bmHeight;

	if( iconinfo.hbmColor == NULL )
	{
		bBWCursor = TRUE;
		dwHeightDest = dwHeightSrc / 2;
	}
	else 
	{
		bBWCursor = FALSE;
		dwHeightDest = dwHeightSrc;
	}

	// Create a surface for the fullscreen cursor
	if( FAILED( hr = pd3dDevice->CreateOffscreenPlainSurface( dwWidth, dwHeightDest, 
		D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL ) ) )
	{
		goto End;
	}

	pcrArrayMask = new DWORD[dwWidth * dwHeightSrc];

	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = dwWidth;
	bmi.bmiHeader.biHeight = dwHeightSrc;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hdcScreen = GetDC( NULL );
	hdcMask = CreateCompatibleDC( hdcScreen );
	if( hdcMask == NULL )
	{
		hr = E_FAIL;
		goto End;
	}
	hgdiobjOld = SelectObject(hdcMask, iconinfo.hbmMask);
	GetDIBits(hdcMask, iconinfo.hbmMask, 0, dwHeightSrc, 
		pcrArrayMask, &bmi, DIB_RGB_COLORS);
	SelectObject(hdcMask, hgdiobjOld);

	if (!bBWCursor)
	{
		pcrArrayColor = new DWORD[dwWidth * dwHeightDest];
		hdcColor = CreateCompatibleDC( hdcScreen );
		if( hdcColor == NULL )
		{
			hr = E_FAIL;
			goto End;
		}
		SelectObject(hdcColor, iconinfo.hbmColor);
		GetDIBits(hdcColor, iconinfo.hbmColor, 0, dwHeightDest, 
			pcrArrayColor, &bmi, DIB_RGB_COLORS);
	}

	// Transfer cursor image into the surface
	D3DLOCKED_RECT lr;
	pCursorSurface->LockRect( &lr, NULL, 0 );
	pBitmap = (DWORD*)lr.pBits;
	for( y = 0; y < dwHeightDest; y++ )
	{
		for( x = 0; x < dwWidth; x++ )
		{
			if (bBWCursor)
			{
				crColor = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
				crMask = pcrArrayMask[dwWidth*(dwHeightSrc-1-y) + x];
			}
			else
			{
				crColor = pcrArrayColor[dwWidth*(dwHeightDest-1-y) + x];
				crMask = pcrArrayMask[dwWidth*(dwHeightDest-1-y) + x];
			}
			if (crMask == 0)
				pBitmap[dwWidth*y + x] = 0xff000000 | crColor;
			else
				pBitmap[dwWidth*y + x] = 0x00000000;

			// It may be helpful to make the D3D cursor look slightly 
			// different from the Windows cursor so you can distinguish 
			// between the two when developing/testing code.  When
			// bAddWatermark is TRUE, the following code adds some
			// small grey "D3D" characters to the upper-left corner of
			// the D3D cursor image.
			if( bAddWatermark && x < 12 && y < 5 )
			{
				// 11.. 11.. 11.. .... CCC0
				// 1.1. ..1. 1.1. .... A2A0
				// 1.1. .1.. 1.1. .... A4A0
				// 1.1. ..1. 1.1. .... A2A0
				// 11.. 11.. 11.. .... CCC0

				const WORD wMask[5] = { 0xccc0, 0xa2a0, 0xa4a0, 0xa2a0, 0xccc0 };
				if( wMask[y] & (1 << (15 - x)) )
				{
					pBitmap[dwWidth*y + x] |= 0xff808080;
				}
			}
		}
	}
	pCursorSurface->UnlockRect();

	// Set the device cursor
	if( FAILED( hr = pd3dDevice->SetCursorProperties( iconinfo.xHotspot, 
		iconinfo.yHotspot, pCursorSurface ) ) )
	{
		goto End;
	}

	hr = S_OK;

End:
	if( iconinfo.hbmMask != NULL )
		DeleteObject( iconinfo.hbmMask );
	if( iconinfo.hbmColor != NULL )
		DeleteObject( iconinfo.hbmColor );
	if( hdcScreen != NULL )
		ReleaseDC( NULL, hdcScreen );
	if( hdcColor != NULL )
		DeleteDC( hdcColor );
	if( hdcMask != NULL )
		DeleteDC( hdcMask );
	SAFE_DELETE_ARRAY( pcrArrayColor );
	SAFE_DELETE_ARRAY( pcrArrayMask );
	SAFE_RELEASE( pCursorSurface );
	return hr;
}

std::wstring CU_A2U(const std::string& _str)
{
	//！第一次调用得到大小，源字符串的长度
	int	iSize	=	MultiByteToWideChar (CP_ACP, 0, _str.c_str(), -1, NULL, 0);
	//!加一的目的是保存结束字符
	std::wstring	strTemp;
	//!申请同样大的空间来保存转化出来的UNICODE字符串
	strTemp.resize(iSize);
	MultiByteToWideChar(CP_ACP, 0, _str.c_str(), -1, &strTemp[0], iSize);
	return	strTemp;
}
