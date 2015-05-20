// BackAlphaDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ToolAlphaDialog.h"

// CBackAlphaDialog 对话框

IMPLEMENT_DYNAMIC(CToolAlphaDialog, CDialog)

CToolAlphaDialog::CToolAlphaDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CToolAlphaDialog::IDD, pParent)
{

}

CToolAlphaDialog::~CToolAlphaDialog()
{
}

void CToolAlphaDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderAlpha1);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderAlpha2);
	DDX_Control(pDX, IDC_STATIC1, m_StaticText1);
	DDX_Control(pDX, IDC_STATIC2, m_StaticText2);
}


BEGIN_MESSAGE_MAP(CToolAlphaDialog, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CToolAlphaDialog::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CToolAlphaDialog::OnNMCustomdrawSlider2)
	ON_BN_CLICKED(IDC_CHECK1, &CToolAlphaDialog::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CToolAlphaDialog::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CToolAlphaDialog::OnBnClickedCheck3)
END_MESSAGE_MAP()


// CBackAlphaDialog 消息处理程序

BOOL CToolAlphaDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 获得自身的宽和高
	GetWindowRect(&m_rWndRect);
	m_rWndRect.right  -= m_rWndRect.left;
	m_rWndRect.bottom -= m_rWndRect.top;

	// 初始化背景滑动条
	m_SliderAlpha1.SetRange(0,255);                  // 滑动条的范围
	m_SliderAlpha1.SetPos(BACK_DEFAULT_ALPHAVALUE);  // 滑动条的初始位置

	// 初始化层级滑动条
	m_SliderAlpha2.SetRange(0,255);                  // 滑动条的范围
	m_SliderAlpha2.SetPos(255);                      // 滑动条的初始位置

	// 初始化CheckBox
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(BST_CHECKED);

	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 过滤ESC键与Alt+F4
BOOL CToolAlphaDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if(::GetAsyncKeyState(VK_LMENU) & ::GetAsyncKeyState(VK_F4))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CToolAlphaDialog::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	// 背景透明度
	WCHAR str[4] = {0};
	int s = m_SliderAlpha1.GetPos();
	_itow_s(s,str,4,10);
	m_StaticText1.SetWindowText(str);

	CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
	::PostMessage(cview->m_hWnd,WM_BACKALPHA,s,0);

	*pResult = 0;
}

void CToolAlphaDialog::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	// 层级透明度
	WCHAR str[4] = {0};
	int s = m_SliderAlpha2.GetPos();
	_itow_s(s,str,4,10);
	m_StaticText2.SetWindowText(str);

	CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
	::PostMessage(cview->m_hWnd,WM_LAYOUTALPHA,s,0);

	*pResult = 0;
}

void CToolAlphaDialog::OnBnClickedCheck1()
{
	if(BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck())
	{
		CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		::PostMessage(cview->m_hWnd,WM_DRAWMESH,TRUE,0);
	}
	else if(BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck())
	{
		CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		::PostMessage(cview->m_hWnd,WM_DRAWMESH,FALSE,0);
	}
}	

void CToolAlphaDialog::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck())
	{
		CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		::PostMessage(cview->m_hWnd,WM_DRAWRED,TRUE,0);
	}
	else if(BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck())
	{
		CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		::PostMessage(cview->m_hWnd,WM_DRAWRED,FALSE,0);
	}
}

void CToolAlphaDialog::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	if(BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck())
	{
		CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		::PostMessage(cview->m_hWnd,WM_MAPINFO,TRUE,0);
	}
	else if(BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck())
	{
		CWnd* cview = (CWnd*)AfxGetMainWnd()->GetWindow(GW_CHILD);
		::PostMessage(cview->m_hWnd,WM_MAPINFO,FALSE,0);
	}
}
