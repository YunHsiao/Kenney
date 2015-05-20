// MapEditView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MapEditor.h"
#include "MapEditorView.h"

#include "ToolLoadDialog.h"
#include"ToolBrushDialog.h"

// ȫ�ֶ��߳���Դ����
LPDIRECT3DDEVICE9		_pd3dDevice  = NULL;
LPD3DXLINE				_pLine       = NULL;
LPD3DXFONT				_pd3dFont    = NULL;
HWND					_pHwnd       = NULL;
LPDIRECT3DTEXTURE9**	g_pppTexture = NULL;
extern CDrawBrush*		g_pDrawBrush = NULL;		// �㼶ͼƬ
INT						g_nMapLayout = 0;			// ��ͼ�����㼶


// D3D��صĺ���
HRESULT InitD3D(
				D3DPRESENT_PARAMETERS* d3dpp,
				LPDIRECT3DDEVICE9* pd3dDevice,
				HWND hWnd, 
				UINT nWidth, 
				UINT nHeight, 
				BOOL bWnd, 
				D3DDEVTYPE emDeviceType)
{
	HRESULT hr = S_OK;

	// ��ʼ��D3D
	LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		::MessageBox(0, _T("��ʼ��D3D9ʧ��!"), 0, 0);
		return E_FAIL;
	}

	// D3D���
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, emDeviceType, &caps);

	// �жϵ�ǰ�Կ��Ƿ�֧��Ӳ�����㴦��
	int vp = 0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// ��ʼ����ز���
	d3dpp->BackBufferWidth				= nWidth;							// ��̨�������Ŀ��
	d3dpp->BackBufferHeight				= nHeight;							// ��̨�������ĸ߶�
	d3dpp->BackBufferFormat				= D3DFMT_UNKNOWN;					// ��̨�����������ظ�ʽ
	d3dpp->BackBufferCount				= 1;								// ��̨������������
	d3dpp->MultiSampleType				= D3DMULTISAMPLE_NONE;				// ���ز���������
	d3dpp->MultiSampleQuality			= 0;								// ���ز���������
	d3dpp->SwapEffect					= D3DSWAPEFFECT_DISCARD;			// ��̨��������ҳ���û���ʽ
	d3dpp->hDeviceWindow				= hWnd;								// ���ھ��
	d3dpp->Windowed						= bWnd;								// ���ڻ���ȫ��
	d3dpp->EnableAutoDepthStencil		= TRUE;								// �����ģ�建��
	d3dpp->AutoDepthStencilFormat		= D3DFMT_D16;						// ��Ȼ�����ģ�建������ظ�ʽ
	d3dpp->Flags						= 0;								// ���ӵ�����
	d3dpp->FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;			// ˢ��Ƶ��
	d3dpp->PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;	// ˢ��ʱ����(��ֱͬ��)


	hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,
		 emDeviceType,
		 hWnd,
		 vp,
		 d3dpp,
		 pd3dDevice);

	if(FAILED(hr))
	{
		if(IDOK == ::MessageBox(0, _T("��ǰ��ʾģʽ��֧��,�Ƿ����ð�ȫģʽ?"), 0, MB_YESNO))
		{
			// ���ʧ��,��ʹ�ð�ȫģʽ
			hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,
				 emDeviceType,
				 hWnd,
				 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				 d3dpp,
				 pd3dDevice);

			if(FAILED(hr))
			{
				SAFE_RELEASE(d3d9);
				::MessageBox(0, _T("����D3D�豸ʧ��!"), 0, 0);
				return hr;
			}
		}
		else
		{
			return hr;
		}
	}

	SAFE_RELEASE(d3d9); // ��ȫ�ͷ�
	return hr;
}

HRESULT Init(HWND hWnd, UINT nWidth, UINT nHeight, BOOL bWnd, D3DDEVTYPE emDeviceType)
{
	HRESULT hr = S_OK;

	// ��̬���ʼ��D3D
	D3DPRESENT_PARAMETERS d3dpp;

	if(FAILED(hr = InitD3D(&d3dpp,&_pd3dDevice,hWnd,nWidth,nHeight,bWnd,emDeviceType)))
	{
		::MessageBox(0, _T("��̬��ʼ��D3Dʧ��!"), 0, 0);
		return hr;
	}

	// �����߿�
	if(FAILED(hr = D3DXCreateLine(_pd3dDevice,&_pLine)))
	{
		::MessageBox(0, _T("�����߿�ʧ��!"), 0, 0);
		return hr;
	}

	// ��������
	if(FAILED(hr = D3DXCreateFont(_pd3dDevice,16,0,FW_BOLD,0,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"����",&_pd3dFont)))
	{
		::MessageBox(0, _T("��������ʧ��!"), 0, 0);
		return hr;
	}

	return hr;
}

HRESULT InitResource(void)
{
	HRESULT hr = S_OK;

	// ����Ԫ��ͼƬ(����ϵĸ���ͼƬ)

	// XML����
	tinyxml2::XMLDocument tFile;
	if(tFile.LoadFile("Res/LoadResource.xml"))
	{
		AfxMessageBox(L"Res/LoadResource.xml ����ʧ��!");
		return E_FAIL;
	}

	tinyxml2::XMLElement* tRoot = tFile.FirstChildElement();
	tinyxml2::XMLElement* tNode = tRoot->FirstChildElement();
	g_nMapLayout = atoi(tNode->FirstChild()->Value());
	tNode = tNode->NextSiblingElement();
	tinyxml2::XMLElement* tData;

	// ��ʼ��ȫ�ֱ���
	g_pDrawBrush = new CDrawBrush[g_nMapLayout];
	g_pppTexture = new LPDIRECT3DTEXTURE9*[g_nMapLayout];

	// ͼƬ��Դ����
	for(int i = 0; i < g_nMapLayout; ++i)
	{
		int cur(0), mx(atoi(tNode->Attribute("num")));
		tData = tNode->FirstChildElement();
		g_pppTexture[i] = new LPDIRECT3DTEXTURE9[mx];
		do {
		LPCSTR str = tData->Attribute("path");
		LPWSTR buf = new WCHAR[strlen(str)+1];
		MultiByteToWideChar(CP_ACP,0,str,-1,buf,(int)strlen(str)+1);

		if(i!=(g_nMapLayout - 1)) // Ԫ��ͼƬ
		{
			int maxNum = atoi(tData->Attribute("num"));

			for(int k(0); k < maxNum; ++k)
			{
				size_t size = wcslen(buf)+1;
				LPWSTR str  = new WCHAR[size];
				swprintf_s(str,size,buf,(k+1));

				hr = D3DXCreateTextureFromFileEx(_pd3dDevice, str,
					D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 
					D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
					D3DX_FILTER_LINEAR, D3DX_FILTER_NONE,
					NULL, NULL, NULL,
					&g_pppTexture[i][cur+k]);

				SAFE_DELETE_ARRAY(str); // �ַ������ͷ�

				if(FAILED(hr))
				{
					::MessageBox(0, L"��ˢͼƬ����ʧ��!", 0, 0);
					return hr;
				}
			}
			cur += maxNum;
		}
		else // ����ͼƬ
		{
			int maxNum = atoi(tData->Attribute("num"));

			for(int k(0); k < maxNum; ++k)
			{
				size_t size = wcslen(buf)+1;
				LPWSTR str  = new WCHAR[size];
				swprintf_s(str,size,buf,(k+1));

				hr = D3DXCreateTextureFromFileEx(_pd3dDevice, str,
					D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 
					D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
					D3DX_FILTER_LINEAR, D3DX_FILTER_NONE,
					NULL, NULL, NULL,
					&g_pppTexture[i][cur+k]);

				SAFE_DELETE_ARRAY(str); // �ַ������ͷ�

				if(FAILED(hr))
				{
					::MessageBox(0, L"����ͼƬ����ʧ��!", 0, 0);
					return hr;
				}
			}
			cur += maxNum;			
		}

		SAFE_DELETE_ARRAY(buf);
		
		} while (tData = tData->NextSiblingElement());

		if (i < (g_nMapLayout - 3)) {
			if(FAILED(hr = g_pDrawBrush[i].OnCreateByPoint(_pd3dDevice,g_pppTexture[i],mx)))
				return hr;
		} else {	// No Scaling
			if(FAILED(hr = g_pDrawBrush[i].OnCreateByPoint(_pd3dDevice,g_pppTexture[i],mx, 
				FALSE, FALSE)))	return hr;
		}
		tNode = tNode->NextSiblingElement();
	}

	g_pDrawBrush[CURSOR_BACK].SetTextureColor(D3DCOLOR_ARGB(BACK_DEFAULT_ALPHAVALUE,255,255,255));
	return hr;
}

// ������Դ���߳�
void ThreadProc(LPVOID pParams)
{
	// ��ʼ��D3D����
	HRESULT hr = S_OK;

	hr = Init(_pHwnd, 800, 600, TRUE, D3DDEVTYPE_HAL);

	if(FAILED(hr))
	{
		AfxMessageBox(L"��ʼ��D3D����ʧ��!");
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_QUIT,0,0);
		_endthread();
	}
	// ��ʼ��D3D��Դ
	if(FAILED(hr = InitResource()))
	{
		AfxMessageBox(L"��Դ����ʧ��!");
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_QUIT,0,0);
		_endthread();
	}

	return _endthread();
}

//----------------------------------------------------------------------------

// CMapEditView

IMPLEMENT_DYNCREATE(CMapEditView, CView)

CMapEditView::CMapEditView()
:m_bIsNewMap       (FALSE)
,m_bIsLBotton      (FALSE)
,m_bIsInitResruce  (FALSE)
,m_bIsRedLine      (TRUE)
,m_bIsLine         (TRUE)
,m_bIsMapInfo      (TRUE)
,m_emCursorLayout  (CURSOR_NONE)
,m_emMapLayout     (MAP_BRUSH)
,m_nLayoutAlpha    (255)
,m_nTriggerNum     (0)
,m_nMapID          (0)
,m_pDrawMapColl    (NULL)
,m_pMapTest        (NULL)
,m_nTrigMapID      (0)
,m_nSelTriggerid   (-1) 
{
	m_nCurrentCoord.x = 0;
	m_nCurrentCoord.y = 0;
	m_nStartCoord.x = 0;
	m_nStartCoord.y = 0;
	m_nMapCoord.x = 0;
	m_nMapCoord.y = 0;
	m_nMapPrevCoord.x = 0;
	m_nMapPrevCoord.y = 0;
	m_nDefaultCoord.x = 0;
	m_nDefaultCoord.y = 0;
}

