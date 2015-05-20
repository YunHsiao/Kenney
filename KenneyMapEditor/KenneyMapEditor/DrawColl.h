#pragma once

extern int g_nWndWidth;
extern int g_nWndHeight;

extern int g_nAppWidth;
extern int g_nAppHeight;

class CDrawColl
{
public:
	explicit CDrawColl(void);
	virtual  ~CDrawColl(void);

public:
	HRESULT OnCreate(LPDIRECT3DDEVICE9 pd3dDevice,
		LPDIRECT3DTEXTURE9 pTexture);
	HRESULT Render(POINT& offset);
	void Update(LONG px, LONG py);
	void Destroy();
	typedef std::list<D3DXVECTOR2*> VecPoints;
	typedef std::list<D3DXVECTOR2*>::iterator VecPointsIt;

public:     // 内联函数
	inline BOOL empty() { return m_vColl.empty(); }
	inline BOOL GetActive(){ return m_bIsActive; }
	inline VecPoints* const GetColls(){ return &m_vColl; }
	inline INT  GetMapCellWidth(){ return m_nCellWidth; }
	inline INT  GetMapCellHeight(){ return m_nCellHeight; }
	inline DWORD GetCollsCount() { return (DWORD) m_vColl.size(); }
	inline void SetTextureColor(D3DCOLOR color){ m_dwMapColor = color; }
	inline void SetMapUpdateNum(INT num){ m_nNum = num; }
	inline void SetActive(BOOL bIsActive = TRUE){ m_bIsActive=bIsActive; }
	inline void SetMapColor(D3DCOLOR color){ m_dwMapColor = color; }
	inline void InsertColl(D3DXVECTOR2* coll) { m_vColl.push_back(coll); }
	inline void ResetInsertIt() { m_cIt = m_vColl.begin(); }
	inline void ToggleLerpRendering() { m_bLerp = !m_bLerp; }
	inline void SafeDeleteFront() { 
		SAFE_DELETE(m_vColl.front())
		m_vColl.pop_front(); 
		ResetInsertIt();
	}
	inline void ClearColls() { 
		for (VecPointsIt it(m_vColl.begin()); it != m_vColl.end(); ++it)
			SAFE_DELETE(*it)
		m_vColl.clear();
		ResetInsertIt();
	}

private:
	INT					m_nRowSelect;	//被选中的行
	INT					m_nColSelect;	//被选中的列
	INT					m_nCellWidth;
	INT					m_nCellHeight;
	INT					m_nNum;
	D3DCOLOR				m_dwLerpColor;	// 插值部分的颜色和Alpha透明通道
	D3DCOLOR				m_dwMapColor;	// 地图的颜色和Alpha透明通道
	BOOL					m_bIsActive;	// 地图是否是激活状态
	BOOL					m_bLerp;		// 碰撞块是否采用插值绘制
	VecPoints				m_vColl;		// 碰撞块坐标列表
	VecPointsIt				m_cIt;			// 插入列表位置
	LPDIRECT3DTEXTURE9		m_pTexture; 
	LPD3DXSPRITE			m_pSprite;
};