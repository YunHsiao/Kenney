#ifndef KENNEY_MONSTERBARNACLE
#define KENNEY_MONSTERBARNACLE

#include "Monster.h"
// ≥»Àª®
class CMonsterBarnacle : public CMonster
{
public:
	CMonsterBarnacle(std::vector<RECT> pRole, D3DXVECTOR3 vecPos, 
		LPD3DXLINE pLine, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex,
		unsigned int **tiles,CPlayer* player,short id);
	~CMonsterBarnacle(void);
public:
	void Destroy();
	bool TickFSM(float fElapsedTime);
};
#endif