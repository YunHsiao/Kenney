// MaskToolDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ToolBrushDialog.h"

// CMaskToolDialog 对话框

IMPLEMENT_DYNAMIC(CToolBrushDialog, CDialog)

CToolBrushDialog::CToolBrushDialog(CWnd* pParent /*=NULL*/)
: CDialog(CToolBrushDialog::IDD, pParent)
, m_dwMessage     (WM_BRUSH)
, m_nBrushPage	  (0)
{
}

CToolBrushDialog::~CToolBrushDialog()
{
}

void CToolBrushDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1,  m_Button[0]);
	DDX_Control(pDX, IDC_BUTTON2,  m_Button[1]);
	DDX_Control(pDX, IDC_BUTTON3,  m_Button[2]);
	DDX_Control(pDX, IDC_BUTTON4,  m_Button[3]);
	DDX_Control(pDX, IDC_BUTTON5,  m_Button[4]);
	DDX_Control(pDX, IDC_BUTTON6,  m_Button[5]);
	DDX_Control(pDX, IDC_BUTTON7,  m_Button[6]);
	DDX_Control(pDX, IDC_BUTTON8,  m_Button[7]);
	DDX_Control(pDX, IDC_BUTTON9,  m_Button[8]);
	DDX_Control(pDX, IDC_BUTTON10, m_Button[9]);
	DDX_Control(pDX, IDC_BUTTON11, m_Button[10]);
	DDX_Control(pDX, IDC_BUTTON12, m_Button[11]);
	DDX_Control(pDX, IDC_BUTTON13, m_Button[12]);
	DDX_Control(pDX, IDC_BUTTON14, m_Button[13]);
	DDX_Control(pDX, IDC_BUTTON15, m_Button[14]);
	DDX_Control(pDX, IDC_BUTTON16, m_Button[15]);
	DDX_Control(pDX, IDC_BUTTON17, m_Button[16]);
	DDX_Control(pDX, IDC_BUTTON18, m_Button[17]);
	DDX_Control(pDX, IDC_BUTTON19, m_Button[18]);
	DDX_Control(pDX, IDC_BUTTON20, m_Button[19]);
	DDX_Control(pDX, IDC_BUTTON21, m_Button[20]);
	DDX_Control(pDX, IDC_BUTTON22, m_Button[21]);
	DDX_Control(pDX, IDC_BUTTON23, m_Button[22]);
	DDX_Control(pDX, IDC_BUTTON24, m_Button[23]);
	DDX_Control(pDX, IDC_BUTTON25, m_Button[24]);
	DDX_Control(pDX, IDC_BUTTON26, m_Button[25]);
	DDX_Control(pDX, IDC_BUTTON27, m_Button[26]);
	DDX_Control(pDX, IDC_BUTTON28, m_Button[27]);
	DDX_Control(pDX, IDC_BUTTON29, m_Button[28]);
	DDX_Control(pDX, IDC_BUTTON30, m_Button[29]);
	DDX_Control(pDX, IDC_BUTTON31, m_Button[30]);
	DDX_Control(pDX, IDC_BUTTON32, m_Button[31]);

	DDX_Control(pDX, IDC_TAB, m_Tab);
}


BEGIN_MESSAGE_MAP(CToolBrushDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1,  &CToolBrushDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2,  &CToolBrushDialog::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3,  &CToolBrushDialog::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4,  &CToolBrushDialog::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5,  &CToolBrushDialog::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6,  &CToolBrushDialog::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7,  &CToolBrushDialog::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8,  &CToolBrushDialog::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9,  &CToolBrushDialog::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CToolBrushDialog::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CToolBrushDialog::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CToolBrushDialog::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CToolBrushDialog::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CToolBrushDialog::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CToolBrushDialog::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CToolBrushDialog::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CToolBrushDialog::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CToolBrushDialog::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &CToolBrushDialog::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CToolBrushDialog::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON21, &CToolBrushDialog::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON22, &CToolBrushDialog::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON23, &CToolBrushDialog::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON24, &CToolBrushDialog::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON25, &CToolBrushDialog::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CToolBrushDialog::OnBnClickedButton26)
	ON_BN_CLICKED(IDC_BUTTON27, &CToolBrushDialog::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON28, &CToolBrushDialog::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON29, &CToolBrushDialog::OnBnClickedButton29)
	ON_BN_CLICKED(IDC_BUTTON30, &CToolBrushDialog::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON31, &CToolBrushDialog::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CToolBrushDialog::OnBnClickedButton32)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CToolBrushDialog::OnTcnSelchangeMasktab)
	ON_LBN_SELCHANGE(IDC_BRUSHLIST, &CToolBrushDialog::OnLbnSelchangeBrushlist)
