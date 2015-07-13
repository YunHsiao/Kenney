#ifndef __KENNEY_TASKEDITOR_H__
#define __KENNEY_TASKEDITOR_H__

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"

class CKenneyTaskEditorApp : public CWinApp
{
public:
	CKenneyTaskEditorApp();

	public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CKenneyTaskEditorApp theApp;

#endif