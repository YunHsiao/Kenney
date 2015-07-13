#ifndef KENNEY_MONSTERGROUNDBIONT
#define KENNEY_MONSTERGROUNDBIONT

#include "Monster.h"

class CMonsterGroundBiont : public CMonster
{
public:
	CMonsterGroundBiont(std::vector<RECT> pRole, D3DXVECTOR3 vecPos, 
		LPD3DXLINE pLine, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex,
		unsigned int **tiles,CPlayer* player,short id);
	~CMonsterGroundBiont(void);
public:
	void Destroy();
	bool TickFSM(float fElapsedTime);
private:
	short m_sLifeCount;
};
#endif