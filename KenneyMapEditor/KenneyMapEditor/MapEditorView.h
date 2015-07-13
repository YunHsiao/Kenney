#pragma once

// ������
#include "DrawBrush.h"
#include "DrawColl.h"

#include "ToolBackDialog.h"
#include "DialogTest.h"

// CMapEditView ��ͼ

class CMapEditView : public CView
{
	DECLARE_DYNCREATE(CMapEditView)

protected:
	CMapEditView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CMapEditView();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

private:
	HRESULT RenderText();
// �ṩ��MainFrm���õ�D3D��Ⱦ����
public:
	HRESULT Update();
	HRESULT Render();
// ��Ϣӳ�亯��
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnEditUndo();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileNew();
	afx_msg void OnFileClose();

	afx_msg void OnLayoutBrush();
	afx_msg void OnLayoutNpc();
	afx_msg void OnLayoutMonster();
	afx_msg void OnLayoutColl();
	afx_msg void OnLayoutTrigger();
	afx_msg void OnLayoutAll();
	afx_msg void OnBackselect();

public:
	inline void SetMapName(std::wstring name)
	{
		m_szMapName = name;
	}
	inline LPCWSTR GetMapName()
	{
		return (m_szMapName.c_str());
	}
	inline void SetMapID(INT num)
	{
		m_nMapID = num;
	}
	inline INT GetMapID()
	{
		return m_nMapID;
	}

public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// ѯ���Ƿ񱣴��ͼ
	void IsSaveMap();
private:
	// ʰ������õ���ز���
	void MousePickBrushMap(CPoint point);     
	void MouseDownBrushMap(CPoint point);   

	void MousePickNpcMap(CPoint point);      
	void MouseDownNpcMap(CPoint point);   

	void MousePickMonsterMap(CPoint point);    
	void MouseDownMonsterMap(CPoint point); 

	void MousePickTrigMap(CPoint point);       
	void MouseDownTrigMap(CPoint point);      
	      
	void SaveFileByBin(std::wstring PathName); // �����ļ�Ϊ������
	void SaveFileByTex(std::wstring PathName); // �����ļ�Ϊ�ı���ʽ
	void SaveFileByXml(std::wstring PathName); // �����ļ�Ϊ�ı���ʽ
	void LoadFileByBin(std::wstring PathName); // ��ȡ�ļ�Ϊ������
	void LoadFileByTex(std::wstring PathName); // ��ȡ�ļ�Ϊ�ı���ʽ
	void LoadFileByXml(std::wstring PathName); // ��ȡ�ļ�Ϊ������
	bool LoadCfgFile();   // ��ȡ�����ļ����ж�
	void SaveCfgFile();   // ��׷�ӷ�ʽ���������ļ�
	//����ͼ������ĸ��Ĳ���
	void SetMapCursorNum(LAYOUT_MAP map,LAYOUT_CURSOR cursor,INT num); 
	// �Ե�ͼ�㼶��͸���ȸ���
	void SetMapLayoutColor(D3DCOLOR brush,D3DCOLOR npc,D3DCOLOR monster,D3DCOLOR coll,D3DCOLOR trig);   
	// ��������
	void RenderLine();
	void Clear();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMaptest();
	typedef std::vector<CDrawBrush*> Elements;
	typedef std::vector<CDrawBrush*>::iterator ElementsIt;

public:
	Elements		m_stdMapBrushElement;  
	Elements		m_stdMapNpcElement; 
	Elements		m_stdMapMonsterElement;
	Elements		m_stdMapTrigElement; 
	BOOL			m_bIsNewMap;		// �Ƿ��½���ͼ״̬
	CDrawColl*		m_pDrawMapColl;		// ������ײ��Ļ���
private:
	CDialogTest*	m_pMapTest;
	std::wstring	m_szMapName;		// ��ͼ����
	CPoint			m_pLastBrush;
	BOOL			m_bIsLBotton;		// �Ƿ�����������(��ײ�������)
	BOOL			m_bIsLine;			// �Ƿ��������
	BOOL			m_bIsRedLine;		// �Ƿ���ƺ�ɫ�߿�
	BOOL			m_bIsMapInfo;		// �Ƿ���ʾ��ͼ��Ϣ
	BOOL			m_bIsInitResruce;	// ��ʼ����Դһ��
	LAYOUT_CURSOR	m_emCursorLayout;	// ���㼶״̬
	LAYOUT_MAP		m_emMapLayout;		// ��ͼ�㼶״̬
	POINT			m_nMapCoord;		// ��ǰ��ͼ����
	POINT			m_nMapPrevCoord;	// ��ǰ��ͼ����
	POINT			m_nCurrentCoord;	// ��굱ǰ����
	POINT			m_nStartCoord;		// �����ʼ����
	POINT			m_nDefaultCoord;	// Ĭ��ƫ������
	INT				m_nTriggerNum;		// ������ı��
	INT				m_nLayoutAlpha;		// �㼶͸������
	INT				m_nMapID;			// ��ͼ���
	INT				m_nTrigMapID;		// �����鴫�͵�ͼ�ı��
	INT				m_nSelTriggerid;	// ��ʾ��ǰѡ��Ĵ�����ID��
public:
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCollLerp();
};