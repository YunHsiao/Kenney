
// MapEditDoc.cpp : CMapEditDoc 类的实现
//

#include "stdafx.h"
#include "MapEditor.h"

#include "MapEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapEditDoc

IMPLEMENT_DYNCREATE(CMapEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CMapEditDoc, CDocument)
END_MESSAGE_MAP()


// CMapEditDoc 构造/析构

CMapEditDoc::CMapEditDoc()
{
	// TODO: 在此添加一次性构造代码

}

CMapEditDoc::~CMapEditDoc()
{
}

BOOL CMapEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMapEditDoc 序列化

void CMapEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CMapEditDoc 诊断

#ifdef _DEBUG
void CMapEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMapEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMapEditDoc 命令
