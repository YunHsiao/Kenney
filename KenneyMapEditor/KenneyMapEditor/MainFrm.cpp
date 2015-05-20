
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "MapEditor.h"

#include "MainFrm.h"

extern int g_nWndWidth;
extern int g_nWndHeight;

extern int g_nAppWidth;
extern int g_nAppHeight;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMainFrame ����/����

CMainFrame::CMainFrame()
:m_bIsCreate    (FALSE)
,m_pAlphaTool   (NULL)
,m_pBrushTool   (NULL)
{
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE(m_pAlphaTool);
	SAFE_DELETE(m_pBrushTool);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ������������
	m_pBrushTool = new CToolBrushDialog;
	m_pBrushTool->Create(IDD_DIALOGBRUSH,this);

	// ������������(��������)
	m_pAlphaTool = new CToolAlphaDialog;
	m_pAlphaTool->Create(IDD_DIALOGSETTING,this);
	m_pAlphaTool->ShowWindow(SW_SHOW);

	RECT rectWnd, rectCen;
	GetWindowRect(&rectWnd);
	GetClientRect(&rectCen);
	INT WndWidth  = rectWnd.right  - rectWnd.left;
	INT WndHeight = rectWnd.bottom - rectWnd.top;
	INT WndWSub   = WndWidth  - rectCen.right;    // ���ұ߿���ɵ�ƫ��
	INT WndHSub   = WndHeight - rectCen.bottom;   // ���±�����ɵ�ƫ��

	// ��Ҫ�����Ĵ��ڵĴ�С
	g_nWndWidth  = 800 + WndWSub;
	g_nWndHeight = 600 + WndHSub;

	m_pAlphaTool->GetWindowRect(&rectWnd);
	m_pBrushTool->GetWindowRect(&rectCen);
	g_nAppWidth = g_nWndWidth + rectCen.right - rectCen.left;
	g_nAppHeight = rectWnd.bottom - rectWnd.top + rectCen.bottom - rectCen.top;

	MoveWindow((::GetSystemMetrics(SM_CXSCREEN)-g_nAppWidth)/2, 
		(::GetSystemMetrics(SM_CYSCREEN)-g_nAppHeight)/2, 
		g_nWndWidth, g_nWndHeight);
	SetTimer(0, 17, NULL);
	m_bIsCreate = TRUE;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	// ע��:
	// Ϊ�˴��ڶ�����׼ȷԤ���������������ﲢ�������յĴ��ڲ���
	cs.x        = (::GetSystemMetrics(SM_CXSCREEN) - 800)/3;
	cs.y        = (::GetSystemMetrics(SM_CYSCREEN) - 600)/3;
	cs.cx       = 800;
	cs.cy       = 600;
	cs.style    = WS_SYSMENU|WS_VISIBLE;
	cs.lpszName = L"���ص�ͼ�༭��";

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);

	RECT rectWnd;
	GetWindowRect(&rectWnd);

	// ����������λ�õ����������ڵ�λ��

	if(m_pBrushTool) // ���Ͻǵ�һ��(��ˢ)
	{
		m_pBrushTool->MoveWindow(
			rectWnd.right+g_nWndWidth-800,
			rectWnd.top,
			m_pBrushTool->GetWndRect().right,
			m_pBrushTool->GetWndRect().bottom);
	}

	if(m_pAlphaTool) // ���½ǵڶ���(����)
	{
		m_pAlphaTool->MoveWindow(
			rectWnd.right+g_nWndWidth-800,
			rectWnd.top + m_pBrushTool->GetWndRect().bottom + g_nWndWidth-800,
			m_pAlphaTool->GetWndRect().right,
			m_pAlphaTool->GetWndRect().bottom);
	}

	if(m_bIsCreate)
	{
		CMapEditView* cView = (CMapEditView*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		cView->Render();
	}
}

void CMainFrame::OnClose()
{
	CMapEditView* cView = (CMapEditView*)AfxGetMainWnd()->GetWindow(GW_CHILD);
	cView->IsSaveMap();

	KillTimer(0);
	AnimateWindow(300, AW_BLEND | AW_HIDE);
	m_pBrushTool->AnimateWindow(300, AW_BLEND | AW_HIDE);
	m_pAlphaTool->AnimateWindow(300, AW_BLEND | AW_HIDE);
	CFrameWnd::OnClose();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	CMapEditView* cView = (CMapEditView*)(AfxGetMainWnd()->GetWindow(GW_CHILD));
	cView->Update();
	cView->Render();

	CFrameWnd::OnTimer(nIDEvent);
}