END_MESSAGE_MAP()



BOOL CToolBrushDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 获取自身的宽高
	GetWindowRect(&m_rWndRect);
	m_rWndRect.right  -= m_rWndRect.left;
	m_rWndRect.bottom -= m_rWndRect.top;

	// 添加LIST控件项
	CListBox* list = (CListBox*)GetDlgItem(IDC_BRUSHLIST);
	for(int i(0);i < 10;++i)
	{
		CString str = _T("");
		str.Format(_T("%d"),i);
		list->InsertString(i,str.GetBuffer());
	}

	tinyxml2::XMLDocument File;
	if(File.LoadFile("Res/ButtonStr.xml"))
	{
		AfxMessageBox(_T("Res/ButtonStr.xml 文件加载失败!"));
		return FALSE;
	}

	tinyxml2::XMLElement* tRoot = File.FirstChildElement();
	tinyxml2::XMLElement* tName = tRoot->FirstChildElement();
	int tabNum = atoi(tName->FirstChild()->Value());

	// 初始化标签
	for(int i(0);i<tabNum;++i)
	{
		tName = tName->NextSiblingElement();
		WCHAR str[MAX_PATH] = {0};
		LPCSTR buff = tName->Attribute("str");
		MultiByteToWideChar(CP_ACP,0,buff,-1,str,(int)strlen(buff)+1);
		m_Tab.InsertItem(i,str);
	}

	tinyxml2::XMLElement* tTemp = tName;
	std::vector<std::wstring> pStr;
	for (int j(0); j < tabNum; ++j) {
		tTemp = tTemp->NextSiblingElement();
		m_nMaxNum.push_back(atoi(tTemp->FirstChild()->Value()));
		for(int i(0);i < m_nMaxNum.back(); ++i)
		{
			tTemp = tTemp->NextSiblingElement();
			WCHAR str[MAX_PATH] = {0};
			LPCSTR buff = tTemp->Attribute("str");
			MultiByteToWideChar(CP_ACP,0,buff,-1,str,(int)strlen(buff)+1);
			pStr.push_back(str);
		}		
		m_pStr.push_back(pStr);
		pStr.clear();
	}

	// 设置默认层
	m_Tab.SetCurSel(0);
	for (int n(IDI_ICON4), i(0); i < TAB_BUTTON_MAXNUM; ++i) {
		if (n < (IDI_ICON4 + m_nMaxNum[0])) {
			m_Button[i].SetIcon(::LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(n)));
			m_Button[i].SetWindowText(m_pStr[0][i].c_str());
			m_Button[i].ShowWindow(SW_SHOW);
			++n;
		} else m_Button[i].ShowWindow(SW_HIDE);
	}

	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CMaskToolDialog 消息处理程序

// 过滤掉系统关闭按键	
BOOL CToolBrushDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if(::GetAsyncKeyState(VK_LMENU) & ::GetAsyncKeyState(VK_F4))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CToolBrushDialog::OnTcnSelchangeMasktab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int sel = m_Tab.GetCurSel(), icon;
	m_nBrushPage = GetPageBeginningIndex(sel);
	switch(sel) {
	case 0:
		icon = IDI_ICON4;
		m_dwMessage = WM_BRUSH;
		break;
	case 1:
		icon = IDI_ICON41;
		m_dwMessage = WM_BRUSH;
		break;
	case 2:
		icon = IDI_ICON75;
		m_dwMessage = WM_BRUSH;
		break;
	case 3:
		icon = IDI_ICON98;
		m_dwMessage = WM_BRUSH;
		break;
	case 4:
		icon = IDI_ICON116;
		m_dwMessage = WM_BRUSH;
		break;
	case 5:
		icon = IDI_ICON134;
		m_dwMessage = WM_BRUSH;
		break;
	case 6:
		icon = IDI_ICON59;
		m_dwMessage = WM_BRUSH;
		break;
	case 7:
		icon = IDI_ICON152;
		m_dwMessage = WM_BRUSH;
		break;
	case 8:
		icon = IDI_ICON184;
		m_dwMessage = WM_BRUSH;
		break;
	case 9:
		icon = IDI_ICON23;
		m_dwMessage = WM_MONSTER;
		m_nBrushPage = 0;
		break;
	case 10:
		icon = IDI_ICON93;
		m_dwMessage = WM_NPC;
		m_nBrushPage = 0;
		break;
	case 11:
		icon = IDI_ICONCOLL;
		m_dwMessage = WM_COLL;
		m_nBrushPage = 0;
		break;
	case 12:
		icon = IDI_ICONTRIG;
		m_dwMessage = WM_TRIG;
		m_nBrushPage = 0;
		break;
	}

	int n = icon;
	for (int i(0);i < TAB_BUTTON_MAXNUM;++i) {
		if (n < (icon + m_nMaxNum[sel])) {
			m_Button[i].SetIcon(::LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(n)));
			m_Button[i].SetWindowText(m_pStr[sel][i].c_str());
			m_Button[i].ShowWindow(SW_SHOW);
			++n;
		} else m_Button[i].ShowWindow(SW_HIDE);
	}
	CWnd* cView = AfxGetMainWnd()->GetWindow(GW_CHILD); 
	if (m_dwMessage != WM_COLL) ::PostMessage(cView->m_hWnd,m_dwMessage,MODIFY,0);
	*pResult = 0;
}

