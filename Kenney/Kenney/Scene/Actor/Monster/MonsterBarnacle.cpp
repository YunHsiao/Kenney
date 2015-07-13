#include "StdAfx.h"
#include "MonsterBarnacle.h"
#include "Scene.h"
#include "ResourceManager.h"

CMonsterBarnacle::CMonsterBarnacle
(std::vector<RECT> pRole, D3DXVECTOR3 vecPos
 ,LPD3DXLINE pLine,LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex
 ,unsigned int **tiles,CPlayer* player,short id)
 :CMonster(pRole,vecPos,pSprite,pTex,pLine)
{
	m_iID = id;
	m_vecPxl = CResourceManager::GetIntance()->GetPixelsRange(m_iID);
	SMonsterParameter mp = CScene::GetInstance()->GetParameter(id);
	m_fGravityWater = mp.fGravityWater;
	m_fGravity		= mp.fGravity;
	m_fJumpHeight	= mp.fJumpHeight;
	m_fDefSpe		= mp.fDefSpe;
	m_fDefElapse	= mp.fDefElapse;
	m_fMirrorTime	= mp.fMirrorTime;
	m_fRestTime		= mp.fRestTime;

	m_bStopFrames = false;
	m_bTimeDetection = true;
	m_pPlayer = player;
	m_tiles = tiles;
}

CMonsterBarnacle::~CMonsterBarnacle(void)
{
	Destroy();
}

void CMonsterBarnacle::Destroy()
{
	CMonster::Destroy();
	m_tiles = NULL;

}
bool CMonsterBarnacle::TickFSM(float fElapseTime)
{
	UpdateGrid();
	if(m_pPlayer->Alive() && IntersectRect(&m_intersect,&m_rect,m_pPlayer->GetRect()))
		m_pPlayer->Die();

	return CMonster::TickFSM(fElapseTime);
}