CMapEditView::~CMapEditView()
{
	// ���ٵ�ʱ�����
	SAFE_RELEASE(_pLine);
	SAFE_RELEASE(_pd3dDevice);
	SAFE_RELEASE(_pd3dFont);

	// ���STL���ڴ�й©                     
	SAFE_STLDELETE(m_stdMapBrushElement);
	SAFE_STLDELETE(m_stdMapNpcElement);
	SAFE_STLDELETE(m_stdMapMonsterElement);
	SAFE_STLDELETE(m_stdMapTrigElement);

	SAFE_DELETE(m_pDrawMapColl);
	SAFE_DELETE(m_pMapTest);

	// ȫ��ָ����ͷ�
	for(int i(0);i < g_nMapLayout;++i)
	{
		for(int k(0); k < g_pDrawBrush[i].GetTextureMaxNum();++k)
			SAFE_RELEASE(g_pppTexture[i][k]);

		SAFE_DELETE_ARRAY(g_pppTexture[i]);
	}

	SAFE_DELETE_ARRAY(g_pppTexture);
	SAFE_DELETE_ARRAY(g_pDrawBrush);  
}

BEGIN_MESSAGE_MAP(CMapEditView, CView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_EDIT_UNDO,     &CMapEditView::OnEditUndo)
	ON_COMMAND(ID_FILE_SAVE,     &CMapEditView::OnFileSave)
	ON_COMMAND(ID_FILE_NEW,      &CMapEditView::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN,     &CMapEditView::OnFileOpen)
	ON_COMMAND(ID_FILE_CLOSE,    &CMapEditView::OnFileClose)

	ON_COMMAND(ID_LAYOUTBRUSH,   &CMapEditView::OnLayoutBrush)
	ON_COMMAND(ID_LAYOUTNPC,     &CMapEditView::OnLayoutNpc)
	ON_COMMAND(ID_LAYOUTMONSTER, &CMapEditView::OnLayoutMonster)
	ON_COMMAND(ID_LAYOUTCOLL,    &CMapEditView::OnLayoutColl)
	ON_COMMAND(ID_LAYOUTTRIGGER, &CMapEditView::OnLayoutTrigger)
	ON_COMMAND(ID_LAYOUTALL,     &CMapEditView::OnLayoutAll)

	ON_COMMAND(ID_BACKSELECT, &CMapEditView::OnBackselect)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_MAPTEST, &CMapEditView::OnMaptest)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_32818, &CMapEditView::OnCollLerp)
END_MESSAGE_MAP()


// CMapEditView ��ͼ

void CMapEditView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// CMapEditView ���

#ifdef _DEBUG
void CMapEditView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE

void CMapEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif
#endif //_DEBUG


// CMapEditView ��Ϣ�������
int CMapEditView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	_pHwnd = GetSafeHwnd();

	//_beginthread(ThreadProc,0,0);
	//CToolLoadDialog* pNew = new CToolLoadDialog;
	//pNew->DoModal();
	//SAFE_DELETE(pNew);

	// ��ʼ��D3D����
	HRESULT hr = Init(_pHwnd, 800, 600, TRUE, D3DDEVTYPE_HAL);
	if(FAILED(hr))
	{
		AfxMessageBox(L"��ʼ��D3D����ʧ��!");
		::SendMessage(_pHwnd, WM_QUIT, 0, 0);
	}

	// ��ʼ��D3D��Դ
	if(FAILED(hr = InitResource()))
	{
		AfxMessageBox(L"��Դ����ʧ��!");
		::SendMessage(_pHwnd, WM_QUIT, 0, 0);
	}

	return 0;
}

void CMapEditView::OnInitialUpdate()
{
	CView::OnInitialUpdate(); 

	AfxGetMainWnd()->MoveWindow((::GetSystemMetrics(SM_CXSCREEN) - g_nAppWidth) / 2,
		(::GetSystemMetrics(SM_CYSCREEN) - g_nAppHeight) / 2, 
		g_nWndWidth, g_nWndHeight);
	//AfxGetMainWnd()->CenterWindow();

	m_pMapTest = new CDialogTest;
	m_pMapTest->Create(IDD_DIALOGTEST,AfxGetMainWnd());

	ShowWindow(SW_SHOW);
	UpdateWindow();
}

void CMapEditView::SetMapLayoutColor(D3DCOLOR brush,D3DCOLOR npc,D3DCOLOR monster,D3DCOLOR coll,D3DCOLOR trig)
{
	if(m_bIsNewMap)
	{
		// �Բ㼶��ɫ��ʵʱ����
		for(size_t i(0);i<m_stdMapBrushElement.size();++i)
			m_stdMapBrushElement[i]->SetTextureColor(brush);

		for(size_t i(0);i<m_stdMapNpcElement.size();++i)
			m_stdMapNpcElement[i]->SetTextureColor(npc);
		
		for(size_t i(0);i<m_stdMapMonsterElement.size();++i)
			m_stdMapMonsterElement[i]->SetTextureColor(monster);
		
		if( m_pDrawMapColl )
			m_pDrawMapColl->SetTextureColor(coll);

		for(size_t i(0);i<m_stdMapTrigElement.size();++i)
			m_stdMapTrigElement[i]->SetTextureColor(trig);
	}
}

HRESULT CMapEditView::Update(void)
{
	HRESULT hr = S_OK;

	// ��ɫ��˵��Ĺ�ѡ״̬ʵʱ����
	CMenu* pSub = AfxGetMainWnd()->GetMenu()->GetSubMenu(2);
	D3DCOLOR colorAll = D3DCOLOR_ARGB(255,255,255,255);
	D3DCOLOR colorCha = D3DCOLOR_ARGB(m_nLayoutAlpha,32,32,32);
	if(m_bIsNewMap)
	{
		if(m_emMapLayout == MAP_BRUSH)
		{
			SetMapLayoutColor(colorAll,colorCha,colorCha,colorCha,colorCha);
			for(int i(0);i < g_nMapLayout;++i)
			{
				if(i == 0)
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
				else
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_UNCHECKED);
			}
		}
		else if(m_emMapLayout == MAP_NPC)
		{
			SetMapLayoutColor(colorCha,colorAll,colorCha,colorCha,colorCha);
			for(int i(0);i < g_nMapLayout;++i)
			{
				if(i == 1)
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
				else
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_UNCHECKED);
			}
		}
		else if(m_emMapLayout == MAP_MONSTER)
		{
			SetMapLayoutColor(colorCha,colorCha,colorAll,colorCha,colorCha);
			for(int i(0);i < g_nMapLayout;++i)
			{
				if(i == 2)
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
				else
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_UNCHECKED);
			}
		}
		else if(m_emMapLayout == MAP_COLL)
		{
			SetMapLayoutColor(colorCha,colorCha,colorCha,colorAll,colorCha);
			for(int i(0);i < g_nMapLayout;++i)
			{
				if(i == 3)
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
				else
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_UNCHECKED);
			}
		}
		else if(m_emMapLayout == MAP_TRIG)
		{
			SetMapLayoutColor(colorCha,colorCha,colorCha,colorCha,colorAll);
			for(int i(0);i < g_nMapLayout;++i)
			{
				if(i == 4)
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
				else
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_UNCHECKED);
			}
		}
		else if(m_emMapLayout == MAP_ALL)
		{
			SetMapLayoutColor(colorAll,colorAll,colorAll,colorAll,colorAll);
			for(int i(0);i < g_nMapLayout;++i)
			{
				if(i == 5)
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
				else
					pSub->CheckMenuItem(i,MF_BYPOSITION|MF_UNCHECKED);
			}
		}
	}

	return hr;
}

HRESULT CMapEditView::RenderText()
{
	HRESULT hr = S_OK;

	std::wstringstream sstr;
	sstr << L"��ͼID:";
	sstr << m_nMapID;
	sstr << L"\n��ͼ����:";
	sstr << m_szMapName.c_str();
	sstr << L"\n��ͼ����: (";
	sstr << m_nMapCoord.x;
	sstr << L", ";
	sstr <<	m_nMapCoord.y;
	sstr << L")";
	sstr << L"\n�������: (";
	sstr << m_nCurrentCoord.x;
	sstr << L", ";
	sstr <<	m_nCurrentCoord.y;
	sstr << L")";
	sstr << L"\n��������: (";
	sstr << m_nMapCoord.x + m_nCurrentCoord.x;
	sstr << L", ";
	sstr <<	m_nMapCoord.y + m_nCurrentCoord.y;
	sstr << L")";
	_pd3dFont->DrawText(NULL,sstr.str().c_str(),-1,0,D3DFMT_UNKNOWN,0xFFFF0000);
	return hr;
}

void CMapEditView::RenderLine()
{
	if(TRUE == m_bIsLine)
	{
		POINT offset;
		offset.x = m_nMapCoord.x/MOUSE_SPACING*(MOUSE_SPACING+1) - m_nMapCoord.x;
		offset.y = m_nMapCoord.y/MOUSE_SPACING*(MOUSE_SPACING+1) - m_nMapCoord.y;
		_pLine->Begin();
		for(FLOAT row(0);row<13;++row)
		{
			D3DXVECTOR2 vecLine[2] = 
			{
				D3DXVECTOR2(offset.x + row*MOUSE_SPACING, 0.f),
				D3DXVECTOR2(offset.x + row*MOUSE_SPACING, 600.f)
			};
			_pLine->Draw(vecLine, 2, 0x4000FF00);
		}
		for(FLOAT col(0);col<10;++col)
		{
			D3DXVECTOR2 vecLine[2] = 
			{
				D3DXVECTOR2(0.f, offset.y + col*MOUSE_SPACING),
				D3DXVECTOR2(800.f, offset.y + col*MOUSE_SPACING)
			};
			_pLine->Draw(vecLine, 2, 0x4000FF00);
		}
		_pLine->End();
	}
}

