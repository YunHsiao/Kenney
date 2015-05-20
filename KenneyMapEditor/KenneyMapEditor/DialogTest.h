#pragma once

#include "DrawBrush.h"

// CDialogTest 对话框

enum ROLE_MOVE
{
	STAND,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	LEFT_UP,
	LEFT_DOWN,
	RIGHT_UP,
	RIGHT_DOWN
};

class CDialogTest : public CDialog
{
	DECLARE_DYNAMIC(CDialogTest)

public:
	CDialogTest(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogTest();

// 对话框数据
	enum { IDD = IDD_DIALOGTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void Update();
	void Render();
	void Cleanup();
public:
	void CenterRole();
	void PlayFrame();
	void SetFrame(INT num);
	void SetFrameSet(INT num);
private:
	INT                                  m_nFrameWidth;
	INT                                  m_nFrameHeight;
	INT                                  m_nFrameSet;
	INT                                  m_nCurFrame;
	ROLE_MOVE                            m_Move;
private:
	std::vector<CDrawBrush*>    m_stdMapBrushElement;  
	std::vector<CDrawBrush*>    m_stdMapNpcElement; 
	std::vector<CDrawBrush*>    m_stdMapMonsterElement; 
	D3DXVECTOR2 m_vOffset, m_vLocal;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
