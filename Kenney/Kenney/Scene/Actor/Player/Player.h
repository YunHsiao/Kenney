#ifndef KENNEY_PLAYER
#define KENNEY_PLAYER
#include "Actor.h"
#include "PlayerDefine.h"
#include "ResourceManager.h"
#include "Bag.h"
#include "ItemDefine.h"

class CPlayer : public CActor 
{
public:
	CPlayer(ERoleType eRoleType, D3DXVECTOR3 vecPos, 
		LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex, LPD3DXLINE pLine);
	~CPlayer(void);

public:
	void SetRole(unsigned int eRole, bool bFillHP = true);
	inline unsigned int GetRole() { return m_iRole - ERT_Beige + Beige; }

public:
	inline CBag *GetBag() { return &m_Bag; }
	inline int GetHP() { return m_iHP; }
	inline int GetMaxHP() { return m_iMaxHP; }
	inline int* GetPxl() { return m_pxl; }
	inline void FillHP() { m_iHP = m_iMaxHP; }
	inline void CutScene(D3DXVECTOR3 vecPos, unsigned int** tiles, float fMaxRol) { 
		m_eCurState = EAS_Stand;
		m_vecPos = vecPos; 
		m_vecVel = D3DXVECTOR3(0.f, 0.f, 0.f);
		m_tiles = tiles; 
		UpdateGrid();
		m_fMaxCol = fMaxRol - m_vecSize.x;
	}
	inline void IsUpPressed(bool bUp) { m_bUp = bUp; }
	inline const RECT* GetRect() const { return &m_rect; }
	inline bool IsFalling() { return m_vecVel.y > 0.f; }
	inline void Bounce() {
		m_vecVel.y = -m_fDefSpe; 
		m_vecAcc.y = m_fGravity;
	}
	inline bool Alive() { return (m_eCurState&(EAS_Death|EAS_Hurt)) == 0; }
	inline void SetRoleByHeadId(unsigned int iHead, bool bFillHP = true) { SetRole(iHead-Beige+ERT_Beige, bFillHP); }
	inline void Die() { 
		if (g_bDebug) return;
		if (AddActorState(EAS_Hurt)) {
			--m_iHP; 
			if (m_iHP <= 0) {
				AddActorState(EAS_Death);
				m_iHP = 0;
			}
			Bounce();
			m_vecVel.x = m_fDefSpe*(m_bMirror?1.f:-1.f); m_vecAcc.x = 0.f;
		}
	}

protected:
	void RenderDebug(D3DXMATRIX matOffset);
	unsigned int DetectPixelColl(short sColl, unsigned int id = 0);
	void DetectCollision(float fElapsedTime, bool bDuck);
	void LoadRoles();
	std::vector<RECT> CalcRect(std::vector<int*> vecPxl, std::vector<RECT> vecRole);
	void UpdataFrame();
	void Destroy();
	bool TickFSM(float fElapsedTime);
	short GetColl();
	bool PrizesColl(bool bRight);
	bool PrizeCollProceed(int iRow, int iCol, int eColl);
	void SetFrame(unsigned int ePlayerAction);
	bool IsHill(unsigned int id);
	void UpdateGridY() { 
		m_rect.top = static_cast<long>(m_vecPos.y) + m_coll.top;
		m_rect.bottom = static_cast<long>(m_vecPos.y) + m_coll.bottom;
		m_iRow = m_rect.top / BLOCKLENGTH;
		m_iRowFoot = m_rect.bottom / BLOCKLENGTH;
	}
	void UpdateGridX() { 
		m_rect.left = static_cast<long>(m_vecPos.x) + m_coll.left;
		m_rect.right = static_cast<long>(m_vecPos.x) + m_coll.right;
		m_iCol = m_rect.left / BLOCKLENGTH;
		m_iColMid = (m_rect.left+m_rect.right) / 2 / BLOCKLENGTH;
		m_iColRight = m_rect.right / BLOCKLENGTH;
	}	
	inline bool In3Grid() { return (m_iRowFoot - m_iRow) == 2; }
	inline unsigned int Coll(unsigned int iRow, unsigned int iCol) {
		if (iRow >= MAX_ROW) iRow = MAX_ROW - 1;
		if (iCol >= m_fMaxCol/BLOCKLENGTH) iCol = (int)m_fMaxCol / BLOCKLENGTH;
		return m_tiles[iRow][iCol];
	}

private:
	CBag m_Bag;

	float m_fMaxCol, m_fPxlUp;
	unsigned int m_iRowFoot, m_iColMid, m_iColRight, m_iRole;
	ERoleType m_eRoleType;

	int m_iHP, m_iMaxHP, m_iShift;
	D3DXVECTOR3 m_vecAcc;
	std::vector<RECT> m_vecColl;
	std::vector<int*> m_vecPxl;
	RECT m_rect, m_coll;
	int *m_pxl;
	std::map<unsigned int, SPlayerParameter> m_mapParam;
	bool m_bUp, m_bFirstFrame, m_bJump;
};

#endif