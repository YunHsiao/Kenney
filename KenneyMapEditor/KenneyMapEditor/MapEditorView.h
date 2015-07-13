#pragma once

// 绘制类
#include "DrawBrush.h"
#include "DrawColl.h"

#include "ToolBackDialog.h"
#include "DialogTest.h"

// CMapEditView 视图

class CMapEditView : public CView
{
	DECLARE_DYNCREATE(CMapEditView)

protected:
	CMapEditView();           // 动态创建所使用的受保护的构造函数
	virtual ~CMapEditView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
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
// 提供给MainFrm调用的D3D渲染函数
public:
	HRESULT Update();
	HRESULT Render();
// 消息映射函数
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
	// 询问是否保存地图
	void IsSaveMap();
private:
	// 拾起与放置的相关操作
	void MousePickBrushMap(CPoint point);     
	void MouseDownBrushMap(CPoint point);   

	void MousePickNpcMap(CPoint point);      
	void MouseDownNpcMap(CPoint point);   

	void MousePickMonsterMap(CPoint point);    
	void MouseDownMonsterMap(CPoint point); 

	void MousePickTrigMap(CPoint point);       
	void MouseDownTrigMap(CPoint point);      
	      
	void SaveFileByBin(std::wstring PathName); // 保存文件为二进制
	void SaveFileByTex(std::wstring PathName); // 保存文件为文本方式
	void SaveFileByXml(std::wstring PathName); // 保存文件为文本方式
	void LoadFileByBin(std::wstring PathName); // 读取文件为二进制
	void LoadFileByTex(std::wstring PathName); // 读取文件为文本方式
	void LoadFileByXml(std::wstring PathName); // 读取文件为二进制
	bool LoadCfgFile();   // 读取配置文件做判断
	void SaveCfgFile();   // 以追加方式保存配置文件
	//设置图层与光标的更改操作
	void SetMapCursorNum(LAYOUT_MAP map,LAYOUT_CURSOR cursor,INT num); 
	// 对地图层级的透明度更新
	void SetMapLayoutColor(D3DCOLOR brush,D3DCOLOR npc,D3DCOLOR monster,D3DCOLOR coll,D3DCOLOR trig);   
	// 绘制网格
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
	BOOL			m_bIsNewMap;		// 是否新建地图状态
	CDrawColl*		m_pDrawMapColl;		// 进行碰撞层的绘制
private:
	CDialogTest*	m_pMapTest;
	std::wstring	m_szMapName;		// 地图名字
	CPoint			m_pLastBrush;
	BOOL			m_bIsLBotton;		// 是否进行了鼠标点击(碰撞层绘制用)
	BOOL			m_bIsLine;			// 是否绘制网格
	BOOL			m_bIsRedLine;		// 是否绘制红色边框
	BOOL			m_bIsMapInfo;		// 是否显示地图信息
	BOOL			m_bIsInitResruce;	// 初始化资源一次
	LAYOUT_CURSOR	m_emCursorLayout;	// 鼠标层级状态
	LAYOUT_MAP		m_emMapLayout;		// 地图层级状态
	POINT			m_nMapCoord;		// 当前地图坐标
	POINT			m_nMapPrevCoord;	// 拖前地图坐标
	POINT			m_nCurrentCoord;	// 鼠标当前坐标
	POINT			m_nStartCoord;		// 鼠标起始坐标
	POINT			m_nDefaultCoord;	// 默认偏移坐标
	INT				m_nTriggerNum;		// 触发块的编号
	INT				m_nLayoutAlpha;		// 层级透明调节
	INT				m_nMapID;			// 地图编号
	INT				m_nTrigMapID;		// 触发块传送地图的编号
	INT				m_nSelTriggerid;	// 表示当前选择的触碰块ID。
public:
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCollLerp();
};