void CMapEditView::Clear()
{
	m_bIsNewMap			=  FALSE;
	m_bIsLBotton		=  FALSE;
	m_bIsInitResruce	=  FALSE;
	m_bIsRedLine		=  TRUE;
	m_bIsLine           =  TRUE;
	m_bIsMapInfo		=  TRUE;
	m_emCursorLayout	=  CURSOR_NONE;
	m_emMapLayout       =  MAP_BRUSH;
	m_nLayoutAlpha		= 255;
	m_nTriggerNum		= 0;
	m_nMapID            = 0;
	m_nTrigMapID		= 0;
	m_nSelTriggerid		= -1; 
	m_nMapCoord.x		= 0;
	m_nMapCoord.y		= 0;
	//m_pDrawMapColl      = NULL;
}

HRESULT CMapEditView::Render()
{
	HRESULT hr = S_OK;
	
	if(_pd3dDevice)
	{
		_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		_pd3dDevice->BeginScene();

		if(m_bIsNewMap)
		{
			POINT copy = m_nMapCoord;
			// ���Ʊ�����
			g_pDrawBrush[CURSOR_BACK].Render(NULL,copy,FALSE);
			copy.x -= g_pDrawBrush[CURSOR_BACK].GetTextureWidth();
			g_pDrawBrush[CURSOR_BACK].Render(NULL,copy,FALSE);
			copy.x -= g_pDrawBrush[CURSOR_BACK].GetTextureWidth();
			g_pDrawBrush[CURSOR_BACK].Render(NULL,copy,FALSE);

			// ������
			RenderLine();
			_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
			_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

			// ����Ԫ�ز�
			for(size_t i(0);i<m_stdMapBrushElement.size();++i)
			{
				if(m_bIsRedLine)
					m_stdMapBrushElement[i]->Render(_pLine,m_nMapCoord,FALSE);
				else
					m_stdMapBrushElement[i]->Render(NULL,m_nMapCoord,FALSE);
			}

			// ����NPC��
			for(size_t i(0);i<m_stdMapNpcElement.size();++i)
			{
				if(m_bIsRedLine)
					m_stdMapNpcElement[i]->Render(_pLine,m_nMapCoord,FALSE);
				else
					m_stdMapNpcElement[i]->Render(NULL,m_nMapCoord,FALSE);
			}

			// ���ƹ����
			for(size_t i(0);i<m_stdMapMonsterElement.size();++i)
			{
				if(m_bIsRedLine)
					m_stdMapMonsterElement[i]->Render(_pLine,m_nMapCoord,FALSE);
				else
					m_stdMapMonsterElement[i]->Render(NULL,m_nMapCoord,FALSE);
			}

			// ���Ƶ�ͼ��ײ��
			if(m_pDrawMapColl)
				m_pDrawMapColl->Render(m_nMapCoord);

			// ���ƴ�����
			for(size_t i(0);i<m_stdMapTrigElement.size();++i)
				m_stdMapTrigElement[i]->Render(NULL,m_nMapCoord,FALSE);

			if(m_emMapLayout != MAP_ALL)
			{
				// ��������ϵ�ͼ��
				if(g_pDrawBrush && (m_emCursorLayout != CURSOR_NONE))
				{
					if(m_emCursorLayout == CURSOR_NPC)
						g_pDrawBrush[m_emCursorLayout].SetTextureColor(D3DCOLOR_ARGB(255,0,0,255));
					else if(m_emCursorLayout == CURSOR_MONSTER)
						g_pDrawBrush[m_emCursorLayout].SetTextureColor(D3DCOLOR_ARGB(255,0,255,0));
					else
						g_pDrawBrush[m_emCursorLayout].SetTextureColor(D3DCOLOR_ARGB(255,255,0,0));

					// ��ײ�򴥷���Ҫ��С��
					if(m_emCursorLayout == CURSOR_COLL || m_emCursorLayout == CURSOR_TRIG)
						g_pDrawBrush[m_emCursorLayout].Render(NULL,m_nMapCoord,FALSE);
					else
						g_pDrawBrush[m_emCursorLayout].Render(_pLine,m_nMapCoord,FALSE);
				}
			}

			// ���Ƶ�ͼ��Ϣ
			if(m_bIsMapInfo)
				RenderText();
		}

		_pd3dDevice->EndScene();

		_pd3dDevice->Present(0, 0, 0, 0);
	}

	return hr;
}

void CMapEditView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_nStartCoord = point;
	m_nMapPrevCoord = m_nMapCoord;
	CView::OnMButtonDown(nFlags, point);
}

void CMapEditView::OnMButtonUp(UINT nFlags, CPoint point)
{
	CView::OnMButtonUp(nFlags, point);
}

void CMapEditView::OnMouseMove(UINT nFlags, CPoint point) {
	m_nCurrentCoord = point;
	point += m_nMapCoord;
	CPoint tmp = point;

	float row = (float)(point.x / MOUSE_SPACING);
	float col = (float)(point.y / MOUSE_SPACING);

	point.x = (int)(row * MOUSE_SPACING);
	point.y = (int)(col * MOUSE_SPACING);

	if(m_bIsNewMap) {
		if (nFlags == MK_MBUTTON) {
			m_nMapCoord.x = m_nMapPrevCoord.x - (m_nCurrentCoord.x - m_nStartCoord.x);
			m_nMapCoord.y = m_nMapPrevCoord.y - (m_nCurrentCoord.y - m_nStartCoord.y);
		}
		if(m_emMapLayout == MAP_COLL && g_pDrawBrush[CURSOR_COLL].GetActive()) {
			g_pDrawBrush[CURSOR_COLL].SetTexturePosition(
				(float)tmp.x - g_pDrawBrush[CURSOR_COLL].GetTextureWidth()/4,
				(float)tmp.y - g_pDrawBrush[CURSOR_COLL].GetTextureHeight()/4
			);
			if(m_bIsLBotton) {
				// �ж�����ڵ�ǰ��ײ����ļ��м���
				m_pDrawMapColl->Update(tmp.x,tmp.y);
			}
		} else if(m_emMapLayout == MAP_TRIG) {
			for(size_t i(0);i<m_stdMapTrigElement.size();++i) {
				if(m_stdMapTrigElement[i]->GetSelectState()) {
					m_stdMapTrigElement[i]->SetTexturePosition(
						(float)tmp.x, (float)tmp.y);
					break;
				}
			}
		} else if(m_emCursorLayout != CURSOR_NONE) {
			g_pDrawBrush[m_emCursorLayout].SetTexturePosition(
				(float)point.x, (float)point.y);
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void CMapEditView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// ��Ϊ��������Ҽ�ɾ�������ж�������ʱ���Դ�����������
	if(m_emMapLayout != MAP_TRIG) {
		if (m_emCursorLayout != CURSOR_NONE) {
			g_pDrawBrush[m_emCursorLayout].SetActive(FALSE);
			m_emCursorLayout = CURSOR_NONE;
		}
	} else {
		if (!m_stdMapTrigElement.empty() && m_nSelTriggerid != -1) {
			SAFE_DELETE(*(m_stdMapTrigElement.begin()+m_nSelTriggerid));
			m_stdMapTrigElement.erase(m_stdMapTrigElement.begin()+m_nSelTriggerid);
			m_emCursorLayout	= CURSOR_NONE;
			m_nTriggerNum--;
		}
	}

	CView::OnRButtonDown(nFlags, point);
}

void CMapEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_emMapLayout == MAP_COLL)
	{
		m_bIsLBotton = FALSE;
	}
	CView::OnLButtonUp(nFlags, point);
}

void CMapEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	point.x += m_nMapCoord.x;
	point.y += m_nMapCoord.y;
	CPoint tmp = point;

	int row = point.x/MOUSE_SPACING;
	int col = point.y/MOUSE_SPACING;

	point.x = row*MOUSE_SPACING;
	point.y = col*MOUSE_SPACING;

	if(!m_bIsNewMap)
	{
		return;
	}

	// �����û����ִ��ʰ�����
	// ������ж���ִ�з��ò���
	if(m_emMapLayout == MAP_BRUSH) // �����װ�β�״̬
	{
		if(m_emCursorLayout == CURSOR_NONE)
		{
			MousePickBrushMap(tmp);
		}
		else if(m_emCursorLayout == CURSOR_BRUSH)
		{
			MouseDownBrushMap(point);
			m_emCursorLayout = CURSOR_NONE;
		}
	}
	else if(m_emMapLayout == MAP_NPC)
	{
		if(m_emCursorLayout == CURSOR_NONE)
		{
			MousePickNpcMap(tmp);
		}
		else if(m_emCursorLayout == CURSOR_NPC)
		{
			MouseDownNpcMap(point);
			m_emCursorLayout = CURSOR_NONE;
		}
	}
	else if(m_emMapLayout == MAP_MONSTER)
	{
		if(m_emCursorLayout == CURSOR_NONE)
		{
			MousePickMonsterMap(tmp);
		}
		else if(m_emCursorLayout == CURSOR_MONSTER)
		{
			MouseDownMonsterMap(point);
			m_emCursorLayout = CURSOR_NONE;
		}
	}
	else if(m_emMapLayout == MAP_TRIG)
	{
		if(m_emCursorLayout == CURSOR_NONE)
		{
			MousePickTrigMap(tmp);
		}
		else if(m_emCursorLayout == CURSOR_TRIG)
		{
			MouseDownTrigMap(tmp);
			m_emCursorLayout = CURSOR_NONE;
		}
	}
	else if(m_emMapLayout == MAP_COLL) // �������ײ��״̬
	{
		m_bIsLBotton = TRUE;
		if(g_pDrawBrush[CURSOR_COLL].GetActive()) 
		{
			g_pDrawBrush[CURSOR_COLL].SetTexturePosition(
				(float)tmp.x-m_nMapCoord.x,(float)tmp.y-m_nMapCoord.y);
			if(m_bIsLBotton)
			{
				// �ж�����ڵ�ǰ��ײ����ļ��м���
				m_pDrawMapColl->Update(tmp.x,tmp.y);
			}
		}
	}

	CView::OnLButtonDown(nFlags, point);
}

