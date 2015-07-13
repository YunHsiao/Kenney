#pragma once

class CDrawBrush
{
public:
	explicit CDrawBrush(void);
	virtual  ~CDrawBrush(void);
public:   // ���е��ú���
	HRESULT OnCreateByPoint(LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DTEXTURE9* ppTexture, INT num, BOOL bIsActive = FALSE, BOOL bNeedScale = TRUE);
	HRESULT Render(LPD3DXLINE pLine,POINT& offset,BOOL zDepth = TRUE);
	void  Destroy();
	void  SetTexturePosition(float x, float y);
public:   // ��������
	inline void  SetTextureNum(INT num){ m_nCurNum = num; }
	inline void  SetTextureColor(D3DCOLOR color){ m_dwColor = color; }
	inline void  SetActive(BOOL bIsActive = TRUE){ m_bIsActive = bIsActive; }
	inline const BOOL GetActive(){ return m_bIsActive; }
	inline float GetPositionX(){ return m_mWor._41; }
	inline float GetPositionY(){ return m_mWor._42; }
	inline INT   GetTextrueNum(){ return m_nCurNum; }
	inline INT   GetTextureMaxNum(){ return m_nMaxNum; }
	inline INT   GetTextureWidth() { return m_pCurWidth[m_nCurNum];  }
	inline INT   GetTextureHeight() { return m_pCurHeight[m_nCurNum]; }
	inline LPDIRECT3DTEXTURE9* GetTexturePoint(){ return m_ppTexture; }
	inline bool IsValid() { return m_nCurNum >= 0 && m_nCurNum < m_nMaxNum; }
	// ���������õ�
	inline void SetSelectState(BOOL bIsSel)
	{
		m_bIsSelect = bIsSel;
	}
	inline void SetTrigMapID(INT id)
	{
		m_nTrigMapID = id;
	}
	inline const BOOL GetSelectState()
	{
		return m_bIsSelect;
	}
	inline const FLOAT GetDepthZ()
	{
		return m_fDepthZ;
	}
	inline const INT GetTrigMapID()
	{
		return m_nTrigMapID;
	}

private:
	INT                                        m_nTrigMapID;       // �����ĵ�ͼID
	LPD3DXSPRITE                               m_pSprite;
	D3DXMATRIXA16                              m_mWor, m_mSca, m_mTra;
	D3DSURFACE_DESC                            m_desc;
	INT                                        m_nMaxNum;
	INT                                        m_nCurNum;
	BOOL                                       m_bIsSelect;        // �ж��Ƿ�Ϊѡ��(ע��:ֻ��������,�����ü̳���)
	BOOL                                       m_bIsActive;        // �ж��Ƿ񼤻�ɻ���
	D3DCOLOR                                   m_dwColor;          // ��ͼ��ɫ��Alphaͨ��
	LPDIRECT3DTEXTURE9*                        m_ppTexture;
	LPINT                                      m_pCurWidth;
	LPINT                                      m_pCurHeight;
	FLOAT                                      m_fDepthZ;          // ���ֵ
	FLOAT                                      m_fScale;
};