
// MapEdit.h : MapEdit Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "MapEditorView.h"

// CMapEditApp:
// �йش����ʵ�֣������ MapEdit.cpp
//

class CMapEditApp : public CWinApp
{
public:
	CMapEditApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMapEditApp theApp;