// ��ˢ���ʰ�����
void CMapEditView::MousePickBrushMap(CPoint point)
{
	// ���������ڵ���Ԫ��(�и�)���о��ε���ж�
	// ע��:�������������ǰ��������ж�
	for(int i((int)m_stdMapBrushElement.size()-1);i>-1;--i)
	{
		// Ԫ�صľ���
		RECT rect   = CRect(0,0,0,0);
		rect.left   = (LONG)m_stdMapBrushElement[i]->GetPositionX();
		rect.top    = (LONG)m_stdMapBrushElement[i]->GetPositionY();
		rect.right  = rect.left + m_stdMapBrushElement[i]->GetTextureWidth();
		rect.bottom = rect.top  + m_stdMapBrushElement[i]->GetTextureHeight();

		if(point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
		{
			// ����Ԫ�ؾ���
			int  num = m_stdMapBrushElement[i]->GetTextrueNum();

			SAFE_DELETE(m_stdMapBrushElement[i]);
			
			m_stdMapBrushElement.erase(m_stdMapBrushElement.begin()+i);
			m_emCursorLayout = CURSOR_BRUSH;
			g_pDrawBrush[m_emCursorLayout].SetTextureNum(num);
			g_pDrawBrush[m_emCursorLayout].SetTexturePosition(
				(float)(point.x/MOUSE_SPACING*MOUSE_SPACING),(float)(point.y/MOUSE_SPACING*MOUSE_SPACING));
			g_pDrawBrush[m_emCursorLayout].SetActive(true);
			return;
		}
	}
}

// NPC���ʰ�����
void CMapEditView::MousePickNpcMap(CPoint point) 
{
	// ��������NPC��(װ����)Ԫ�ؽ��о��ε���ж�
	// ע��:�������������ǰ��������ж�
	for(int i((int)m_stdMapNpcElement.size()-1);i>-1;--i)
	{
		// Ԫ�صľ���
		RECT rect   = CRect(0,0,0,0);
		rect.left   = (LONG)m_stdMapNpcElement[i]->GetPositionX();
		rect.top    = (LONG)m_stdMapNpcElement[i]->GetPositionY();
		rect.right  = rect.left + m_stdMapNpcElement[i]->GetTextureWidth();
		rect.bottom = rect.top  + m_stdMapNpcElement[i]->GetTextureHeight();

		if(point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
		{
			// ����Ԫ�ؾ���
			int  num = m_stdMapNpcElement[i]->GetTextrueNum();

			SAFE_DELETE(m_stdMapNpcElement[i]);
			m_stdMapNpcElement.erase(m_stdMapNpcElement.begin()+i);
			m_emCursorLayout = CURSOR_NPC;
			g_pDrawBrush[m_emCursorLayout].SetTextureNum(num);
			g_pDrawBrush[m_emCursorLayout].SetTexturePosition(
				(float)(point.x/MOUSE_SPACING*MOUSE_SPACING),(float)(point.y/MOUSE_SPACING*MOUSE_SPACING));
			g_pDrawBrush[m_emCursorLayout].SetActive(true);
			return;
		}
	}
}

// ������ʰ�����
void CMapEditView::MousePickMonsterMap(CPoint point)
{
	// ʰȡ����
	// ��������װ�β�(�и�)Ԫ�ؽ��о��ε���ж�
	// ע��:�������������ǰ��������ж�
	for(int i((int)m_stdMapMonsterElement.size()-1);i>-1;--i)
	{
		// Ԫ�صľ���
		RECT rect   = CRect(0,0,0,0);
		rect.left   = (LONG)m_stdMapMonsterElement[i]->GetPositionX();
		rect.top    = (LONG)m_stdMapMonsterElement[i]->GetPositionY();
		rect.right  = rect.left + m_stdMapMonsterElement[i]->GetTextureWidth();
		rect.bottom = rect.top  + m_stdMapMonsterElement[i]->GetTextureHeight();
		rect.bottom -= m_stdMapMonsterElement[i]->GetTextureHeight()/2;

		if(point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
		{
			// ����Ԫ�ؾ���
			int  num = m_stdMapMonsterElement[i]->GetTextrueNum();

			SAFE_DELETE(m_stdMapMonsterElement[i]);
			m_stdMapMonsterElement.erase(m_stdMapMonsterElement.begin()+i);
			m_emCursorLayout = CURSOR_MONSTER;
			g_pDrawBrush[m_emCursorLayout].SetTextureNum(num);
			g_pDrawBrush[m_emCursorLayout].SetTexturePosition(
				(float)(point.x/MOUSE_SPACING*MOUSE_SPACING),(float)(point.y/MOUSE_SPACING*MOUSE_SPACING));
			g_pDrawBrush[m_emCursorLayout].SetActive(true);
			return;
		}
	}
}

// �������ʰ�����
void CMapEditView::MousePickTrigMap(CPoint point)
{
	// �������д�����Ԫ�ؽ��о��ε���ж�
	// ע��:�������������ǰ��������ж�
	for(int i((int)m_stdMapTrigElement.size()-1);i>-1;--i)
	{
		// Ԫ�صľ���
		RECT rect   = CRect(0,0,0,0);
		rect.left   = (LONG)m_stdMapTrigElement[i]->GetPositionX();
		rect.top    = (LONG)m_stdMapTrigElement[i]->GetPositionY();
		rect.right  = rect.left + m_stdMapTrigElement[i]->GetTextureWidth();
		rect.bottom = rect.top  + m_stdMapTrigElement[i]->GetTextureHeight();

		if(point.x > rect.left && point.x < rect.right && point.y > rect.top && point.y < rect.bottom)
		{
			// ����Ԫ�ؾ���
			m_stdMapTrigElement[i]->SetSelectState(TRUE);
			m_emCursorLayout = CURSOR_TRIG;
			m_nSelTriggerid = i;
			return;
		}
	}
}

// ��ˢ��ķ��ò���
void CMapEditView::MouseDownBrushMap(CPoint point)	
{
	// ���ò���
	// �����������
	int num = g_pDrawBrush[m_emCursorLayout].GetTextrueNum();  // ��õ�ǰ��ͼ���

	CDrawBrush* draw = new CDrawBrush;
	draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[m_emCursorLayout].GetTexturePoint(),g_pDrawBrush[m_emCursorLayout].GetTextureMaxNum());
	draw->SetTextureNum(num);
	draw->SetTexturePosition((float)point.x,(float)point.y);
	draw->SetActive();
	m_stdMapBrushElement.push_back(draw);
}

// NPC��ķ��ò���
void CMapEditView::MouseDownNpcMap(CPoint point)
{
	// ���ò���
	// �����������
	int num = g_pDrawBrush[m_emCursorLayout].GetTextrueNum();  // ��õ�ǰ��ͼ���

	CDrawBrush* draw = new CDrawBrush;
	draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[m_emCursorLayout].GetTexturePoint(),g_pDrawBrush[m_emCursorLayout].GetTextureMaxNum());
	draw->SetTextureNum(num);
	draw->SetTexturePosition((float)point.x,(float)point.y);
	draw->SetActive();
	m_stdMapNpcElement.push_back(draw);
}

// �����ķ��ò���
void CMapEditView::MouseDownMonsterMap(CPoint point)
{
	// ���ò���
	// �����������
	int num = g_pDrawBrush[m_emCursorLayout].GetTextrueNum();  // ��õ�ǰ��ͼ���

	CDrawBrush* draw = new CDrawBrush;
	draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[m_emCursorLayout].GetTexturePoint(),g_pDrawBrush[m_emCursorLayout].GetTextureMaxNum());
	draw->SetTextureNum(num);
	draw->SetTexturePosition((float)point.x,(float)point.y);
	draw->SetActive();
	m_stdMapMonsterElement.push_back(draw);
}

// ������ķ��ò���
void CMapEditView::MouseDownTrigMap(CPoint point)
{
	// ���ò���
	// �����������
	for(size_t i(0);i<m_stdMapTrigElement.size();++i)
	{
		if(m_stdMapTrigElement[i]->GetSelectState())
		{
			m_stdMapTrigElement[i]->SetSelectState(FALSE);
			return;
		}
	}
	m_nSelTriggerid = -1;
}

void CMapEditView::OnEditUndo()
{
	// ȥ�����һ��Ԫ��
	if(m_emMapLayout == MAP_BRUSH)
	{
		if(!m_stdMapBrushElement.empty()) {
			SAFE_DELETE(m_stdMapBrushElement.back());
			m_stdMapBrushElement.pop_back();
		}
	}
	else if(m_emMapLayout == MAP_NPC)
	{
		if(!m_stdMapNpcElement.empty()) {
			SAFE_DELETE(m_stdMapNpcElement.back());
			m_stdMapNpcElement.pop_back();
		}
	}
	else if(m_emMapLayout == MAP_MONSTER)
	{
		if(!m_stdMapMonsterElement.empty()) {
			SAFE_DELETE(m_stdMapMonsterElement.back());
			m_stdMapMonsterElement.pop_back();
		}
	}
	else if(m_emMapLayout == MAP_TRIG)
	{
		if(!m_stdMapTrigElement.empty()) {
			SAFE_DELETE(m_stdMapTrigElement.back());
			m_stdMapTrigElement.pop_back();
		}
		if(m_nTriggerNum > 0)
			--m_nTriggerNum;
	}
	else if(m_emMapLayout == MAP_COLL)
	{
		if(!m_pDrawMapColl->empty()) 
			m_pDrawMapColl->SafeDeleteFront();
	}
}

void CMapEditView::IsSaveMap()
{
	if(!m_stdMapBrushElement.empty()    || 
		!m_stdMapNpcElement.empty()     ||
		!m_stdMapMonsterElement.empty() ||
		!m_stdMapTrigElement.empty()	||
		m_pDrawMapColl && !m_pDrawMapColl->empty())
	{
		if(IDYES==::MessageBox(0, L"�Ƿ񱣴浱ǰ��ͼ?", 0, MB_YESNO))
		{
			OnFileSave();
		}
	}
}

