
// MapEditDoc.cpp : CMapEditDoc ���ʵ��
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


// CMapEditDoc ����/����

CMapEditDoc::CMapEditDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CMapEditDoc::~CMapEditDoc()
{
}

BOOL CMapEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CMapEditDoc ���л�

void CMapEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CMapEditDoc ���

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


// CMapEditDoc ����
