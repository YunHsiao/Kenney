// ToolLoadDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ToolLoadDialog.h"


// CToolLoadDialog 对话框

IMPLEMENT_DYNAMIC(CToolLoadDialog, CDialog)

CToolLoadDialog::CToolLoadDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CToolLoadDialog::IDD, pParent)
{

}

CToolLoadDialog::~CToolLoadDialog()
{
}

void CToolLoadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}


BEGIN_MESSAGE_MAP(CToolLoadDialog, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CToolLoadDialog 消息处理程序

BOOL CToolLoadDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	ShowWindow(SW_SHOW);
	UpdateWindow();
	CenterWindow();

	// TODO:  在此添加额外的初始化
	m_Progress.SetRange(0,100);
	m_Progress.SetStep(10);
	m_Progress.SetPos(0);

	SetTimer(0,100,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CToolLoadDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_Progress.StepIt() == 100)
	{
		KillTimer(0);
		EndDialog(0);
	}

	CDialog::OnTimer(nIDEvent);
}
