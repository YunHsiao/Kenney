// DialogTest.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "DialogTest.h"

#include "ToolLoadDialog.h"

LPDIRECT3DTEXTURE9**              g_pppTestTexture = NULL;
LPDIRECT3DTEXTURE9                g_pTextureRole   = NULL;
LPDIRECT3DDEVICE9                 g_pd3dDevice     = NULL;
LPD3DXSPRITE                      g_pSpriteRole    = NULL;
LPD3DXSPRITE                      g_pSpriteBack    = NULL;
INT                               g_nTexMaxNum     = 0;
LPINT                             g_pMaxNum        = NULL;
FLOAT                             g_fDepthZ        = 0.f;
RECT                              g_rRectRole;
D3DSURFACE_DESC                   g_Desc;
D3DXMATRIX                        g_matWorld;

// CDialogTest 对话框

IMPLEMENT_DYNAMIC(CDialogTest, CDialog)

HRESULT InitD3D(HWND hWnd)
{
	HRESULT hr = S_OK;

	// 初始化D3D

	LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d9)
	{
		::MessageBox(0, L"初始化D3D9失败!", 0, 0);
		return E_FAIL;
	}

	// D3D检测
	D3DCAPS9   caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	// 判断当前显卡是否支持硬件顶点处理
	int vp = 0;
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;

	// 初始化相关参数
	d3dpp.BackBufferWidth            = 800;                           // 后台缓冲区的宽度
	d3dpp.BackBufferHeight           = 600;                           // 后台缓冲区的高度
	d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;                // 后台缓冲区的像素格式
	d3dpp.BackBufferCount            = 1;                             // 后台缓冲区的数量
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;           // 多重采样的类型
	d3dpp.MultiSampleQuality         = 0;                             // 多重采样的质量
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;         // 后台缓冲区的页面置换方式
	d3dpp.hDeviceWindow              = hWnd;                          // 窗口句柄
	d3dpp.Windowed                   = TRUE;                          // 窗口还是全屏
	d3dpp.EnableAutoDepthStencil     = TRUE;                          // 深度与模板缓存
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D16;                    // 深度缓存与模板缓存的像素格式
	d3dpp.Flags                      = 0;                             // 附加的特性
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;       // 刷新频率
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE; // 刷新时间间隔(垂直同步)


	if(FAILED(hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		vp,
		&d3dpp,
		&g_pd3dDevice)))
	{
		if(IDOK==::MessageBox(0, L"当前显示模式不支持,是否启用安全模式?", 0, MB_YESNO))
		{
			// 如果失败,则使用安全模式
			hr=d3d9->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				AfxGetMainWnd()->m_hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&g_pd3dDevice);

			if(FAILED(hr))
			{
				SAFE_RELEASE(d3d9);
				::MessageBox(0, L"创建D3D设备失败!", 0, 0);
				return hr;
			}
		}
		else
		{
			return hr;
		}
	}

	// 主角
	D3DXMatrixIdentity(&g_matWorld);
	D3DXCreateSprite(g_pd3dDevice,&g_pSpriteRole);
	D3DXCreateSprite(g_pd3dDevice,&g_pSpriteBack);

	if(FAILED(hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, L"Res/Test/Test.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 
		D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
		D3DX_FILTER_LINEAR, D3DX_FILTER_NONE,
		D3DCOLOR_ARGB(255, 255, 0, 255),
		NULL, NULL,
		&g_pTextureRole)))
	{
		AfxMessageBox(L"测试窗口主角图片加载失败!");
		return hr;
	}

	g_pTextureRole->GetLevelDesc(0,&g_Desc);
	g_rRectRole.right  = g_Desc.Width/8;
	g_rRectRole.bottom = g_Desc.Height/8;

	// 初始化全局变量
	// XML加载
	tinyxml2::XMLDocument tFile;

	if(tFile.LoadFile("Res/LoadResource.xml"))
	{
		AfxMessageBox(L"Res/LoadResource.xml 测试窗口加载失败!");
		return E_FAIL;
	}

	tinyxml2::XMLElement* tRoot = tFile.FirstChildElement();
	tinyxml2::XMLElement* tNode = tRoot->FirstChildElement();
	g_nTexMaxNum = atoi(tNode->FirstChild()->Value());
	tNode = tNode->NextSiblingElement();
	tinyxml2::XMLElement* tData;

	g_pppTestTexture = new LPDIRECT3DTEXTURE9*[g_nTexMaxNum];
	g_pMaxNum        = new INT[g_nTexMaxNum];

	// 图片资源加载
	for(int i(0);i < g_nTexMaxNum;++i)
	{
		int cur(0); g_pMaxNum[i] = atoi(tNode->Attribute("num"));
		tData = tNode->FirstChildElement();
		g_pppTestTexture[i] = new LPDIRECT3DTEXTURE9[g_pMaxNum[i]];
		do {
		LPCSTR str = tData->Attribute("path");
		LPWSTR buf = new WCHAR[strlen(str)+1];
		MultiByteToWideChar(CP_ACP,0,str,-1,buf,(int)strlen(str)+1);

		if(i!=(g_nTexMaxNum - 1)) // 元素图片
		{
			int mx(atoi(tData->Attribute("num")));

			for(int k(0);k < mx; ++k)
			{
				size_t size = wcslen(buf)+1;
				LPWSTR str  = new WCHAR[size];
				swprintf_s(str,size,buf,(k+1));

				hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, str,
					D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 
					D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
					D3DX_FILTER_LINEAR, D3DX_FILTER_NONE,
					D3DCOLOR_ARGB(255, 255, 0, 255),
					NULL, NULL,
					&g_pppTestTexture[i][cur+k]);

				SAFE_DELETE_ARRAY(str); // 字符串的释放

				if(FAILED(hr))
				{
					::MessageBox(0, L"测试窗口画刷图片加载失败!", 0, 0);
					return hr;
				}
			}
			cur += mx;
		}
		else // 背景图片
		{
			int mx(atoi(tData->Attribute("num")));

			for(int k(0); k < mx; ++k)
			{
				size_t size = wcslen(buf)+1;
				LPWSTR str  = new WCHAR[size];
				swprintf_s(str,size,buf,(k+1));

				hr = D3DXCreateTextureFromFileEx(g_pd3dDevice, str,
					D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 
					D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
					D3DX_FILTER_LINEAR, D3DX_FILTER_NONE,
					D3DCOLOR_ARGB(255, 255, 0, 255),
					NULL, NULL,
					&g_pppTestTexture[i][cur+k]);

				SAFE_DELETE_ARRAY(str); // 字符串的释放

				if(FAILED(hr))
				{
					::MessageBox(0, _T("测试窗口背景图片加载失败!"), 0, 0);
					return hr;
				}
			}
			cur += mx;
		}

		SAFE_DELETE_ARRAY(buf);
		} while (tData = tData->NextSiblingElement());

		tNode = tNode->NextSiblingElement();
	}

	SAFE_RELEASE(d3d9); // 安全释放
	return hr;
}