void CMapEditView::OnFileNew()
{
	IsSaveMap();

	// ѡ���ͼ����
	CToolBackDialog* pBack = new CToolBackDialog;
	INT_PTR nResponse = pBack->DoModal();
	SAFE_DELETE(pBack);
	if (nResponse == IDCANCEL) return;
	
	// �û�����ȷ�Ϻ�������
	SAFE_STLDELETE(m_stdMapBrushElement);
	SAFE_STLDELETE(m_stdMapNpcElement);
	SAFE_STLDELETE(m_stdMapMonsterElement);
	SAFE_STLDELETE(m_stdMapTrigElement);

	// ��ײҲҪ���
	if(m_pDrawMapColl)
		m_pDrawMapColl->ClearColls();
	Clear();
	m_bIsNewMap = TRUE;
	
	// �������0��ʼ
	m_nTriggerNum = 0;

	g_pDrawBrush[CURSOR_BACK].SetTextureColor(D3DCOLOR_ARGB(BACK_DEFAULT_ALPHAVALUE,255,255,255));

	// ������ײ��(������)
	if(!m_pDrawMapColl)
	{
		m_pDrawMapColl = new CDrawColl;

		if(FAILED(m_pDrawMapColl->OnCreate(_pd3dDevice,g_pDrawBrush[CURSOR_COLL].GetTexturePoint()[0])))
		{
			AfxMessageBox(L"��ײ��ͼƬ����ʧ��!");
			::PostQuitMessage(0);
		}
	}
	else
	{
		m_pDrawMapColl->ClearColls();
	}
	m_pDrawMapColl->SetActive();

	m_emMapLayout = MAP_ALL;
}

void CMapEditView::LoadFileByBin(std::wstring PathName)
{
	// ��ʼ��ȡ�ļ�(�����Ʒ�ʽ)
	FILE* file = NULL;
	_wfopen_s(&file,PathName.c_str(),L"rb");

	// ��ͼ�����ı��
	DWORD backNum;
	fread_s(&backNum,sizeof(DWORD),sizeof(DWORD),1,file);
	m_nMapID = backNum;
	DWORD brushNum;   // ��ˢԪ�صĸ���
	DWORD npcNum;     // NPCԪ�صĸ���
	DWORD monsterNum; // ����Ԫ�صĸ���
	DWORD trigNum;    // ����Ԫ�صĸ���

	fread_s(&brushNum,  sizeof(DWORD),sizeof(DWORD),1,file);
	fread_s(&npcNum,    sizeof(DWORD),sizeof(DWORD),1,file);
	fread_s(&monsterNum,sizeof(DWORD),sizeof(DWORD),1,file);
	fread_s(&trigNum,   sizeof(DWORD),sizeof(DWORD),1,file);

	// ��ˢԪ������
	for(DWORD i(0);i < brushNum;++i)
	{
		int index,x,y;
		fread_s(&index,sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&x,    sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&y,    sizeof(DWORD),sizeof(DWORD),1,file);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_BRUSH].GetTexturePoint(),g_pDrawBrush[CURSOR_BRUSH].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapBrushElement.push_back(draw);
	}

	// NPCԪ������
	for(DWORD i(0);i < npcNum;++i)
	{
		int index,x,y;
		fread_s(&index,sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&x,    sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&y,    sizeof(DWORD),sizeof(DWORD),1,file);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_NPC].GetTexturePoint(),g_pDrawBrush[CURSOR_NPC].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index - 1);
		m_stdMapNpcElement.push_back(draw);
	}

	// ����Ԫ������
	for(DWORD i(0);i < monsterNum;++i)
	{
		int index,x,y;
		fread_s(&index,sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&x,    sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&y,    sizeof(DWORD),sizeof(DWORD),1,file);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_MONSTER].GetTexturePoint(),g_pDrawBrush[CURSOR_MONSTER].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapMonsterElement.push_back(draw);
	}

	// ����Ԫ������
	for(DWORD i(0);i < trigNum;++i)
	{
		int index,x,y;
		fread_s(&index,sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&x,    sizeof(DWORD),sizeof(DWORD),1,file);
		fread_s(&y,    sizeof(DWORD),sizeof(DWORD),1,file);
		DWORD  m_iNo1 = 0;
		fread_s(&m_iNo1,sizeof(DWORD),sizeof(DWORD),1,file);
		
		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_TRIG].GetTexturePoint(),g_pDrawBrush[CURSOR_TRIG].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		draw->SetTrigMapID(m_iNo1);
		m_stdMapTrigElement.push_back(draw);
	}

	// ������ײ��ͼƬ
	if(!m_pDrawMapColl)
	{
		m_pDrawMapColl = new CDrawColl;

		if(FAILED(m_pDrawMapColl->OnCreate(_pd3dDevice,g_pDrawBrush[CURSOR_COLL].GetTexturePoint()[0])))
		{
			AfxMessageBox(L"��ײ��ͼƬ����ʧ��!");
			::PostQuitMessage(0);
		}
	} else {
		m_pDrawMapColl->ClearColls();
	}

	// ��ȡ��ײ��Ϣ
	DWORD collNum;
	fread_s(&collNum,sizeof(DWORD),sizeof(DWORD),1,file);

	for (DWORD i(0); i < collNum; ++i) 
	{
		FLOAT x, y;
		fread_s(&x,sizeof(FLOAT),sizeof(FLOAT),1,file);
		fread_s(&y,sizeof(FLOAT),sizeof(FLOAT),1,file);
		m_pDrawMapColl->InsertColl(new D3DXVECTOR2(x, y));
	}
	m_pDrawMapColl->ResetInsertIt();
	fclose(file);
	m_pDrawMapColl->SetActive();

	// ���õ�ͼ����ͼƬ
	g_pDrawBrush[CURSOR_BACK].SetTextureNum(backNum);
	g_pDrawBrush[CURSOR_BACK].SetTextureColor(D3DCOLOR_ARGB(BACK_DEFAULT_ALPHAVALUE,255,255,255));
	g_pDrawBrush[CURSOR_BACK].SetActive();

}

void CMapEditView::LoadFileByTex(std::wstring PathName)
{
	// ��ʼ��ȡ�ļ�(�ı���ʽ)
	FILE* file = NULL;
	_wfopen_s(&file,PathName.c_str(),L"r");

	// ��ͼ�����ı��
	int backNum    = 0;
	fscanf_s(file,"%d",&backNum);
	m_nMapID = backNum;
	int brushNum   = 0; // ��ˢԪ�صĸ���
	int npcNum     = 0; // NPCԪ�صĸ���
	int monsterNum = 0; // ����Ԫ�صĸ���
	int trigNum    = 0; // ����Ԫ�صĸ���
	fscanf_s(file,"%d",&brushNum);
	fscanf_s(file,"%d",&npcNum);
	fscanf_s(file,"%d",&monsterNum);
	fscanf_s(file,"%d",&trigNum);

	// ��ˢԪ������
	for(int i(0);i < brushNum;++i)
	{
		int index(0),x(0),y(0);
		fscanf_s(file,"%d %d %d",&index,&x,&y);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_BRUSH].GetTexturePoint(),g_pDrawBrush[CURSOR_BRUSH].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapBrushElement.push_back(draw);
	}

	// NPCԪ������
	for(int i(0);i < npcNum;++i)
	{
		int index(0),x(0),y(0);
		fscanf_s(file,"%d %d %d",&index,&x,&y);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_NPC].GetTexturePoint(),g_pDrawBrush[CURSOR_NPC].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index - 1);
		m_stdMapNpcElement.push_back(draw);
	}

	// ����Ԫ������
	for(int i(0);i < monsterNum;++i)
	{
		int index(0),x(0),y(0);
		fscanf_s(file,"%d %d %d",&index,&x,&y);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_MONSTER].GetTexturePoint(),g_pDrawBrush[CURSOR_MONSTER].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapMonsterElement.push_back(draw);
	}

	// ����Ԫ������
	for(int i(0);i < trigNum;++i)
	{
		int index(0),x(0),y(0),triggeridx(0);
		fscanf_s(file,"%d %d %d %d ",&index,&x,&y,&triggeridx);

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_TRIG].GetTexturePoint(),g_pDrawBrush[CURSOR_TRIG].GetTextureMaxNum());
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		draw->SetTrigMapID(triggeridx);
		m_stdMapTrigElement.push_back(draw);
	}

	// ������ײ��ͼƬ
	if(!m_pDrawMapColl)
	{
		m_pDrawMapColl = new CDrawColl;

		if(FAILED(m_pDrawMapColl->OnCreate(_pd3dDevice,g_pDrawBrush[CURSOR_COLL].GetTexturePoint()[0])))
		{
			AfxMessageBox(L"��ײ��ͼƬ����ʧ��!");
			::PostQuitMessage(0);
		}
	} else {
		m_pDrawMapColl->ClearColls();
	}

	// ��ȡ��ײ��Ϣ
	int collNum(0);
	fscanf_s(file,"%d",&collNum);
	for(int i(0); i<collNum; ++i)
	{
		float x(0),y(0);
		fscanf_s(file,"%f %f",&x,&y);
		m_pDrawMapColl->InsertColl(new D3DXVECTOR2(x, y));
	}
	m_pDrawMapColl->ResetInsertIt();
	fclose(file);
	m_pDrawMapColl->SetActive();

	// ���õ�ͼ����ͼƬ
	g_pDrawBrush[CURSOR_BACK].SetTextureNum(backNum);
	g_pDrawBrush[CURSOR_BACK].SetTextureColor(D3DCOLOR_ARGB(BACK_DEFAULT_ALPHAVALUE,255,255,255));
	g_pDrawBrush[CURSOR_BACK].SetActive();

}

