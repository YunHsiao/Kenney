#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CBackAlphaDialog �Ի���

class CToolAlphaDialog : public CDialog
{
	DECLARE_DYNAMIC(CToolAlphaDialog)

public:
	CToolAlphaDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolAlphaDialog();

// �Ի�������
	enum { IDD = IDD_BACKALPHA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl    m_SliderAlpha1;
	CSliderCtrl    m_SliderAlpha2;
	CStatic        m_StaticText1;
	CStatic        m_StaticText2;
	RECT           m_rWndRect;
public:
	inline RECT GetWndRect()
	{
		return m_rWndRect;
	}
public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
