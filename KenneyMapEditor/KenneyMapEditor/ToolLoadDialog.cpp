// ToolLoadDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ToolLoadDialog.h"


// CToolLoadDialog �Ի���

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


// CToolLoadDialog ��Ϣ�������

BOOL CToolLoadDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	ShowWindow(SW_SHOW);
	UpdateWindow();
	CenterWindow();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_Progress.SetRange(0,100);
	m_Progress.SetStep(10);
	m_Progress.SetPos(0);

	SetTimer(0,100,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CToolLoadDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_Progress.StepIt() == 100)
	{
		KillTimer(0);
		EndDialog(0);
	}

	CDialog::OnTimer(nIDEvent);
}
