#pragma once
#include "afxcmn.h"


// CToolLoadDialog 对话框

class CToolLoadDialog : public CDialog
{
	DECLARE_DYNAMIC(CToolLoadDialog)

public:
	CToolLoadDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolLoadDialog();

// 对话框数据
	enum { IDD = IDD_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
