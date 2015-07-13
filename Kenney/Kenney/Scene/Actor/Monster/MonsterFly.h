#ifndef KENNEY_MONSTERFLY
#define KENNEY_MONSTERFLY

#include "Monster/Monster.h"
//»á·ÉµÄ

class CMonsterFly : public CMonster
{
public:
	CMonsterFly(std::vector<RECT> pRole, D3DXVECTOR3 vecPos,
		LPD3DXLINE pLine, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex,
		unsigned int **tiles,CPlayer* player,short id);
	~CMonsterFly(void);
public:
	void Destroy();
	void Death();
	bool TickFSM(float fElapsedTime);
};
#endif
