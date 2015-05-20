// BackAlphaDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ToolAlphaDialog.h"

// CBackAlphaDialog �Ի���

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


// CBackAlphaDialog ��Ϣ�������

BOOL CToolAlphaDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �������Ŀ�͸�
	GetWindowRect(&m_rWndRect);
	m_rWndRect.right  -= m_rWndRect.left;
	m_rWndRect.bottom -= m_rWndRect.top;

	// ��ʼ������������
	m_SliderAlpha1.SetRange(0,255);                  // �������ķ�Χ
	m_SliderAlpha1.SetPos(BACK_DEFAULT_ALPHAVALUE);  // �������ĳ�ʼλ��

	// ��ʼ���㼶������
	m_SliderAlpha2.SetRange(0,255);                  // �������ķ�Χ
	m_SliderAlpha2.SetPos(255);                      // �������ĳ�ʼλ��

	// ��ʼ��CheckBox
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(BST_CHECKED);

	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// ����ESC����Alt+F4
BOOL CToolAlphaDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if(::GetAsyncKeyState(VK_LMENU) & ::GetAsyncKeyState(VK_F4))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CToolAlphaDialog::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// ����͸����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// �㼶͸����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