void CMapEditView::LoadFileByXml(std::wstring PathName) {
	// ��ʼ��ȡ�ļ�(XML��ʽ)
	tinyxml2::XMLDocument doc; CHAR buf[256];
	WideCharToMultiByte(CP_ACP, 0, PathName.c_str(), -1, buf, 256, NULL, NULL);
	if (doc.LoadFile(buf)) return;
	tinyxml2::XMLElement *node, *child;

	// ��ͼ�����ı��
	node = doc.FirstChildElement("Backgroud");
	m_nMapID = atoi(node->GetText());

	// ��ˢԪ������
	node = doc.FirstChildElement("Brush");
	child = node->FirstChildElement();
	while (child)
	{
		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_BRUSH].GetTexturePoint(),g_pDrawBrush[CURSOR_BRUSH].GetTextureMaxNum());
		draw->SetTexturePosition((FLOAT)atoi(child->Attribute("x")), 
			(FLOAT)atoi(child->Attribute("y")));
		draw->SetActive();
		draw->SetTextureNum(atoi(child->Attribute("index")));
		m_stdMapBrushElement.push_back(draw);
		child = child->NextSiblingElement();
	}

	// NPCԪ������
	node = doc.FirstChildElement("NPC");
	child = node->FirstChildElement();
	while (child)
	{
		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_NPC].GetTexturePoint(),g_pDrawBrush[CURSOR_NPC].GetTextureMaxNum());
		draw->SetTexturePosition((FLOAT)atoi(child->Attribute("x")),
			(FLOAT)atoi(child->Attribute("y")));
		draw->SetActive();
		draw->SetTextureNum(atoi(child->Attribute("index"))-1);
		m_stdMapNpcElement.push_back(draw);
		child = child->NextSiblingElement();
	}

	// ����Ԫ������
	node = doc.FirstChildElement("Monster");
	child = node->FirstChildElement();
	while (child)
	{
		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_MONSTER].GetTexturePoint(),g_pDrawBrush[CURSOR_MONSTER].GetTextureMaxNum());
		draw->SetTexturePosition((FLOAT)atoi(child->Attribute("x")),
			(FLOAT)atoi(child->Attribute("y")));
		draw->SetActive();
		draw->SetTextureNum(atoi(child->Attribute("index")));
		m_stdMapMonsterElement.push_back(draw);
		child = child->NextSiblingElement();
	}

	// ����Ԫ������
	node = doc.FirstChildElement("Trigger");
	child = node->FirstChildElement();
	while (child)
	{
		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_TRIG].GetTexturePoint(),g_pDrawBrush[CURSOR_TRIG].GetTextureMaxNum());
		draw->SetTexturePosition((FLOAT)atoi(child->Attribute("x")),
			(FLOAT)atoi(child->Attribute("y")));
		draw->SetActive();
		draw->SetTextureNum(atoi(child->Attribute("index")));
		draw->SetTrigMapID(atoi(child->Attribute("id")));
		m_stdMapTrigElement.push_back(draw);
		child = child->NextSiblingElement();
	}

	// ������ײ��ͼƬ
	if(!m_pDrawMapColl)
	{
		m_pDrawMapColl = new CDrawColl;

		if(FAILED(m_pDrawMapColl->OnCreate(_pd3dDevice,g_pDrawBrush[CURSOR_COLL].GetTexturePoint()[0])))
		{
			AfxMessageBox(L"��ײ��ͼƬ����ʧ��!");
			::PostQuitMessage(0);
		}
	} else {
		m_pDrawMapColl->ClearColls();
	}

	// ��ȡ��ײ��Ϣ
	node = doc.FirstChildElement("Collision");
	child = node->FirstChildElement();
	while (child) {
		m_pDrawMapColl->InsertColl(new D3DXVECTOR2((FLOAT)atoi(child->Attribute("x")), 
			(FLOAT)atoi(child->Attribute("y"))));
		child = child->NextSiblingElement();
	}
	m_pDrawMapColl->ResetInsertIt();
	m_pDrawMapColl->SetActive();

	// ���õ�ͼ����ͼƬ
	g_pDrawBrush[CURSOR_BACK].SetTextureNum(m_nMapID);
	g_pDrawBrush[CURSOR_BACK].SetTextureColor(D3DCOLOR_ARGB(BACK_DEFAULT_ALPHAVALUE,255,255,255));
	g_pDrawBrush[CURSOR_BACK].SetActive();
}

void CMapEditView::OnFileOpen()
{
	IsSaveMap();

	CFileDialog cfile(TRUE);
	cfile.m_ofn.lpstrFilter = L"BSP(*.bsp)\0*.bsp\0MAP(*.map)\0*.map\0XML(*.xml)\0*.xml\0\0";

	// ��õ�ǰĿ¼
	//WCHAR buff[MAX_PATH];
	//::GetCurrentDirectory(MAX_PATH,buff);
	//wcscat_s(buff,-1,L"\\data\\map");

	cfile.m_ofn.lpstrInitialDir = L"./data/map";

	if(IDOK == cfile.DoModal())
	{
		// ���������·����
		std::wstring buff = cfile.GetPathName().GetString();

		// �û�����ȷ�Ϻ�������
		SAFE_STLDELETE(m_stdMapBrushElement);
		SAFE_STLDELETE(m_stdMapNpcElement);
		SAFE_STLDELETE(m_stdMapMonsterElement);
		SAFE_STLDELETE(m_stdMapTrigElement);

		m_bIsNewMap = TRUE;

		DWORD Index = cfile.GetOFN().nFilterIndex;

		// 1 == .bsp(������) 2 == .map(�ı�) 3 = .xml(�������)
		if(Index == 1)
		{
			LoadFileByBin(buff);
		}
		else if(Index == 2)
		{
			LoadFileByTex(buff);
		}
		else if(Index == 3)
		{
			LoadFileByXml(buff);
		}

		// ���ļ��󴥷���Ҫ���
		m_nTriggerNum = (int)m_stdMapTrigElement.size();
		m_nMapCoord.x = 0;
		m_nMapCoord.y = 0;

		m_emMapLayout = MAP_ALL;
	}
}

// vector����ıȽϺ���
struct CDrawBrushComp
{
	bool operator()(CDrawBrush* a,CDrawBrush* b)const
	{
		return (a->GetTextrueNum() < b->GetTextrueNum());
	}
};

void CMapEditView::SaveFileByBin(std::wstring PathName)
{
	wchar_t* str  = &(PathName.at(PathName.size()-4));
	wchar_t* name = L".bsp";

	// ����û�û������,Ҫ������չ��
	if(0!=wcscmp(str,name))
		PathName += L".bsp";

	// ��ʼд���ļ�
	FILE* file = NULL;
	_wfopen_s(&file,PathName.c_str(),L"wb");

	// ��ͼ�����ı��
	DWORD backNum  = g_pDrawBrush[CURSOR_BACK].GetTextrueNum();
	fwrite(&backNum,sizeof(DWORD),1,file);

	// ��ˢԪ�صĸ���
	DWORD brushNum = (DWORD)m_stdMapBrushElement.size();
	fwrite(&brushNum,sizeof(DWORD),1,file);

	// NPCԪ�صĸ���
	DWORD npcNum = (DWORD)m_stdMapNpcElement.size();
	fwrite(&npcNum,sizeof(DWORD),1,file);

	// ����Ԫ�صĸ���
	DWORD monsterNum = (DWORD)m_stdMapMonsterElement.size();
	fwrite(&monsterNum,sizeof(DWORD),1,file);

	// ����Ԫ�صĸ���
	DWORD trigNum = (DWORD)m_stdMapTrigElement.size();
	fwrite(&trigNum,sizeof(DWORD),1,file);

	// ��ˢԪ����������
	sort(m_stdMapBrushElement.begin(),m_stdMapBrushElement.end(),CDrawBrushComp()); 

	// ��ˢԪ�ص�����д��
	for(DWORD i(0);i < brushNum;++i)
	{
		DWORD index = m_stdMapBrushElement[i]->GetTextrueNum();
		DWORD x     = (DWORD)m_stdMapBrushElement[i]->GetPositionX();
		DWORD y     = (DWORD)m_stdMapBrushElement[i]->GetPositionY();
		fwrite(&index,sizeof(DWORD),1,file);
		fwrite(&x,    sizeof(DWORD),1,file);
		fwrite(&y,    sizeof(DWORD),1,file);
	}

	// NPCԪ����������
	sort(m_stdMapNpcElement.begin(),m_stdMapNpcElement.end(),CDrawBrushComp()); 

	// NPCԪ�ص�����д��
	for(DWORD i(0);i < npcNum;++i)
	{
		DWORD index = m_stdMapNpcElement[i]->GetTextrueNum() + 1;
		DWORD x     = (DWORD)m_stdMapNpcElement[i]->GetPositionX();
		DWORD y     = (DWORD)m_stdMapNpcElement[i]->GetPositionY();
		fwrite(&index,sizeof(DWORD),1,file);
		fwrite(&x,    sizeof(DWORD),1,file);
		fwrite(&y,    sizeof(DWORD),1,file);
	}

	// ����Ԫ����������
	sort(m_stdMapMonsterElement.begin(),m_stdMapMonsterElement.end(),CDrawBrushComp());

	// ����Ԫ�ص�����д��
	for(DWORD i(0);i < monsterNum;++i)
	{
		DWORD index = m_stdMapMonsterElement[i]->GetTextrueNum();
		DWORD x     = (DWORD)m_stdMapMonsterElement[i]->GetPositionX();
		DWORD y     = (DWORD)m_stdMapMonsterElement[i]->GetPositionY();
		fwrite(&index,sizeof(DWORD),1,file);
		fwrite(&x,    sizeof(DWORD),1,file);
		fwrite(&y,    sizeof(DWORD),1,file);
	}

	// ����Ԫ�ص�����д��
	for(DWORD i(0);i < trigNum;++i)
	{
		DWORD index = m_stdMapTrigElement[i]->GetTextrueNum();
		DWORD x     = (DWORD)m_stdMapTrigElement[i]->GetPositionX();
		DWORD y     = (DWORD)m_stdMapTrigElement[i]->GetPositionY();
		DWORD id    = m_stdMapTrigElement[i]->GetTrigMapID();

		fwrite(&index,sizeof(DWORD),1,file);
		fwrite(&x,    sizeof(DWORD),1,file);
		fwrite(&y,    sizeof(DWORD),1,file);
		fwrite(&id,   sizeof(DWORD),1,file);
	}

	DWORD CollsNum = m_pDrawMapColl->GetCollsCount();
	fwrite(&CollsNum,sizeof(DWORD),1,file);

	CDrawColl::VecPoints *coll = m_pDrawMapColl->GetColls();
	for (CDrawColl::VecPointsIt it(coll->begin()); it != coll->end(); ++it)
	{
		FLOAT x     = (*it)->x;
		FLOAT y     = (*it)->y;
		fwrite(&x,    sizeof(FLOAT),1,file);
		fwrite(&y,    sizeof(FLOAT),1,file);
	}

	fclose(file);
}

