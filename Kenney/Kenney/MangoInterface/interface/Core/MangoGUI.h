//--------------------------------------------------------------------------------------
// File: Utility.h  
//
// Helper functions for Direct3D programming.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------

#ifndef MANGO_GUI_H
#define MANGO_GUI_H

#pragma once

#ifndef UNICODE
#error "MANGO requires a Unicode build. See the nearby comments for details"
#endif

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
class CD3DEnumeration;

struct MANGODeviceSettings
{
	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DFORMAT AdapterFormat;
	DWORD BehaviorFlags;
	D3DPRESENT_PARAMETERS pp;
};

//--------------------------------------------------------------------------------------
// Error codes
//--------------------------------------------------------------------------------------
#define MANGOERR_NODIRECT3D              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0901)
#define MANGOERR_NOCOMPATIBLEDEVICES     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0902)
#define MANGOERR_MEDIANOTFOUND           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0903)
#define MANGOERR_NONZEROREFCOUNT         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0904)
#define MANGOERR_CREATINGDEVICE          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0905)
#define MANGOERR_RESETTINGDEVICE         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0906)
#define MANGOERR_CREATINGDEVICEOBJECTS   MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0907)
#define MANGOERR_RESETTINGDEVICEOBJECTS  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0908)
#define MANGOERR_INCORRECTVERSION        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0909)


//--------------------------------------------------------------------------------------
// Defines and macros 
//--------------------------------------------------------------------------------------
#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_IMAGE_CLICKED                 0x0102
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703


//--------------------------------------------------------------------------------------
// Callback registration 
//--------------------------------------------------------------------------------------
typedef bool    (CALLBACK *LPMANGOCALLBACKISDEVICEACCEPTABLE)( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
typedef bool    (CALLBACK *LPMANGOCALLBACKMODIFYDEVICESETTINGS)( MANGODeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
typedef HRESULT (CALLBACK *LPMANGOCALLBACKDEVICECREATED)( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef HRESULT (CALLBACK *LPMANGOCALLBACKDEVICERESET)( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKDEVICEDESTROYED)( void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKDEVICELOST)( void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKFRAMEMOVE)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKFRAMERENDER)( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKKEYBOARD)( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKMOUSE)( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos, void* pUserContext );
typedef LRESULT (CALLBACK *LPMANGOCALLBACKMSGPROC)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
typedef void    (CALLBACK *LPMANGOCALLBACKTIMER)( UINT idEvent, void* pUserContext );

// Device callbacks
void MANGOSetDeviceCreated( LPMANGOCALLBACKDEVICECREATED pCallbackDeviceCreated, void* pUserContext = NULL );
void MANGOSetDeviceReset( LPMANGOCALLBACKDEVICERESET pCallbackDeviceReset, void* pUserContext = NULL );
void MANGOSetDeviceLost( LPMANGOCALLBACKDEVICELOST pCallbackDeviceLost, void* pUserContext = NULL );
void MANGOSetDeviceDestroyed( LPMANGOCALLBACKDEVICEDESTROYED pCallbackDeviceDestroyed, void* pUserContext = NULL );
void MANGOSetDeviceChanging( LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings, void* pUserContext = NULL );

// Frame callbacks
void MANGOSetFrameMove( LPMANGOCALLBACKFRAMEMOVE pCallbackFrameMove, void* pUserContext = NULL );
void MANGOSetFrameRender( LPMANGOCALLBACKFRAMERENDER pCallbackFrameRender, void* pUserContext = NULL );

// Message callbacks
void MANGOSetKeyboard( LPMANGOCALLBACKKEYBOARD pCallbackKeyboard, void* pUserContext = NULL );
void MANGOSetMouse( LPMANGOCALLBACKMOUSE pCallbackMouse, bool bIncludeMouseMove = false, void* pUserContext = NULL );
void MANGOSetMsgProc( LPMANGOCALLBACKMSGPROC pCallbackMsgProc, void* pUserContext = NULL );


inline int RectWidth( RECT &rc ) { return ( (rc).right - (rc).left ); }
inline int RectHeight( RECT &rc ) { return ( (rc).bottom - (rc).top ); }


class CMANGOXApp
{
public:
	CMANGOXApp(CMANGOXApp * v);
	// MANGOInit
	HRESULT MANGOInit( bool bParseCommandLine = true, bool bHandleDefaultHotkeys = true, bool bShowMsgBoxOnError = true, bool bHandleAltEnter = true );

	// Choose either MANGOCreateWindow or MANGOSetWindow.  If using MANGOSetWindow, consider using MANGOStaticWndProc
	HRESULT MANGOCreateWindow( const WCHAR* strWindowTitle = L"MANGOX Direct3D Window", 
		HINSTANCE hInstance = NULL, HICON hIcon = NULL, HMENU hMenu = NULL,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT );
	HRESULT MANGOSetWindow( HWND hWndFocus, HWND hWndDeviceFullScreen, HWND hWndDeviceWindowed, bool bHandleMessages = true );

	// Choose either MANGOCreateDevice or MANGOSetDevice or MANGOCreateDeviceFromSettings
	HRESULT MANGOCreateDevice( UINT AdapterOrdinal = D3DADAPTER_DEFAULT, bool bWindowed = true, 
		int nSuggestedWidth = 0, int nSuggestedHeight = 0,
		LPMANGOCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable = NULL,
		LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings = NULL, 
		void* pUserContext = NULL );
	HRESULT MANGOCreateDeviceFromSettings( MANGODeviceSettings* pDeviceSettings, bool bPreserveInput = false, bool bClipWindowToSingleAdapter = true );
	HRESULT MANGOSetDevice( IDirect3DDevice9* pd3dDevice );

	// Choose either MANGOMainLoop or implement your own main loop 
	HRESULT MANGOMainLoop( HACCEL hAccel = NULL );

	virtual bool IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext ){return 0;}
	virtual bool ModifyDeviceSettings( MANGODeviceSettings* pDeviceSettings,const D3DCAPS9* pCaps, void* pUserContext ){return 0;}
	virtual HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext ){return 0;}
	virtual HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice,const D3DSURFACE_DESC* pBackBufferSurfaceDesc,void* pUserContext ){return 0;}
	virtual void OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime,float fElapsedTime, void* pUserContext ){};
	virtual void OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime,float fElapsedTime, void* pUserContext ){};
	virtual void OnLostDevice( void* pUserContext ){};
	virtual void OnDestroyDevice( void* pUserContext ){};
};

