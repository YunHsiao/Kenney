#pragma once
#include "afxcmn.h"


// CSelectBackDialog �Ի���

class CToolBackDialog : public CDialog
{
	DECLARE_DYNAMIC(CToolBackDialog)

public:
	CToolBackDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolBackDialog();

// �Ի�������
	enum { IDD = IDD_TREEDIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
