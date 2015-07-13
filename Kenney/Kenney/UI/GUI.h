#ifndef KENNEY_GUI
#define KENNEY_GUI

enum UIType
{
	ET_HP,
	ET_HEAD,
	ET_JEWEL_BLANK,
	ET_JEWEL,
	ET_GOLD,
	ET_NUMBER
};

enum EJewelType
{
	EJT_Blue	= (1 << 0),
	EJT_Green	= (1 << 1),
	EJT_Red		= (1 << 2),
	EJT_Yellow  = (1 << 3),
	EJI_All		= EJT_Blue | EJT_Green | EJT_Red | EJT_Yellow
};

struct UIData
{
	UIType eType;
	int x,y;
	RECT rect;
	bool visible;
};

class CGUI
{
private:
	CGUI(void);
	~CGUI(void);

public:
	static CGUI* GetInstance() { return &s_GUI; }

public:
	bool Initialize(LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTexture, float fLayerDapth = .1f);
	void Tick(const int iCurHP, const int iMaxHP, const int iGoldNum, 
		const int iJewel, const int iHead, const int iHeadCnt, const int iCurRole);
	void Render();

	inline void SetVisible(bool IsVisible){m_IsVisible = IsVisible;}
	inline void CheckVisible(){if (!m_IsVisible) return;}
	unsigned int ClickOnHead(POINT pos);

private:
	void Destroy();
	bool LoadUIData(const char* sheet);

private:
	LPD3DXSPRITE m_pSprite;
	LPDIRECT3DTEXTURE9 m_Texture;
	LPDIRECT3DTEXTURE9 m_TextureBackground;

	float m_fLayerDapth;
	bool m_IsVisible;

private:
	// UIData处理
	typedef std::multimap<unsigned int, UIData> UIBlockData;
	typedef std::multimap<unsigned int, UIData>::iterator UIBlockDataIt;
	UIBlockData m_mtmapUIData;

	void TickHP(UIBlockDataIt it,const int iMaxHP, const int iCurHP);
	void TickHEAD(UIBlockDataIt it, const int iHead, const int iHeadCnt, const int iCurRole);
	void TickJEWEL(UIBlockDataIt it, const int iJewel);
	bool TickJEWEL_BLANK();
	void TickNUMBER(UIBlockDataIt it,const int iNum);
	
	void ChangeItemCount(const unsigned int itemID, int deltaNumb);

private:
	// 当前玩家数据
	int m_iMaxHP;
	int m_iCurHP;
	int m_iGoldNum;
	int m_progress;
	int m_oldGoldNumsize;
	std::vector<int> vecHead_x;

private:
	static CGUI s_GUI;
};


#endif //KENNEY_GUI