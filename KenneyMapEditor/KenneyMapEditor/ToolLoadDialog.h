#pragma once
#include "afxcmn.h"


// CToolLoadDialog �Ի���

class CToolLoadDialog : public CDialog
{
	DECLARE_DYNAMIC(CToolLoadDialog)

public:
	CToolLoadDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolLoadDialog();

// �Ի�������
	enum { IDD = IDD_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
