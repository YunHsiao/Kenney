#ifndef KENNEY_MONSTERFISH
#define KENNEY_MONSTERFISH

#include "Monster.h"

class CMonsterFish : public CMonster
{
public:
	CMonsterFish(std::vector<RECT> pRole, D3DXVECTOR3 vecPos,
		LPD3DXLINE pLine, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex,
		unsigned int **tiles,CPlayer* player,short id);
	~CMonsterFish(void);
public:
	void Destroy();
	bool TickFSM(float fElapsedTime);
};
#endif
