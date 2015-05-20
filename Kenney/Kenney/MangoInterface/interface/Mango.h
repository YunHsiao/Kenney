//=============================================================================
/**
 * @file  MangoGUI.h  
 *
 * CreateBy 2011/11/18 12:55
 * 
 * @author Ò¹ÐÐµÄÃ¨×Ð < Heavin.MI > [GameMark @ FoxMail.com]   . CHINA
 * http://blog.csdn.net/m9551             http://m9551.i.sohu.com/blog 
 */
//=============================================================================
#pragma once
#ifndef MANGO_H
#define MANGO_H

#ifndef STRICT
#define STRICT
#endif

// If app hasn't choosen, set to work with Windows 98, Windows Me, Windows 2000, Windows XP and beyond
#ifndef WINVER
#define WINVER         0x0410
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0500 
#endif

// #define MANGO_AUTOLIB to automatically include the libs needed for MANGO 
#ifdef MANGO_AUTOLIB
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxguid.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )
#endif

#pragma warning( disable : 4100 ) // disable unreference formal parameter warnings for /W4 builds

#include <windows.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>      // for placement new
#include <math.h>      
#include <limits.h>      
#include <stdio.h>
#include <XInput.h> // Header for XInput APIs
#include <Tchar.h>
#include <string>

// CRT's memory leak detection
#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#endif

// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) || defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

// strsafe.h deprecates old unsecure string functions.  If you 
// really do not want to it to (not recommended), then uncomment the next line 
//#define STRSAFE_NO_DEPRECATE

#ifndef STRSAFE_NO_DEPRECATE
#pragma deprecated("strncpy")
#pragma deprecated("wcsncpy")
#pragma deprecated("_tcsncpy")
#pragma deprecated("wcsncat")
#pragma deprecated("strncat")
#pragma deprecated("_tcsncat")
#endif

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 

#include "core/MangoGUI.h"
#include "core/MangoHelper.h"
#include "core/D3DEnumeration.h"
#include "core/D3DEnumDeviceInfo.h"
#include "core/D3DEnumAdapterInfo.h"
#include "include/MANGOElement.h "
#include "include/MANGOScene.h"
#include "include/mangoresourcemanager.h"
#include "include/mangocontrol.h"
#include "include/MANGOImage.h"
#include "include/MANGOLable.h"
#include "include/mangobutton.h"
#include "include/mangocheckbox.h"
#include "include/mangoradiobutton.h"
#include "include/mangoscrollbar.h"
#include "include/MANGOListBox.h"
#include "include/mangocombobox.h"
#include "include/mangoslider.h"
#include "include/unibuffer.h"
#include "include/mangoeditbox.h"
#include "include/mangoimeeditbox.h"
#include "include/MANGOSceneManager.h"

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x)           { hr = x; if( FAILED(hr) ) { MANGOTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return MANGOTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#else
#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#endif // !defined(MANGO_H)
