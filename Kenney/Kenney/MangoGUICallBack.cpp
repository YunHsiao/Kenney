#include "stdafx.h"
#include "Kenney.h"
#include "MangoGUICallBack.h"

extern CGame g_Game;

bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	//检查后台缓冲区格式是否支持Alpha混合等操作(post pixel blending operations)
	IDirect3D9* pD3D = MANGOGetD3DObject(); 
	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
		return false;

	return true;
}

bool CALLBACK ModifyDeviceSettings( MANGODeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	pDeviceSettings->pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//如果不支持硬件顶点处理则使用软件顶点处理
	if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0)
		pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//如果使用参考设备,则弹出警告对话框
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
			MANGODisplaySwitchingToREFWarning();
	}

	return true;
}

HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return g_Game.OnCreateDevice( pd3dDevice,pBackBufferSurfaceDesc,pUserContext );
}

HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return g_Game.OnResetDevice(pd3dDevice,pBackBufferSurfaceDesc,pUserContext);
}

void CALLBACK OnLostDevice( void* pUserContext )
{
	g_Game.OnLostDevice(pUserContext);
}

void CALLBACK OnDestroyDevice( void* pUserContext )
{
	g_Game.OnDestroyDevice(pUserContext);
}

void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_Game.OnFrameMove(pd3dDevice,fTime,fElapsedTime,pUserContext);
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_Game.OnFrameRender( pd3dDevice, fTime, fElapsedTime, pUserContext);
}

void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext )
{
	g_Game.OnGameGUIEvent( nEvent, nControlID, pControl,pUserContext );
}

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	*pbNoFurtherProcessing = g_Game.MsgProc( hWnd, uMsg, wParam, lParam,pbNoFurtherProcessing,pUserContext ) != 0;
	if( *pbNoFurtherProcessing )
		return 1;

	return 0;
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
	g_Game.KeyboardProc(nChar,bKeyDown,bAltDown,pUserContext);
}