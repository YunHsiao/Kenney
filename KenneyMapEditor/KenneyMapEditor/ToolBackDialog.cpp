// SelectBackDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ToolBackDialog.h"

extern CDrawBrush*      g_pDrawBrush;
static int              g_nCurSel = 0;

// CSelectBackDialog 对话框

IMPLEMENT_DYNAMIC(CToolBackDialog, CDialog)

CToolBackDialog::CToolBackDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CToolBackDialog::IDD, pParent)
	, m_nBackNum(0)
{

}

CToolBackDialog::~CToolBackDialog()
{
	SAFE_DELETE_ARRAY(m_pBitmap);
}

void CToolBackDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_BACKLISTBOX,m_ListBox);
	DDX_Control(pDX,IDC_MAPIMAGE,m_Static);
}


BEGIN_MESSAGE_MAP(CToolBackDialog, CDialog)
	ON_BN_CLICKED(IDC_BACKOKBUTTON, &CToolBackDialog::OnBnClickedBackokbutton)
	ON_BN_CLICKED(IDC_BACKCANCELBUTTON, &CToolBackDialog::OnBnClickedBackcancelbutton)
	ON_LBN_SELCHANGE(IDC_BACKLISTBOX, &CToolBackDialog::OnLbnSelchangeBacklistbox)
END_MESSAGE_MAP()


// CSelectBackDialog 消息处理程序

BOOL CToolBackDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_nBackNum = g_pDrawBrush[CURSOR_BACK].GetTextureMaxNum();

	// 初始化图像控件
	m_pBitmap = new HBITMAP[m_nBackNum];

	for(int i(0);i < m_nBackNum;++i)
	{
		m_pBitmap[i] = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP1+i));
		m_Static.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	}
	
	//m_pBitmap[9] = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP10));
	//m_Static.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	//
	//m_pBitmap[10] = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP11));
	//m_Static.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);

	m_Static.SetBitmap(m_pBitmap[0]);

	// 给列表框添加项
	for(int i(0);i < m_nBackNum;++i)
	{
		std::wstringstream sstr;
		sstr << L"Map";
		sstr << i;
		m_ListBox.AddString(sstr.str().c_str());
	}

	// 默认背景 == 当前背景
	m_ListBox.SetCurSel(g_nCurSel);
	CMapEditView* cView = (CMapEditView*)AfxGetMainWnd()->GetWindow(GW_CHILD);
	::PostMessage(cView->m_hWnd,WM_BACK,g_nCurSel,0);

	// 下拉添加同样多项
	for(int i(0);i < m_nBackNum;++i)
	{
		WCHAR buff[8] = {0};
		_itow_s(i,buff,-1,10);
		((CComboBox*)GetDlgItem(IDC_TREECOMBO))->AddString(buff);
	}

	// 默认ID == 当前ID
	int n = cView->GetMapID();
	((CComboBox*)GetDlgItem(IDC_TREECOMBO))->SetCurSel(n);

	// 默认名称 == 当前名称
	LPCWSTR name = cView->GetMapName();
	((CEdit*)GetDlgItem(IDC_TREEEDIT))->SetWindowText(name);

	CenterWindow();
	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 消息过滤
BOOL CToolBackDialog::PreTranslateMessage(MSG* pMsg)
{
	// 过滤掉ESC键执行关闭对话框
	if(WM_KEYDOWN == pMsg->message && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// 点击确定按钮时
void CToolBackDialog::OnBnClickedBackokbutton()
{
	// 如果选择正确则关闭对话框
	CString str;
	((CEdit*)GetDlgItem(IDC_TREEEDIT))->GetWindowText(str);

	if(str.IsEmpty())
	{
		AfxMessageBox(L"名字不能为空!");
		return;
	}

	// 确定后背景的切换
	g_nCurSel = m_ListBox.GetCurSel();
	CMapEditView* cView = (CMapEditView*)AfxGetMainWnd()->GetWindow(GW_CHILD);
	::PostMessage(cView->m_hWnd,WM_BACK,g_nCurSel,0);

	// 确定后设置地图的名字
	WCHAR wbuff[32] = {0};
	((CEdit*)GetDlgItem(IDC_TREEEDIT))->GetWindowText(wbuff,32);

	if(0 != wbuff[0])
	{
		cView->SetMapName(wbuff);
	}

	// 确定后设置地图的ID
	int sel = ((CComboBox*)GetDlgItem(IDC_TREECOMBO))->GetCurSel();
	cView->SetMapID(sel);

	EndDialog(IDOK);
}

void CToolBackDialog::OnBnClickedBackcancelbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(IDCANCEL);
}

void CToolBackDialog::OnLbnSelchangeBacklistbox()
{
	// TODO: 在此添加控件通知处理程序代码
	g_nCurSel = m_ListBox.GetCurSel();
	m_Static.SetBitmap(m_pBitmap[g_nCurSel]);
}
