
// MapEditDoc.h : CMapEditDoc ��Ľӿ�
//


#pragma once


class CMapEditDoc : public CDocument
{
protected: // �������л�����
	CMapEditDoc();
	DECLARE_DYNCREATE(CMapEditDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CMapEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


