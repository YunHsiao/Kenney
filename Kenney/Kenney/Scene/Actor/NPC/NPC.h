#ifndef KENNEY_NPC
#define KENNEY_NPC

#include "NPCDefine.h"
#include "Scene.h"
#include "Actor.h"
#include "Player.h"
#include "ResourceManager.h"

class CNPC : public CActor
{
public:
	CNPC(int eNPCType, D3DXVECTOR3 vecPos, LPD3DXSPRITE pSprite, 
		LPDIRECT3DTEXTURE9 pTex, CPlayer* pPlayer, unsigned int** ppTiles, LPD3DXLINE pLine);
	virtual ~CNPC(void);

public:
	

private:
	void Destroy();
	int CheckPlayerRange();
	void Idle(float fElapsedTime);
	void Move(float fElapsedTime);
	void Stand();

public:
	virtual bool TickFSM(float fElapsedTime);
	virtual void UpdataFrame();

public:
	bool DetectCollision(float fElapsedTime);

private:
	//float m_fElapsedTime;
	ENPCType m_eNPCType;
	bool m_bFrame;

	D3DXVECTOR3 m_vecAcc;

	float m_fIdleTime;
	float m_fIdleElapsed;

	float m_fMoveTime;
	float m_fMoveElapsed;

	CPlayer* m_pPlayer;
};

#endif