CDialogTest::CDialogTest(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTest::IDD, pParent)
	, m_nFrameWidth (0)
	, m_nFrameHeight(0)
	, m_nCurFrame   (0)
	, m_nFrameSet   (0)
	, m_Move        (STAND)
{
	m_vOffset.x = 0.f;
	m_vOffset.y = 0.f;
	m_vLocal.x = 0.f;
	m_vLocal.y = 0.f;
}

CDialogTest::~CDialogTest()
{
	Cleanup();
}

void CDialogTest::Cleanup()
{
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pSpriteRole);
	SAFE_RELEASE(g_pSpriteBack);
	SAFE_RELEASE(g_pTextureRole);

	// 解决STL的内存泄漏                     
	SAFE_STLDELETE(m_stdMapBrushElement);
	SAFE_STLDELETE(m_stdMapNpcElement);
	SAFE_STLDELETE(m_stdMapMonsterElement);

	// 全局指针的释放
	for(int i(0);i < g_nTexMaxNum;++i)
	{
		for(int k(0); k < g_pMaxNum[i];++k)
		{
			SAFE_RELEASE(g_pppTestTexture[i][k]);
		}
		SAFE_DELETE_ARRAY(g_pppTestTexture[i]);
	}
	SAFE_DELETE_ARRAY(g_pppTestTexture);

	SAFE_DELETE_ARRAY(g_pMaxNum);
}

void CDialogTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogTest, CDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


// CDialogTest 消息处理程序
void TestThreadProc(LPVOID pParams)
{
	InitD3D((HWND)pParams);
	return _endthread();
}

BOOL CDialogTest::OnInitDialog()
{
	CDialog::OnInitDialog();

	//_beginthread(TestThreadProc,0,this->m_hWnd);
	InitD3D(m_hWnd);

	//CToolLoadDialog* pNew = new CToolLoadDialog;
	//pNew->DoModal();
	//SAFE_DELETE(pNew);

	MoveWindow(0,0,g_nWndWidth,g_nWndHeight - 22);
	CenterWindow();

	// TODO:  在此添加额外的初始化
	SetTimer(0,30,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDialogTest::CenterRole()
{
	g_matWorld._41 = 800/2 - (float)g_rRectRole.right/2;
	g_matWorld._42 = 600/2 - (float)g_rRectRole.bottom/2;
}

void CDialogTest::PlayFrame()
{
	static DWORD StartTime = timeGetTime();
	if(timeGetTime() - StartTime > 60)
	{
		++m_nCurFrame;
		StartTime = timeGetTime();
	}
	if(m_nCurFrame == 8)
		m_nCurFrame = 0;

	SetFrame(m_nCurFrame);
}

void CDialogTest::SetFrame(INT num)
{
	m_nFrameWidth      = g_Desc.Width/8;
	m_nFrameHeight     = g_Desc.Height/8;
	g_rRectRole.left   = m_nFrameWidth * num;
	g_rRectRole.top    = m_nFrameSet * m_nFrameHeight;
	g_rRectRole.right  = g_rRectRole.left + m_nFrameWidth;
	g_rRectRole.bottom = g_rRectRole.top  + m_nFrameHeight;
	m_nCurFrame        = num;
}

void CDialogTest::SetFrameSet(INT num)
{
	m_nFrameSet = num;
}

void CDialogTest::Update()
{
	static float speed  = 8.f;
	static float speed2 = 0.71f;

	if(::GetKeyState('W')&0x8000)
	{
		m_Move = UP;

		if(::GetKeyState('A')&0x8000)
		{
			m_Move = LEFT_UP;
		}
		else if(::GetKeyState('D')&0x8000)
		{
			m_Move = RIGHT_UP;
		}
	}
	else if(::GetKeyState('S')&0x8000)
	{
		m_Move = DOWN;

		if(::GetKeyState('A')&0x8000)
		{
			m_Move = LEFT_DOWN;
		}
		else if(::GetKeyState('D')&0x8000)
		{
			m_Move = RIGHT_DOWN;
		}
	}
	else if(::GetKeyState('A')&0x8000)
	{
		m_Move = LEFT;
	}
	else if(::GetKeyState('D')&0x8000)
	{
		m_Move = RIGHT;
	}

	switch(m_Move)
	{
	case UP:
		//m_Move = UP;
		SetFrameSet(3);
		PlayFrame();
		g_matWorld._42 -= speed;
		m_vLocal.y = g_matWorld._42-m_vOffset.y;
		if (m_vLocal.y < 75) m_vOffset.y += m_vLocal.y-75;
		break;
	case DOWN:
		SetFrameSet(0);
		PlayFrame();
		g_matWorld._42 += speed;
		m_vLocal.y = g_matWorld._42-m_vOffset.y;
		if (m_vLocal.y > 450) m_vOffset.y += m_vLocal.y-450;
		break;
	case LEFT:
		SetFrameSet(1);
		PlayFrame();
		g_matWorld._41 -= speed;
		m_vLocal.x = g_matWorld._41-m_vOffset.x;
		if (m_vLocal.x < 100) m_vOffset.x += m_vLocal.x-100;
		break;
	case RIGHT:
		SetFrameSet(2);
		PlayFrame();
		g_matWorld._41 += speed;
		m_vLocal.x = g_matWorld._41-m_vOffset.x;
		if (m_vLocal.x > 600) m_vOffset.x += m_vLocal.x-600;
		break;
	case LEFT_UP:
		SetFrameSet(6);
		PlayFrame();
		g_matWorld._41 -= speed * speed2;
		g_matWorld._42 -= speed * speed2;
		m_vLocal.x = g_matWorld._41-m_vOffset.x;
		m_vLocal.y = g_matWorld._42-m_vOffset.y;
		if (m_vLocal.x < 100) m_vOffset.x += m_vLocal.x-100;
		if (m_vLocal.y < 75) m_vOffset.y += m_vLocal.y-75;
		break;
	case LEFT_DOWN:
		SetFrameSet(4);
		PlayFrame();
		g_matWorld._41 -= speed * speed2;
		g_matWorld._42 += speed * speed2;
		m_vLocal.x = g_matWorld._41-m_vOffset.x;
		m_vLocal.y = g_matWorld._42-m_vOffset.y;
		if (m_vLocal.x < 100) m_vOffset.x += m_vLocal.x-100;
		if (m_vLocal.y > 450) m_vOffset.y += m_vLocal.y-450;
		break;
	case RIGHT_UP:
		SetFrameSet(7);
		PlayFrame();
		g_matWorld._41 += speed * speed2;
		g_matWorld._42 -= speed * speed2;
		m_vLocal.x = g_matWorld._41-m_vOffset.x;
		m_vLocal.y = g_matWorld._42-m_vOffset.y;
		if (m_vLocal.x > 600) m_vOffset.x += m_vLocal.x-600;
		if (m_vLocal.y < 75) m_vOffset.y += m_vLocal.y-75;
		break;
	case RIGHT_DOWN:
		SetFrameSet(5);
		PlayFrame();
		g_matWorld._41 += speed * speed2;
		g_matWorld._42 += speed * speed2;
		m_vLocal.x = g_matWorld._41-m_vOffset.x;
		m_vLocal.y = g_matWorld._42-m_vOffset.y;
		if (m_vLocal.x > 600) m_vOffset.x += m_vLocal.x-600;
		if (m_vLocal.y > 450) m_vOffset.y += m_vLocal.y-450;
		break;
	}
}

void CDialogTest::Render()
{
	POINT offset; offset.x = (LONG) m_vOffset.x; offset.y = (LONG) m_vOffset.y;
	if(g_pd3dDevice)
	{
		g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
			D3DCOLOR_ARGB(0, 0, 0, 255), 1.0f, 0);

		g_pd3dDevice->BeginScene();

		// 绘制主角
		g_pSpriteRole->Begin(D3DXSPRITE_ALPHABLEND);
		Update();
		g_fDepthZ = 1 - (g_matWorld._42-m_vOffset.y + 112.f)/1000.f; 
		g_pSpriteRole->SetTransform(&g_matWorld);
		g_pSpriteRole->Draw(g_pTextureRole,&g_rRectRole,0,
			&D3DXVECTOR3(-m_vOffset.x,-m_vOffset.y,g_fDepthZ),0xFFFFFFFF);
		g_pSpriteRole->End();

		// 绘制背景
		//g_pSpriteBack->Begin(0);
		//int num = g_pDrawBrush[CURSOR_BACK].GetTextrueNum();
		//g_pSpriteBack->Draw(g_pppTestTexture[CURSOR_BACK][num],0,0,
		//	&D3DXVECTOR3(-m_vOffset.x,-m_vOffset.y,1.f),0xFFFFFFFF);
		//g_pSpriteBack->End();

		// 绘制元素层
		for(size_t i(0);i < m_stdMapBrushElement.size();++i)
		{
			m_stdMapBrushElement[i]->Render(NULL, offset);
		}

		// 绘制NPC层
		for(size_t i(0);i < m_stdMapNpcElement.size();++i)
		{
			m_stdMapNpcElement[i]->Render(NULL, offset);
		}

		// 绘制怪物层
		for(size_t i(0);i < m_stdMapMonsterElement.size();++i)
		{
			m_stdMapMonsterElement[i]->Render(NULL, offset);
		}

		g_pd3dDevice->EndScene();

		g_pd3dDevice->Present(0, 0, 0, 0);
	}
}

void CDialogTest::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Render();

	CDialog::OnTimer(nIDEvent);
}

