
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

// 悬浮窗口头文件
#include "ToolAlphaDialog.h"
#include "ToolBrushDialog.h"

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
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