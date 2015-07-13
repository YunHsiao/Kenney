
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

// Ϊ���ܲ鿴��D3DһЩϸ�ڵ�����Ϣ
#define D3D_DEBUG_INFO

// ���ͷ�ļ�
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

// �û��������Ϣ��
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
#define WM_DRAWMESH    (WM_USER+7)   // �Ƿ���Ƶ�ͼ����
#define WM_DRAWRED     (WM_USER+8)   // �Ƿ���ƺ�ɫ�߿�
#define WM_BACKALPHA   (WM_USER+9)   // ������͸���ȵ���
#define WM_LAYOUTALPHA (WM_USER+10)  // �㼶��͸���ȵ���
#define WM_MAPINFO     (WM_USER+11)  // ��ʾ��ͼ��Ϣ
#define WM_TRIGMAPID   (WM_USER+12)  // ��ͼ����ID

// ÿҳ��ť������
#ifndef TAB_BUTTON_MAXNUM
#define TAB_BUTTON_MAXNUM  32
#endif

// ���λ�Ƶļ��
#ifndef MOUSE_SPACING
#define MOUSE_SPACING      64
#endif

// ����͸���ȵĳ�ʼֵ
#ifndef BACK_DEFAULT_ALPHAVALUE
#define BACK_DEFAULT_ALPHAVALUE 128
#endif

// ��ť��ϢͶ�ݺ�
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

// ���һЩDXUT�ĺ�
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(s) { if(s){(s)->Release();(s)=NULL;} }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(s) { if(s){delete (s);(s)=NULL;} }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(s) { if(s){delete [](s);(s)=NULL;} }
#endif

// STL�����
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

// ���㼶ָʾ��
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

// ��ͼ�㼶ָʾ��
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