void CToolBrushDialog::OnLbnSelchangeBrushlist()
{
	CListBox* list = (CListBox*)GetDlgItem(IDC_BRUSHLIST);
	int num = list->GetCurSel();

	CWnd* cView = AfxGetMainWnd()->GetWindow(GW_CHILD);
	::PostMessage(cView->m_hWnd,WM_TRIGMAPID,num,num);
}

void CToolBrushDialog::OnBnClickedButton1()
{
	static BOOL b(TRUE);
	m_Button[0].SetState(b);
	b = !b;

	// 消息,第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,0+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton2()
{
	static BOOL b(TRUE);
	m_Button[1].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,1+m_nBrushPage);
}
void CToolBrushDialog::OnBnClickedButton3()
{
	static BOOL b(TRUE);
	m_Button[2].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,2+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton4()
{
	static BOOL b(TRUE);
	m_Button[3].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,3+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton5()
{
	static BOOL b(TRUE);
	m_Button[4].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,4+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton6()
{
	static BOOL b(TRUE);
	m_Button[5].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,5+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton7()
{
	static BOOL b(TRUE);
	m_Button[6].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,6+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton8()
{
	static BOOL b(TRUE);
	m_Button[7].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,7+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton9()
{
	static BOOL b(TRUE);
	m_Button[8].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,8+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton10()
{
	static BOOL b(TRUE);
	m_Button[9].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,9+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton11()
{
	static BOOL b(TRUE);
	m_Button[10].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,10+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton12()
{
	static BOOL b(TRUE);
	m_Button[11].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,11+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton13()
{
	static BOOL b(TRUE);
	m_Button[12].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,12+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton14()
{
	static BOOL b(TRUE);
	m_Button[13].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,13+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton15()
{
	static BOOL b(TRUE);
	m_Button[14].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,14+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton16()
{
	static BOOL b(TRUE);
	m_Button[15].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,15+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton17()
{
	static BOOL b(TRUE);
	m_Button[16].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,16+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton18()
{
	static BOOL b(TRUE);
	m_Button[17].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,17+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton19()
{
	static BOOL b(TRUE);
	m_Button[18].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,18+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton20()
{
	static BOOL b(TRUE);
	m_Button[19].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,19+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton21()
{
	static BOOL b(TRUE);
	m_Button[20].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,20+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton22()
{
	static BOOL b(TRUE);
	m_Button[21].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,21+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton23()
{
	static BOOL b(TRUE);
	m_Button[22].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,22+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton24()
{
	static BOOL b(TRUE);
	m_Button[23].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,23+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton25()
{
	static BOOL b(TRUE);
	m_Button[24].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,24+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton26()
{
	static BOOL b(TRUE);
	m_Button[25].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,25+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton27()
{
	static BOOL b(TRUE);
	m_Button[26].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,26+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton28()
{
	static BOOL b(TRUE);
	m_Button[27].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,27+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton29()
{
	static BOOL b(TRUE);
	m_Button[28].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,28+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton30()
{
	static BOOL b(TRUE);
	m_Button[29].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,29+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton31()
{
	static BOOL b(TRUE);
	m_Button[30].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,30+m_nBrushPage);
}

void CToolBrushDialog::OnBnClickedButton32()
{
	static BOOL b(TRUE);
	m_Button[31].SetState(b);
	b = !b;

	// s1,s2 == 消息 n == 第几个按钮
	BUTTON_POSTMESSAGE(m_dwMessage,31+m_nBrushPage);
}