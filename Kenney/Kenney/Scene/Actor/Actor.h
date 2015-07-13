#ifndef KENNEY_ACTOR
#define KENNEY_ACTOR
#include "ActorDefine.h"

class CActor 
{
public:
	CActor(std::vector<RECT> vecRole, D3DXVECTOR3 vecPos, LPD3DXSPRITE pSprite, 
		LPDIRECT3DTEXTURE9 pTex, LPD3DXLINE pLine);
	virtual ~CActor(void);

public:
	//普通函数
	bool Tick(float fElapsedTime);
	void Render(D3DXMATRIX matOffset);

public:
	//inline函数s
	inline void SetDefAcc(float fDefAcc) { m_fDefAcc = fDefAcc; }
	inline void SetDefSpe(float fDefSpe) { m_fDefSpe = fDefSpe; }
	inline void SetPosition(D3DXVECTOR3 vecPos){ m_vecPos = vecPos;}
	inline D3DXVECTOR3& GetPosition(){ return m_vecPos; }
	inline D3DXVECTOR3& GetSize(){ return m_vecSize; }
	inline unsigned int GetActorState(){ return m_eCurState; }
	inline void SetActorState(EActorState state){ m_eCurState = state; }
	inline void SetMirror(bool bMirror){ m_bMirror = bMirror; }
	inline bool IsNPC(){ return m_bNPC && (m_eCurState & EAS_Stand); }
	inline int GetID(){ return m_iID; }
	inline virtual void UpdateGridY() { m_iRow = (int) m_vecPos.y / BLOCKLENGTH; }
	inline virtual void UpdateGridX() { m_iCol = (int) m_vecPos.x / BLOCKLENGTH; }
	inline void UpdateGrid() { UpdateGridX(); UpdateGridY(); }
	inline bool SubtractActorState(EActorState state)
	{ 
		bool bState((m_eCurState & state) > 0); 
		if (bState) 
			m_eCurState -= state; 
		return bState; 
	}
	inline bool AddActorState(EActorState state)
	{ 
		bool bState((m_eCurState & state) == 0); 
		if (bState) 
			m_eCurState += state; 
		return bState;
	}

protected:
	//继承函数


protected:
	//纯虚函数
	virtual bool TickFSM(float fElapsedTime) = 0;
	virtual void UpdataFrame() = 0;
	virtual void RenderDebug(D3DXMATRIX matOffset);


protected:
	// 角色自身状态
	unsigned int m_eCurState, m_iRow, m_iCol;
	float m_fDefAcc, m_fDefSpe;	// Default Acceleration and Speed
	float m_fJumpHeight, m_fGravity, m_fGravityWater;

	LPD3DXLINE m_pLine;
	D3DXVECTOR3 m_vecVel;
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecSize;

	// 图像变换与显示相关
	LPD3DXSPRITE m_pSprite;
	LPDIRECT3DTEXTURE9 m_pTex;

	bool m_bMirror, m_bNPC;
	D3DXMATRIX m_matMirror;

	D3DXMATRIX m_matScale;
	D3DXMATRIX m_matRotate;
	D3DXMATRIX m_matTranslate;
	D3DXMATRIX m_matWorld;

	unsigned int **m_tiles;
	unsigned int m_iID;
	float m_fElapsedTime, m_fDefElapse;
	std::vector<RECT> m_vecRole;
	RECT m_srActor;
};

#endif