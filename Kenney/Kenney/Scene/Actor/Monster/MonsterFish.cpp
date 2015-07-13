#include "StdAfx.h"
#include "MonsterFish.h"
#include "Scene.h"
#include "ResourceManager.h"

CMonsterFish::CMonsterFish
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

	m_bStopFrames = true;
	m_bTimeDetection = true;
	m_pPlayer = player;
	m_tiles = tiles;
	m_fElapsedTime = 0.f;
	m_fEvaluation = m_fDefSpe;
	m_vecVel.x = -1.f; m_vecVel.y = m_vecVel.z = 0.f;
	D3DXVec3Normalize(&m_vecVel, &m_vecVel);
}

CMonsterFish::~CMonsterFish(void)
{
	Destroy();
}

void CMonsterFish::Destroy()
{
	CMonster::Destroy();
	m_tiles = NULL;
}

bool CMonsterFish::TickFSM(float fElapseTime)
{
	UpdateGrid();
	bool bColl(false);
	m_vecPos += m_vecVel * m_fDefSpe * fElapseTime;
	switch(m_iID)
	{
	case EMT_FishBlue:
	case EMT_FishGreen:
	case EMT_FishPink:
		switch(m_eCurState)
		{
		case EAS_Duck:
		case EAS_Stand:
			DetectionCollision(m_intersect,m_rect,m_pPlayer->GetRect());
			if (m_vecPos.x < 0 || m_vecPos.y < 0 || 
				m_tiles[m_iRow][m_iCol] > EBT_BlockMin||
				m_tiles[m_iRowFoot][m_iCol] < EBT_BlockMin)
					Mirror(fElapseTime);
			break;
		case EAS_Death:
		case EAS_Hurt:
			break;
		}
		break;
	}
	return CMonster::TickFSM(fElapseTime);
}