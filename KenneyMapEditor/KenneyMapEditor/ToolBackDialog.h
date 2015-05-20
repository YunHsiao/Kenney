#pragma once
#include "afxcmn.h"


// CSelectBackDialog 对话框

class CToolBackDialog : public CDialog
{
	DECLARE_DYNAMIC(CToolBackDialog)

public:
	CToolBackDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolBackDialog();

// 对话框数据
	enum { IDD = IDD_TREEDIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CListBox   m_ListBox;
	CStatic    m_Static;
	HBITMAP*   m_pBitmap;
	INT        m_nBackNum;
public:
	afx_msg void OnBnClickedBackokbutton();
	afx_msg void OnBnClickedBackcancelbutton();
	afx_msg void OnLbnSelchangeBacklistbox();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
