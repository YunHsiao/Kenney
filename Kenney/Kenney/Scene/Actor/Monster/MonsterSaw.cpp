#include "StdAfx.h"
#include "MonsterSaw.h"
#include "ResourceManager.h"

CMonsterSaw::CMonsterSaw
(std::vector<RECT> pRole, D3DXVECTOR3 vecPos
 ,LPD3DXLINE pLine,LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex
 ,unsigned int **tiles,CPlayer* player,short id,short sswitch)
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
	m_fEvaluation = m_fDefSpe;

	m_fElapsedTime = 0.f;
	if(m_iID == EMT_SawHalf){
		m_vecVel.x = -1.f; m_vecVel.y = m_vecVel.z = 0.f;
		D3DXVec3Normalize(&m_vecVel, &m_vecVel);}

	if(m_iID == EMT_Saw){
		if(sswitch == 1){
			m_vecVel.x = -1.f; m_vecVel.y = m_vecVel.z = 0.f;
			D3DXVec3Normalize(&m_vecVel, &m_vecVel);
		}else{
			m_vecVel.y = 1.f; m_vecVel.x = m_vecVel.z = 0.f;
			D3DXVec3Normalize(&m_vecVel, &m_vecVel);
		}
	}
}

CMonsterSaw::~CMonsterSaw(void)
{
	Destroy();
}
void CMonsterSaw::Destroy()
{
	CMonster::Destroy();
	m_tiles = NULL;

}
bool CMonsterSaw::TickFSM(float fElapseTime)
{
	UpdateGrid();
	m_vecPos += m_vecVel * m_fDefSpe * fElapseTime;
	if(m_vecVel.x != 0.f)
	{
			switch(m_eCurState)
			{
			case EAS_Stand:
			case EAS_Duck:
				if(m_pPlayer->Alive() && IntersectRect(&m_intersect,&m_rect,m_pPlayer->GetRect()))
					m_pPlayer->Die();
				if(m_bTimeDetection){
					m_fElapsedTime += fElapseTime;
					if(m_fElapsedTime > m_fRestTime)
						Mirror(fElapseTime);
				}
				if (m_vecPos.x < 0 || m_vecPos.y < 0 || 
					m_tiles[m_iRow][m_iCol] > EBT_BlockMin ||
					m_tiles[m_iRowFoot][m_iCol] < EBT_BlockMin) {
						m_bTimeDetection = false;
						Mirror(fElapseTime);
				}
				break;
			case EAS_Hurt:
			case EAS_Death:
				break;
			}
	}
	if(m_vecVel.y != 0.f)
	{
			switch(m_eCurState)
			{
			case EAS_Stand:
			case EAS_Duck:
				if(m_pPlayer->Alive() && IntersectRect(&m_intersect,&m_rect,m_pPlayer->GetRect()))
					m_pPlayer->Die();
				if(m_bTimeDetection){
					m_fElapsedTime += fElapseTime;
					if(m_fElapsedTime > m_fRestTime)
						Mirror(fElapseTime);
				}
				if(m_vecPos.x < 0 || m_vecPos.y < 0 || 
					m_tiles[m_iRowV][m_iColV] > EBT_BlockMin){
						m_bTimeDetection = false;
						Mirror(fElapseTime);
				}
				break;
			case EAS_Hurt:
			case EAS_Death:
				break;
			}
	}
	return CMonster::TickFSM(fElapseTime);
}