#include "stdafx.h"
#include "Kenney.h"
#include "MangoGUICallBack.h"
#include "SceneManager.h"
#include "NPCDialog.h"
#include "DlgDefine.h"
#include "TaskDialog.h"
#include "ShopDialog.h"
#include "Goods.h"

#define NAMETITLE TEXT("Kenney")

CGame g_Game;

//--------------------------------------------------------------------------------------

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_Game.Initialize();
	g_Game.GameRun();
	return g_Game.GameExit();
}

//--------------------------------------------------------------------------------------

CGame::CGame(void)
{

}

CGame::~CGame(void)
{

}

void CGame::Initialize()
{
	// 初始化UI
	MANGOSetDeviceCreated( ::OnCreateDevice );
	MANGOSetDeviceReset( ::OnResetDevice );
	MANGOSetDeviceLost( ::OnLostDevice );
	MANGOSetDeviceDestroyed( ::OnDestroyDevice );

	MANGOSetFrameMove( ::OnFrameMove );
	MANGOSetFrameRender( ::OnFrameRender );

	MANGOSetMsgProc( ::MsgProc );
	MANGOSetKeyboard( ::KeyboardProc );

	MANGOInit( true, true, true );
	MANGOSetCursorSettings( true, true );

	MANGOCreateWindow( NAMETITLE ,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);
	MANGOCreateDevice( D3DADAPTER_DEFAULT, true, CLIENT_WIDTH, CLIENT_HEIGHT, ::IsDeviceAcceptable, ::ModifyDeviceSettings );

	// 设置图标
	HICON hIcon = LoadIcon(MANGOGetHINSTANCE(), MAKEINTRESOURCE(IDI_KENNEY));
	SendMessage(MANGOGetHWND(), WM_SETICON, ICON_BIG,  (LPARAM)hIcon);
	hIcon = LoadIcon(MANGOGetHINSTANCE(), MAKEINTRESOURCE(IDI_SMALL));
	SendMessage(MANGOGetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	HCURSOR hCursor = LoadCursor(MANGOGetHINSTANCE(), MAKEINTRESOURCE(IDC_CURSOR2));
	::SetClassLong(MANGOGetHWND(), GCL_HCURSOR, (long)hCursor);
}

void CGame::GameRun()
{
	MANGOMainLoop();
}

int CGame::GameExit()
{
	CSceneManager::GetIntance()->Destroy();
	return MANGOGetExitCode();
}

//--------------------------------------------------------------------------------------

void CGame::InitGame()
{
	m_SceneManager.SetCallback(::OnGUIEvent);
	if (!CSceneManager::GetIntance()->Initialize(MANGOGetD3DDevice(), &m_SceneManager))
		return;
}

bool CGame::Tick(float fElapsedTime)
{
	CSceneManager::GetIntance()->Tick(fElapsedTime);
	return true;
}

//-------------------------------------------------------------------------------------------

#pragma region CallBackFunction

HRESULT CGame::OnCreateDevice( IDirect3DDevice9* pd3dDevice,const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;
	V_RETURN( m_SceneManager.OnCreateDevice( pd3dDevice ) );
	InitGame();
	return hr;
}

HRESULT CGame::OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,	void* pUserContext )
{
	HRESULT hr;
	V_RETURN( m_SceneManager.OnResetDevice() );

	//设置观察矩阵
	D3DXMATRIX matView;
	D3DXVECTOR3 vEyePt( 0.f, 0.f, -1.f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//设置投影矩阵
	D3DXMATRIXA16 matProj;
	float fAspectRatio = (float)pBackBufferSurfaceDesc->Width / pBackBufferSurfaceDesc->Height;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 1.0f, 100.0f );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	return S_OK;
}

void CGame::OnLostDevice(void* pUserContext)
{
	m_SceneManager.OnLostDevice();
}

void CGame::OnDestroyDevice(void* pUserContext)
{
	m_SceneManager.OnDestroyDevice();
}

void CGame::OnFrameMove(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	Tick(fElapsedTime>.1f?.1f:fElapsedTime);
}

void CGame::OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	HRESULT hr;
	//清除后台颜色缓冲区和深度缓冲区
	V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100 ,100 ,100), 1.0f, 0));

	//渲染场景
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		MANGO_BeginPerfEvent( MANGO_PERFEVENTCOLOR, L"HUD / Stats" );
		CSceneManager::GetIntance()->Render();
		m_SceneManager.OnRender(fElapsedTime);
		MANGO_EndPerfEvent();

		V(pd3dDevice->EndScene());
	}
}

void CGame::OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext)
{
	int iScene = (int)pUserContext;
	switch(iScene)
	{
	case ED_NPCDialog:
		CNPCDialog::GetInstance()->OnGameGUIEvent(nEvent,nControlID,pControl,pUserContext);
		break;
	case ED_TaskDialog:
		CTaskDialog::GetInstance()->OnGameGUIEvent(nEvent,nControlID,pControl,pUserContext);
		break;
	case ED_ShopDialog:
		CShopDialog::GetInstance()->OnGameGUIEvent(nEvent,nControlID,pControl,pUserContext);
		break;
	case ED_Goods:
		CGoods::GetInstance()->OnGameGUIEvent(nEvent,nControlID,pControl,pUserContext);
		break;
	}
}

void CGame::KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
}

LRESULT CGame::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,bool* pbNoFurtherProcessing, void* pUserContext)
{
	*pbNoFurtherProcessing = m_SceneManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 1;

	return 0;
}

#pragma endregion CallBackFunction

//-------------------------------------------------------------------------------------------