void CMapEditView::SaveFileByTex(std::wstring PathName)
{
	wchar_t* str  = &(PathName.at(PathName.size()-4));
	wchar_t* name = L".map";

	// ����û�û������,Ҫ������չ��
	if(0!=wcscmp(str,name))
		PathName += L".map";

	// ��ʼд���ļ�
	FILE* file = NULL;
	_wfopen_s(&file,PathName.c_str(),L"w");

	// ��ͼ�����ı��
	INT backNum  = g_pDrawBrush[CURSOR_BACK].GetTextrueNum();
	char backBuff[8] = {0};
	_itoa_s(backNum,backBuff,-1,10);
	strcat_s(backBuff,-1,"\n");
	fwrite(backBuff,strlen(backBuff),1,file);

	// ��ˢԪ�صĸ���
	int brushNum = (int)m_stdMapBrushElement.size();
	char brushBuff[8] = {0};
	_itoa_s(brushNum,brushBuff,-1,10);
	strcat_s(brushBuff,-1,"\n");
	fwrite(brushBuff,strlen(brushBuff),1,file);

	// NPCԪ�صĸ���
	int npcNum = (int)m_stdMapNpcElement.size();
	char npcBuff[8] = {0};
	_itoa_s(npcNum,npcBuff,-1,10);
	strcat_s(npcBuff,-1,"\n");
	fwrite(npcBuff,strlen(npcBuff),1,file);

	// ����Ԫ�صĸ���
	int monsterNum = (int)m_stdMapMonsterElement.size();
	char monsterBuff[8] = {0};
	_itoa_s(monsterNum,monsterBuff,-1,10);
	strcat_s(monsterBuff,-1,"\n");
	fwrite(monsterBuff,strlen(monsterBuff),1,file);

	// ����Ԫ�صĸ���
	int trigNum = (int)m_stdMapTrigElement.size();
	char trigBuff[8] = {0};
	_itoa_s(trigNum,trigBuff,-1,10);
	strcat_s(trigBuff,-1,"\n");
	fwrite(trigBuff,strlen(trigBuff),1,file);

	// ��ˢԪ����������
	sort(m_stdMapBrushElement.begin(),m_stdMapBrushElement.end(),CDrawBrushComp()); 

	// ��ˢԪ�ص�����д��
	for(int i(0);i < brushNum;++i)
	{
		int index = m_stdMapBrushElement[i]->GetTextrueNum();
		int x     = (int)m_stdMapBrushElement[i]->GetPositionX();
		int y     = (int)m_stdMapBrushElement[i]->GetPositionY();

		std::stringstream sstr;
		sstr << index;
		sstr << ' ';
		sstr << x;
		sstr << ' ';
		sstr << y;
		sstr << '\n';
		fwrite(sstr.str().c_str(),sstr.str().size(),1,file);
	}

	// NPCԪ����������
	sort(m_stdMapNpcElement.begin(),m_stdMapNpcElement.end(),CDrawBrushComp()); 

	// NPCԪ�ص�����д��
	for(int i(0);i < npcNum;++i)
	{
		int index = m_stdMapNpcElement[i]->GetTextrueNum() + 1;
		int x     = (int)m_stdMapNpcElement[i]->GetPositionX();
		int y     = (int)m_stdMapNpcElement[i]->GetPositionY();

		std::stringstream sstr;
		sstr << index;
		sstr << ' ';
		sstr << x;
		sstr << ' ';
		sstr << y;
		sstr << '\n';
		fwrite(sstr.str().c_str(),sstr.str().size(),1,file);
	}

	// ����Ԫ����������
	sort(m_stdMapMonsterElement.begin(),m_stdMapMonsterElement.end(),CDrawBrushComp());

	// ����Ԫ�ص�����д��
	for(int i(0);i < monsterNum;++i)
	{
		int index = m_stdMapMonsterElement[i]->GetTextrueNum();
		int x     = (int)m_stdMapMonsterElement[i]->GetPositionX();
		int y     = (int)m_stdMapMonsterElement[i]->GetPositionY();

		std::stringstream sstr;
		sstr << index;
		sstr << ' ';
		sstr << x;
		sstr << ' ';
		sstr << y;
		sstr << '\n';
		fwrite(sstr.str().c_str(),sstr.str().size(),1,file);
	}

	// ����Ԫ�ص�����д��
	for(int i(0);i < trigNum;++i)
	{
		int index = m_stdMapTrigElement[i]->GetTextrueNum();
		int x     = (int)m_stdMapTrigElement[i]->GetPositionX();
		int y     = (int)m_stdMapTrigElement[i]->GetPositionY();
		int id    = m_stdMapTrigElement[i]->GetTrigMapID();

		std::stringstream sstr;
		sstr << index;
		sstr << ' ';
		sstr << x;
		sstr << ' ';
		sstr << y;
		sstr << ' ';
		sstr << id;
		sstr << '\n';
		fwrite(sstr.str().c_str(),sstr.str().size(),1,file);
	}
	
	// ��ײԪ�صĸ���
	int CollsNum = (int) m_pDrawMapColl->GetCollsCount();
	std::stringstream sstr;
	sstr << CollsNum << '\n';
	fwrite(sstr.str().c_str(),sstr.str().size(),1,file);

	// ��ײԪ�ص�����д��
	std::stringstream mapstr;
	CDrawColl::VecPoints *coll = m_pDrawMapColl->GetColls();
	for (CDrawColl::VecPointsIt it(coll->begin()); it != coll->end(); ++it)
	{
		float x     = (*it)->x;
		float y     = (*it)->y;
		mapstr << x << ' ' << y << '\n';
	}

	fwrite(mapstr.str().c_str(),mapstr.str().size(),1,file);

	fclose(file);
}

void CMapEditView::SaveFileByXml(std::wstring PathName) {
	wchar_t* str  = &(PathName.at(PathName.size()-4));
	wchar_t* name = L".xml";

	// ����û�û������,Ҫ������չ��
	if(0!=wcscmp(str,name))
		PathName += L".xml";

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *node, *child;
	tinyxml2::XMLDeclaration *dec;
	dec = doc.NewDeclaration();
	doc.InsertFirstChild(dec);
	// ��ͼ�����ı��
	node = doc.NewElement("Backgroud");
	node->SetText(g_pDrawBrush[CURSOR_BACK].GetTextrueNum());
	doc.InsertEndChild(node);

	// ��ˢԪ����������
	sort(m_stdMapBrushElement.begin(),m_stdMapBrushElement.end(),CDrawBrushComp()); 

	// ��ˢԪ�ص�����д��
	node = doc.NewElement("Brush");
	for (ElementsIt it(m_stdMapBrushElement.begin()); it != m_stdMapBrushElement.end(); ++it)
	{
		child = doc.NewElement("Element");
		int index = (*it)->GetTextrueNum();
		int x     = (int)(*it)->GetPositionX();
		int y     = (int)(*it)->GetPositionY();
		child->SetAttribute("index", index);
		child->SetAttribute("x", x);
		child->SetAttribute("y", y);
		node->InsertEndChild(child);
	}
	doc.InsertEndChild(node);

	// NPCԪ����������
	sort(m_stdMapNpcElement.begin(),m_stdMapNpcElement.end(),CDrawBrushComp()); 

	// NPCԪ�ص�����д��
	node = doc.NewElement("NPC");
	for (ElementsIt it(m_stdMapNpcElement.begin()); it != m_stdMapNpcElement.end(); ++it)
	{
		child = doc.NewElement("Element");
		int index = (*it)->GetTextrueNum() + 1;
		int x     = (int)(*it)->GetPositionX();
		int y     = (int)(*it)->GetPositionY();
		child->SetAttribute("index", index);
		child->SetAttribute("x", x);
		child->SetAttribute("y", y);
		node->InsertEndChild(child);
	}
	doc.InsertEndChild(node);

	// ����Ԫ����������
	sort(m_stdMapMonsterElement.begin(),m_stdMapMonsterElement.end(),CDrawBrushComp());

	// ����Ԫ�ص�����д��
	node = doc.NewElement("Monster");
	for (ElementsIt it(m_stdMapMonsterElement.begin()); it != m_stdMapMonsterElement.end(); ++it)
	{
		child = doc.NewElement("Element");
		int index = (*it)->GetTextrueNum();
		int x     = (int)(*it)->GetPositionX();
		int y     = (int)(*it)->GetPositionY();
		child->SetAttribute("index", index);
		child->SetAttribute("x", x);
		child->SetAttribute("y", y);
		node->InsertEndChild(child);
	}
	doc.InsertEndChild(node);

	// ����Ԫ�ص�����д��
	node = doc.NewElement("Trigger");
	for (ElementsIt it(m_stdMapTrigElement.begin()); it != m_stdMapTrigElement.end(); ++it)
	{
		child = doc.NewElement("Element");
		int index = (*it)->GetTextrueNum();
		int x     = (int)(*it)->GetPositionX();
		int y     = (int)(*it)->GetPositionY();
		int id    = (*it)->GetTrigMapID();
		child->SetAttribute("index", index);
		child->SetAttribute("x", x);
		child->SetAttribute("y", y);
		child->SetAttribute("id", id);
		node->InsertEndChild(child);
	}
	doc.InsertEndChild(node);

	// ��ײԪ�ص�����д��
	node = doc.NewElement("Collision");
	CDrawColl::VecPoints *coll = m_pDrawMapColl->GetColls();
	for (CDrawColl::VecPointsIt it(coll->begin()); it != coll->end(); ++it)
	{
		child = doc.NewElement("Position");
		float x     = (*it)->x;
		float y     = (*it)->y;
		child->SetAttribute("x", x);
		child->SetAttribute("y", y);
		node->InsertEndChild(child);
	}
	doc.InsertEndChild(node);

	// д���ļ�
	FILE* file = NULL;
	_wfopen_s(&file,PathName.c_str(),L"w");
	doc.SaveFile(file);
	fclose(file);
}

