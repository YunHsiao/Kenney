#ifndef __KENNEY_TASKEDITOR_H__
#define __KENNEY_TASKEDITOR_H__

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
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