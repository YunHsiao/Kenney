
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

// 为了能查看到D3D一些细节调试信息
#define D3D_DEBUG_INFO

// 添加头文件
#include <d3dx9.h>
#include <process.h>
#include <mmsystem.h>

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <sstream>

#include "tinyxml2/tinyxml2.h"

// 用户定义的消息宏
//#define WM_DIRT        (WM_USER+1)
//#define WM_GRASS       (WM_USER+2)
//#define WM_PLANET      (WM_USER+3)
//#define WM_SAND        (WM_USER+4)
//#define WM_SNOW        (WM_USER+5)
//#define WM_STONE       (WM_USER+6)
//#define WM_ITEM        (WM_USER+7)
//#define WM_TILE        (WM_USER+8)
//#define WM_TILEX	     (WM_USER+9)
//#define WM_ENEMY       (WM_USER+10)
//#define WM_PLAYER      (WM_USER+11)
//#define WM_COLLISION   (WM_USER+12)
//#define WM_TRIGGER     (WM_USER+13)
#define WM_BRUSH       (WM_USER+1)
#define WM_NPC         (WM_USER+2)
#define WM_MONSTER     (WM_USER+3)
#define WM_COLL        (WM_USER+4)
#define WM_TRIG        (WM_USER+5)
#define WM_BACK        (WM_USER+6)
#define WM_DRAWMESH    (WM_USER+7)   // 是否绘制地图网格
#define WM_DRAWRED     (WM_USER+8)   // 是否绘制红色边框
#define WM_BACKALPHA   (WM_USER+9)   // 背景的透明度调节
#define WM_LAYOUTALPHA (WM_USER+10)  // 层级的透明度调节
#define WM_MAPINFO     (WM_USER+11)  // 显示地图信息
#define WM_TRIGMAPID   (WM_USER+12)  // 地图触发ID

// 每页按钮的总数
#ifndef TAB_BUTTON_MAXNUM
#define TAB_BUTTON_MAXNUM  32
#endif

// 鼠标位移的间隔
#ifndef MOUSE_SPACING
#define MOUSE_SPACING      64
#endif

// 背景透明度的初始值
#ifndef BACK_DEFAULT_ALPHAVALUE
#define BACK_DEFAULT_ALPHAVALUE 128
#endif

// 按钮消息投递宏
#ifndef BUTTON_POSTMESSAGE
#define BUTTON_POSTMESSAGE(s1,w)                                    \
        {                                                           \
			if(!b)                                                  \
			{                                                       \
				CWnd* cView = AfxGetMainWnd()->GetWindow(GW_CHILD); \
				::PostMessage(cView->m_hWnd,(s1),(w),(w));          \
			}                                                       \
		}                                                           
#endif

// 添加一些DXUT的宏
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(s) { if(s){(s)->Release();(s)=NULL;} }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(s) { if(s){delete (s);(s)=NULL;} }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(s) { if(s){delete [](s);(s)=NULL;} }
#endif

// STL清理宏
#ifndef SAFE_STLDELETE
#define SAFE_STLDELETE(s)                        \
		{                                        \
			for(size_t i(0);i<(s).size();++i)    \
			{                                    \
				SAFE_DELETE((s)[i]);             \
			}                                    \
			(s).clear();                         \
		}
#endif

// 鼠标层级指示器
enum LAYOUT_CURSOR
{
	CURSOR_BRUSH    = 0,
	CURSOR_NPC      = 1,
	CURSOR_MONSTER  = 2,
	CURSOR_COLL     = 3,
	CURSOR_TRIG     = 4,
	CURSOR_BACK,
	CURSOR_NONE
};

// 地图层级指示器
enum LAYOUT_MAP
{
	MAP_BRUSH    = 1,
	MAP_NPC      = 2,
	MAP_MONSTER  = 3,
	MAP_COLL     = 4,
	MAP_TRIG     = 5,
	MAP_ALL      = 6,
	MAP_NONE     = 7
};


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


