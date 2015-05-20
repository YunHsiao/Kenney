
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

// ��������ͷ�ļ�
#include "ToolAlphaDialog.h"
#include "ToolBrushDialog.h"

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMove(int x, int y);
private:
	CToolAlphaDialog*                        m_pAlphaTool;
	CToolBrushDialog*                        m_pBrushTool;
	BOOL                                     m_bIsCreate;
public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};