bool CMapEditView::LoadCfgFile()
{
	std::wstring buff = L"Map.cfg";

	FILE* file = NULL;

	// errno_t = 0 �ɹ� 
	// errno_t!= 0 ʧ��
	// ENOENT û���������ļ���Ŀ¼
	// EIO    I/O����
	// ENOEXEC ��ʽ����
	if(ENOENT == _wfopen_s(&file,buff.c_str(),L"r"))
	{
		//AfxMessageBox(L"��ȡ�����ļ�������!");
		// ����׷�ӷ�ʽд�������ļ�
		SaveCfgFile();
		return true;
	}

	// ���ļ������ȶ�ȡ���ж�
	while(1)
	{
		int n(0);
		char name[32] = {0};
		if(EOF != fscanf_s(file,"%d %s",&n,name,32))
		{
			wchar_t temp[32] = {0};
			MultiByteToWideChar(CP_ACP,0,name,-1,temp,(int)strlen(name));
			if((m_nMapID == n) || (0 == wcscmp(m_szMapName.c_str(),temp)))
			{
				if(IDYES == ::MessageBox(0,L"�����ļ����ƻ�ID��ͻ,�Ƿ񱣴��ͼ�ļ�?",0,MB_YESNO))
				{
					fclose(file);
					//AfxMessageBox(L"��ͼ�����ļ�����ʧ��!");
					return true;
				}
				fclose(file);
				//AfxMessageBox(L"��ͼ�����ļ�����ʧ��!");
				//AfxMessageBox(L"��ͼ�ļ�����ʧ��!");
				return false;
			}
		}
		else
			break;
	}
	fclose(file);

	return true;
}

void CMapEditView::SaveCfgFile()
{
	FILE* file = NULL;
	std::wstring buff = L"Map.cfg";
	_wfopen_s(&file,buff.c_str(),L"a");

	if(!file)
	{
		AfxMessageBox(L"��׷�ӷ�ʽ�������ļ�ʧ��!");
	}

	char temp[32] = {0};
	_itoa_s(m_nMapID,temp,-1,10);

	char name[32] = {0};
	WideCharToMultiByte(CP_ACP,0,m_szMapName.c_str(),-1,name,(int)m_szMapName.size(),0,FALSE);

	std::string strCfg = temp;
	strCfg += ' ';
	strCfg += name;
	strCfg += '\n';
	// �ݶ���ʽ: ID-�ո�-����-\n
	fwrite(strCfg.c_str(),strCfg.size(),1,file);

	fclose(file);
}

void CMapEditView::OnFileSave()
{

	if(!m_bIsNewMap) return;

	// ��õ�ǰĿ¼
	//WCHAR pathBuff[MAX_PATH] = {0};
	//::GetCurrentDirectory(MAX_PATH,pathBuff);

	// ��ʼ��ͼ�ļ�����
	CFileDialog cfile(FALSE);
	cfile.m_ofn.lpstrFilter = L"BSP(*.bsp)\0*.bsp\0MAP(*.map)\0*.map\0XML(*.xml)\0*.xml\0\0";

	// ��һ�ε�ʱ��ż���\\Save
	//static int one(0);
	//if(pathBuff[wcslen(pathBuff)-1] != 'e')
	//{
	//	if(0 == one)
	//	{
	//		wcscat_s(pathBuff,-1,L"\\data\\map");
	//		++one;
	//	}
	//}

	cfile.m_ofn.lpstrInitialDir = L"./data/map";

	if(IDOK == cfile.DoModal())
	{
		// ���������·����
		std::wstring buff = cfile.GetPathName().GetString();

		// �ȶ�ȡ�����ļ����Ƿ���ID�����Ƴ�ͻ
		if(!LoadCfgFile())
			return;

		// 1 == .bsp(������) 2 == .map(�ı�) 3 == .xml(�������)
		DWORD Index = cfile.GetOFN().nFilterIndex;

		if(Index == 1)
		{
			SaveFileByBin(buff);
		}
		else if(Index == 2)
		{
			SaveFileByTex(buff);
		}
		else if(Index == 3)
		{
			SaveFileByXml(buff);
		}

		AfxMessageBox(L"��ͼ�ļ�����ɹ�!");
	}
}

void CMapEditView::OnFileClose()
{
	// TODO: �ڴ���������������
	IsSaveMap();

	// �û�����ȷ�Ϻ�����Ԫ��
	SAFE_STLDELETE(m_stdMapBrushElement);
	SAFE_STLDELETE(m_stdMapNpcElement);
	SAFE_STLDELETE(m_stdMapMonsterElement);
	SAFE_STLDELETE(m_stdMapTrigElement);

	if(m_pDrawMapColl)
		m_pDrawMapColl->ClearColls();

	m_bIsNewMap = FALSE;

	UpdateWindow();
}

void CMapEditView::OnLayoutBrush()
{
	m_emMapLayout = MAP_BRUSH;
}

void CMapEditView::OnLayoutNpc()
{
	m_emMapLayout = MAP_NPC;
}

void CMapEditView::OnLayoutMonster()
{
	m_emMapLayout = MAP_MONSTER;
}

void CMapEditView::OnLayoutColl()
{
	m_emMapLayout = MAP_COLL;
}

void CMapEditView::OnLayoutTrigger()
{
	m_emMapLayout = MAP_TRIG;
}

void CMapEditView::OnLayoutAll()
{
	m_emMapLayout = MAP_ALL;
}

void CMapEditView::OnBackselect()
{
	if(m_bIsNewMap)
	{
		CToolBackDialog* pBack = new CToolBackDialog;
		pBack->DoModal();
		SAFE_DELETE(pBack);
	}
	else
	{
		AfxMessageBox(L"�����½�һ�ŵ�ͼ!");
	}
}

void CMapEditView::SetMapCursorNum(LAYOUT_MAP map,LAYOUT_CURSOR cursor,INT num)
{
	m_emMapLayout    = map;
	m_emCursorLayout = cursor;
	g_pDrawBrush[m_emCursorLayout].SetTextureNum(num);
	g_pDrawBrush[m_emCursorLayout].SetTexturePosition(-2000, -2000);
	g_pDrawBrush[m_emCursorLayout].SetActive();
}

BOOL CMapEditView::PreTranslateMessage(MSG* pMsg)
{
	// ��Ϊ�������������Ҫ������
	if(!m_bIsNewMap)
	{
		return TRUE;
	}

	switch(pMsg->message)
	{
	case WM_TRIG:
		{
			if( pMsg->wParam == MODIFY )
			{
				m_emMapLayout   = MAP_TRIG;
			}
			else
			{
				if(m_nTriggerNum >= 16)
					return FALSE;

				m_emMapLayout    = MAP_TRIG;
				m_emCursorLayout = CURSOR_TRIG;
				CDrawBrush* draw = new CDrawBrush;
				draw->OnCreateByPoint(_pd3dDevice,g_pDrawBrush[CURSOR_TRIG].GetTexturePoint(),g_pDrawBrush[CURSOR_TRIG].GetTextureMaxNum());

				//draw->SetTextureNum(m_nTriggerNum);

				draw->SetTrigMapID(m_nTrigMapID);

				//++m_nTriggerNum;
				//draw->SetTrigMapID(m_nTriggerNum);//���ô˴�����ID
				draw->SetTextureNum(m_nTriggerNum++);

				draw->SetSelectState(TRUE);
				draw->SetTexturePosition(-32.f,-32.f);
				draw->SetActive();
				m_stdMapTrigElement.push_back(draw);
			}
		}
		break;

	case WM_COLL:       // ��ײ��Ǹı� wParam == ��ť���� lParam == �Ƿ���ײ
		{
			SetMapCursorNum(MAP_COLL, CURSOR_COLL, (INT)pMsg->wParam);
			//::SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_CURSOR1)));
			if(m_pDrawMapColl)
				m_pDrawMapColl->SetMapUpdateNum(!(INT)pMsg->lParam);
		}
		break;

	case WM_BRUSH:      // Ԫ�ص�ѡ�� wParam == ��ť����
		{
			if( pMsg->wParam == MODIFY )
			{
				m_emMapLayout    = MAP_BRUSH;
			}
			else
			{
				SetMapCursorNum(MAP_BRUSH,CURSOR_BRUSH,(INT)pMsg->wParam);
			}
		}
		break;

	case WM_NPC:        // NPC��ѡ�� wParam == ��ť����
		{
			if( pMsg->wParam == MODIFY )
			{
				m_emMapLayout    = MAP_NPC;
			}
			else
			{
				SetMapCursorNum(MAP_NPC, CURSOR_NPC, (INT)pMsg->wParam);
			}
			
		}
		break;

	case WM_MONSTER:    // �����ѡ�� wParam == ��ť����
		{
			if( pMsg->wParam == MODIFY )
			{
				m_emMapLayout    = MAP_MONSTER;

			}
			else
			{
				SetMapCursorNum(MAP_MONSTER, CURSOR_MONSTER, (INT)pMsg->wParam);
			}

		}
		break;

	case WM_BACK:       // ������ѡ�� wParam == �ؿ�
		g_pDrawBrush[CURSOR_BACK].SetTextureNum((INT)pMsg->wParam);
		g_pDrawBrush[CURSOR_BACK].SetActive();
		break;

	case WM_DRAWMESH:   // �Ƿ�������� wParam == BOOL
		m_bIsLine = (BOOL)pMsg->wParam;
		break;

	case WM_DRAWRED:    // �Ƿ���ƺ�� wParam == BOOL
		m_bIsRedLine = (BOOL)pMsg->wParam;
		break;

	case WM_BACKALPHA:  // ����͸���ȵ��� wParam == Alpha
		g_pDrawBrush[CURSOR_BACK].SetTextureColor(D3DCOLOR_ARGB(pMsg->wParam,255,255,255));
		break;

	case WM_LAYOUTALPHA:// �㼶͸���ȵ��� wParam == Alpha
		m_nLayoutAlpha = (INT)pMsg->wParam;
		break;

	case WM_MAPINFO:    // �Ƿ���ʾ��ͼ��Ϣ wParam == BOOL
		m_bIsMapInfo = (INT)pMsg->wParam;
		break;

	case WM_TRIGMAPID:  // ����ѡ��ĵ�ǰ������ĵ�ͼID wParam == num
		m_nTrigMapID = (INT)pMsg->wParam;
		break;
	}

	return CView::PreTranslateMessage(pMsg);
}

void CMapEditView::OnMaptest()
{
	// TODO: �ڴ���������������
	if(!m_bIsNewMap)
	{
		AfxMessageBox(L"�����½�һ�ŵ�ͼ��");
		UpdateWindow();
		return;
	}

	m_pMapTest->ShowWindow(SW_SHOW);
	m_pMapTest->UpdateWindow();
}

void CMapEditView::OnCollLerp()
{
	m_pDrawMapColl->ToggleLerpRendering();
}