//--------------------------------------------------------------------------------------
// Initialization
//--------------------------------------------------------------------------------------
HRESULT MANGOInit( bool bParseCommandLine = true, bool bHandleDefaultHotkeys = true, bool bShowMsgBoxOnError = true, bool bHandleAltEnter = true );

// Choose either MANGOCreateWindow or MANGOSetWindow.  If using MANGOSetWindow, consider using MANGOStaticWndProc
HRESULT MANGOCreateWindow( const WCHAR* strWindowTitle = L"Direct3D Window", DWORD dwStyle = WS_OVERLAPPEDWINDOW,
						 HINSTANCE hInstance = NULL, HICON hIcon = NULL, HMENU hMenu = NULL,
						 int x = CW_USEDEFAULT, int y = CW_USEDEFAULT );
HRESULT MANGOSetWindow( HWND hWndFocus, HWND hWndDeviceFullScreen, HWND hWndDeviceWindowed, bool bHandleMessages = true );
LRESULT CALLBACK MANGOStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// Choose either MANGOCreateDevice or MANGOSetDevice or MANGOCreateDeviceFromSettings
HRESULT MANGOCreateDevice( UINT AdapterOrdinal = D3DADAPTER_DEFAULT, bool bWindowed = true, 
						 int nSuggestedWidth = 0, int nSuggestedHeight = 0,
						 LPMANGOCALLBACKISDEVICEACCEPTABLE pCallbackIsDeviceAcceptable = NULL,
						 LPMANGOCALLBACKMODIFYDEVICESETTINGS pCallbackModifyDeviceSettings = NULL, 
						 void* pUserContext = NULL );
HRESULT MANGOCreateDeviceFromSettings( MANGODeviceSettings* pDeviceSettings, bool bPreserveInput = false, bool bClipWindowToSingleAdapter = true );
HRESULT MANGOSetDevice( IDirect3DDevice9* pd3dDevice );

// Choose either MANGOMainLoop or implement your own main loop 
HRESULT MANGOMainLoop( HACCEL hAccel = NULL );

// If not using MANGOMainLoop consider using MANGORender3DEnvironment
void MANGORender3DEnvironment(); 


//--------------------------------------------------------------------------------------
// Finding valid device settings
//--------------------------------------------------------------------------------------
enum MANGO_MATCH_TYPE
{
	MANGOMT_IGNORE_INPUT = 0,  // Use the closest valid value to a default 
	MANGOMT_PRESERVE_INPUT,    // Use input without change, but may cause no valid device to be found
	MANGOMT_CLOSEST_TO_INPUT   // Use the closest valid value to the input 
};

