#ifndef KENNEY_MONSTERSAW
#define KENNEY_MONSTERSAW

#include "Monster.h"

//¾â³Ý

class CMonsterSaw : public CMonster
{
public:
	CMonsterSaw(std::vector<RECT> pRole, D3DXVECTOR3 vecPos, 
		LPD3DXLINE pLine, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex,
		unsigned int **tiles,CPlayer* player,short id,short sswitch);
	~CMonsterSaw(void);
public:
	void Destroy();
	bool TickFSM(float fElapsedTime);
};
#endif
