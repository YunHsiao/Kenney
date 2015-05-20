#include "stdafx.h"
#include "Kenney.h"
#include "MangoGUICallBack.h"
#include "SceneManager.h"

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
//,m_pUILogo(NULL)
//,m_pUILogin(NULL)
//,m_pUIChoose(NULL)
{
}

CGame::~CGame(void)
{
	//SAFE_DELETE(m_pUILogo);
	//SAFE_DELETE(m_pUILogin);
	//SAFE_DELETE(m_pUIChoose);
}

void CGame::Initialize()
{
	// ��ʼ��UI
	MANGOSetDeviceCreated( ::OnCreateDevice );
	MANGOSetDeviceReset( ::OnResetDevice );
	MANGOSetDeviceLost( ::OnLostDevice );
	MANGOSetDeviceDestroyed( ::OnDestroyDevice );

	MANGOSetFrameMove( ::OnFrameMove );
	MANGOSetFrameRender( ::OnFrameRender );

	MANGOSetMsgProc( ::MsgProc );
	MANGOSetKeyboard( ::KeyboardProc );

	InitGame();

	MANGOInit( true, true, true );
	MANGOSetCursorSettings( true, true );

	MANGOCreateWindow( NAMETITLE ,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);
	MANGOCreateDevice( D3DADAPTER_DEFAULT, true, CLIENT_WIDTH, CLIENT_HEIGHT, ::IsDeviceAcceptable, ::ModifyDeviceSettings );

	// ����ͼ��
	HICON hIcon = LoadIcon(MANGOGetHINSTANCE(), MAKEINTRESOURCE(IDI_KENNEY));
	SendMessage(MANGOGetHWND(), WM_SETICON, ICON_BIG,  (LPARAM)hIcon);
	hIcon = LoadIcon(MANGOGetHINSTANCE(), MAKEINTRESOURCE(IDI_SMALL));
	SendMessage(MANGOGetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
}

void CGame::GameRun()
{
	MANGOMainLoop();
}

int CGame::GameExit()
{
	return MANGOGetExitCode();
}

//--------------------------------------------------------------------------------------

void CGame::InitGame()
{
	m_SceneManager.SetCallback(::OnGUIEvent);

	//m_pUILogo = new CUILogo;
	//m_pUILogo->Initialize(m_SceneManager);

	//m_pUILogin = new CUILogin;
	//m_pUILogin->Initialize(m_SceneManager);

	//m_pUIChoose = new CUIChoose;
	//m_pUIChoose->Initialize(m_SceneManager);
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
	return hr;
}

HRESULT CGame::OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,	void* pUserContext )
{
	HRESULT hr;
	V_RETURN( m_SceneManager.OnResetDevice() );

	//���ù۲����
	D3DXMATRIX matView;
	D3DXVECTOR3 vEyePt( 0.f, 0.f, -1.f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//����ͶӰ����
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
	Tick(fElapsedTime);
}

void CGame::OnFrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	HRESULT hr;
	//�����̨��ɫ����������Ȼ�����
	V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100 ,100 ,100), 1.0f, 0));

	//��Ⱦ����
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		MANGO_BeginPerfEvent( MANGO_PERFEVENTCOLOR, L"HUD / Stats" );
		m_SceneManager.OnRender(fElapsedTime);
		MANGO_EndPerfEvent();

		V(pd3dDevice->EndScene());
	}
}

void CGame::OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext)
{
	//int iScene = (int)pUserContext;
	//switch (iScene)
	//{
	//case EUI_LOGIN_BG:
	//case EUI_LOGIN_HELP:
	//case EUI_LOGIN:
	//	m_pUILogin->OnGameGUIEvent(nEvent, nControlID, pControl, pUserContext);
	//	break;

	//case EUI_CHOOSE:
	//	m_pUIChoose->OnGameGUIEvent(nEvent, nControlID, pControl, pUserContext);
	//	break;
	//}
}

void CGame::KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
}

LRESULT CGame::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,bool* pbNoFurtherProcessing, void* pUserContext)
{
	//*pbNoFurtherProcessing = m_pUILogo->MsgProc(hWnd, uMsg, wParam, lParam) != 0;
	//if( *pbNoFurtherProcessing )
	//	return 1;

	*pbNoFurtherProcessing = m_SceneManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 1;

	return 0;
}

#pragma endregion CallBackFunction

//-------------------------------------------------------------------------------------------