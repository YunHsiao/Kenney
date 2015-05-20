#pragma once
#include "afxwin.h"
#include "afxext.h"
#define   MODIFY   9999 //表示可以编辑当前类的元素。

// CMaskToolDialog 对话框

class CToolBrushDialog : public CDialog
{
	DECLARE_DYNAMIC(CToolBrushDialog)

public:
	CToolBrushDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolBrushDialog();

// 对话框数据
	enum { IDD = IDD_MASKDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton24();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton26();
	afx_msg void OnBnClickedButton27();
	afx_msg void OnBnClickedButton28();
	afx_msg void OnBnClickedButton29();
	afx_msg void OnBnClickedButton30();
	afx_msg void OnBnClickedButton31();
	afx_msg void OnBnClickedButton32();
	afx_msg void OnTcnSelchangeMasktab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchangeBrushlist();

public:
	virtual BOOL OnInitDialog();

public:
	inline RECT GetWndRect()
	{
		return m_rWndRect;
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	inline INT GetPageBeginningIndex(INT page) {
		INT index(0);
		for (INT i(0); i < page; ++i) {
			index += m_nMaxNum[i];
		}
		return index;
	}
	CButton      m_Button[TAB_BUTTON_MAXNUM];
	CTabCtrl     m_Tab;
	std::vector<std::vector<std::wstring> > m_pStr;
	std::vector<INT> m_nMaxNum;
	INT			 m_nBrushPage;
	DWORD        m_dwMessage;
	RECT         m_rWndRect;
};