struct MANGOMatchOptions
{
	MANGO_MATCH_TYPE eAdapterOrdinal;
	MANGO_MATCH_TYPE eDeviceType;
	MANGO_MATCH_TYPE eWindowed;
	MANGO_MATCH_TYPE eAdapterFormat;
	MANGO_MATCH_TYPE eVertexProcessing;
	MANGO_MATCH_TYPE eResolution;
	MANGO_MATCH_TYPE eBackBufferFormat;
	MANGO_MATCH_TYPE eBackBufferCount;
	MANGO_MATCH_TYPE eMultiSample;
	MANGO_MATCH_TYPE eSwapEffect;
	MANGO_MATCH_TYPE eDepthFormat;
	MANGO_MATCH_TYPE eStencilFormat;
	MANGO_MATCH_TYPE ePresentFlags;
	MANGO_MATCH_TYPE eRefreshRate;
	MANGO_MATCH_TYPE ePresentInterval;
};

HRESULT MANGOFindValidDeviceSettings( MANGODeviceSettings* pOut, MANGODeviceSettings* pIn = NULL, MANGOMatchOptions* pMatchOptions = NULL );


//--------------------------------------------------------------------------------------
// Common Tasks 
//--------------------------------------------------------------------------------------
void    MANGOSetCursorSettings( bool bShowCursorWhenFullScreen, bool bClipCursorWhenFullScreen );
void    MANGOSetMultimonSettings( bool bAutoChangeAdapter );
void    MANGOSetShortcutKeySettings( bool bAllowWhenFullscreen = false, bool bAllowWhenWindowed = true ); // Controls the Windows key, and accessibility shortcut keys
void    MANGOSetWindowSettings( bool bCallDefWindowProc = true );
void    MANGOSetConstantFrameTime( bool bConstantFrameTime, float fTimePerFrame = 0.0333f );
HRESULT MANGOSetTimer( LPMANGOCALLBACKTIMER pCallbackTimer, float fTimeoutInSecs = 1.0f, UINT* pnIDEvent = NULL, void* pCallbackUserContext = NULL );
HRESULT MANGOKillTimer( UINT nIDEvent );
HRESULT MANGOToggleFullScreen();
HRESULT MANGOToggleREF();
void    MANGOPause( bool bPauseTime, bool bPauseRendering );
void    MANGOResetFrameworkState();
void    MANGOShutdown( int nExitCode = 0 );
std::wstring	CU_A2U(const std::string& _str);

//--------------------------------------------------------------------------------------
// State Retrieval  
//--------------------------------------------------------------------------------------
IDirect3D9*             MANGOGetD3DObject(); // Does not addref unlike typical Get* APIs
IDirect3DDevice9*       MANGOGetD3DDevice(); // Does not addref unlike typical Get* APIs
MANGODeviceSettings      MANGOGetDeviceSettings(); 
D3DPRESENT_PARAMETERS   MANGOGetPresentParameters();
const D3DSURFACE_DESC*  MANGOGetBackBufferSurfaceDesc();
const D3DCAPS9*         MANGOGetDeviceCaps();
HINSTANCE               MANGOGetHINSTANCE();
HWND                    MANGOGetHWND();
HWND                    MANGOGetHWNDFocus();
HWND                    MANGOGetHWNDDeviceFullScreen();
HWND                    MANGOGetHWNDDeviceWindowed();
RECT                    MANGOGetWindowClientRect();
RECT                    MANGOGetWindowClientRectAtModeChange(); // Useful for returning to windowed mode with the same resolution as before toggle to full screen mode
RECT                    MANGOGetFullsceenClientRectAtModeChange(); // Useful for returning to full screen mode with the same resolution as before toggle to windowed mode
double                  MANGOGetTime();
float                   MANGOGetElapsedTime();
bool                    MANGOIsWindowed();
float                   MANGOGetFPS();
LPCWSTR                 MANGOGetWindowTitle();
LPCWSTR                 MANGOGetFrameStats( bool bIncludeFPS = false );
LPCWSTR                 MANGOGetDeviceStats();
bool                    MANGOIsRenderingPaused();
bool                    MANGOIsTimePaused();
bool                    MANGOIsActive();
int                     MANGOGetExitCode();
bool                    MANGOGetShowMsgBoxOnError();
bool                    MANGOGetHandleDefaultHotkeys();
bool                    MANGOIsKeyDown( BYTE vKey ); // Pass a virtual-key code, ex. VK_F1, 'A', VK_RETURN, VK_LSHIFT, etc
bool                    MANGOIsMouseButtonDown( BYTE vButton ); // Pass a virtual-key code: VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2
bool                    MANGOGetAutomation();  // Returns true if -automation parameter is used to launch the app

#endif		//MANGO_GUI_H