void CDialogTest::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	SetFrameSet(0);
	SetFrame(0);
	CenterRole();

	// 画刷元素
	CMapEditView* view = (CMapEditView*)AfxGetMainWnd()->GetWindow(GW_CHILD);
	size_t brushNum    = view->m_stdMapBrushElement.size();
	size_t npcNum      = view->m_stdMapNpcElement.size();
	size_t monsterNum  = view->m_stdMapMonsterElement.size();

	for(DWORD i(0);i < brushNum;++i)
	{
		int index = view->m_stdMapBrushElement[i]->GetTextrueNum();
		int x     = (int)view->m_stdMapBrushElement[i]->GetPositionX();
		int y     = (int)view->m_stdMapBrushElement[i]->GetPositionY();
		int max   = view->m_stdMapBrushElement[i]->GetTextureMaxNum();

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(g_pd3dDevice,g_pppTestTexture[CURSOR_BRUSH],max);
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapBrushElement.push_back(draw);
	}

	// NPC元素
	for(DWORD i(0);i < npcNum;++i)
	{
		int index = view->m_stdMapNpcElement[i]->GetTextrueNum();
		int x     = (int)view->m_stdMapNpcElement[i]->GetPositionX();
		int y     = (int)view->m_stdMapNpcElement[i]->GetPositionY();
		int max   = view->m_stdMapNpcElement[i]->GetTextureMaxNum();

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(g_pd3dDevice,g_pppTestTexture[CURSOR_NPC],max);
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapNpcElement.push_back(draw);
	}

	// 怪物元素
	for(DWORD i(0);i < monsterNum;++i)
	{
		int index = view->m_stdMapMonsterElement[i]->GetTextrueNum();
		int x     = (int)view->m_stdMapMonsterElement[i]->GetPositionX();
		int y     = (int)view->m_stdMapMonsterElement[i]->GetPositionY();
		int max   = view->m_stdMapMonsterElement[i]->GetTextureMaxNum();

		CDrawBrush* draw = new CDrawBrush;
		draw->OnCreateByPoint(g_pd3dDevice,g_pppTestTexture[CURSOR_MONSTER],max);
		draw->SetTexturePosition((float)x,(float)y);
		draw->SetActive();
		draw->SetTextureNum(index);
		m_stdMapMonsterElement.push_back(draw);
	}
}

void CDialogTest::OnClose()
{
	// 解决STL的内存泄漏                     
	SAFE_STLDELETE(m_stdMapBrushElement);
	SAFE_STLDELETE(m_stdMapNpcElement);
	SAFE_STLDELETE(m_stdMapMonsterElement);

	KillTimer(0);
	CDialog::OnClose();
}

void CDialogTest::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFrame(0);
	m_Move = STAND;

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}
