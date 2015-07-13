#include "StdAfx.h"
#include "MonsterGroundBiont.h"
#include "Scene.h"
#include "ResourceManager.h"

CMonsterGroundBiont::CMonsterGroundBiont
(std::vector<RECT> pRole, D3DXVECTOR3 vecPos
 ,LPD3DXLINE pLine,LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex
 ,unsigned int **tiles,CPlayer* player,short id)
 :CMonster(pRole,vecPos,pSprite,pTex,pLine)
 ,m_sLifeCount(0)
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
	m_fEvaluation = m_fDefSpe;
	m_vecVel.x = -1.f; m_vecVel.y = m_vecVel.z = 0.f;
	D3DXVec3Normalize(&m_vecVel, &m_vecVel);
}

CMonsterGroundBiont::~CMonsterGroundBiont(void)
{
	Destroy();
}
void CMonsterGroundBiont::Destroy()
{
	CMonster::Destroy();
	m_tiles = NULL;

}
bool CMonsterGroundBiont::TickFSM(float fElapseTime)
{
	UpdateGrid();
	bool bColl(false);
	m_vecPos += m_vecVel * m_fDefSpe * fElapseTime;
	switch(m_iID)
	{
	case EMT_Frog:
	case EMT_Ladybug:
	case EMT_Mouse:
	case EMT_SlimeBlock:
	case EMT_SlimeBlue:
	case EMT_SlimeGreen:
	case EMT_SlimePurple:
	case EMT_WormGreen:
	case EMT_WormPink:
	case EMT_Snail:
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
	}
		return CMonster::TickFSM(fElapseTime);
}
	//case EMT_Snail:
	//	switch(m_eCurState)
	//	{
	//	case EAS_Stand:
	//	case EAS_Duck:
	//		if(m_pPlayer->Alive() && IntersectRect(&m_intersect,&m_rect,m_pPlayer->GetRect())){
	//			SetActorState(EAS_Hurt);
	//			//SetMonsterFrame(EMA_Hurt);
	//			m_fDefSpe = 0.f;
	//		}
	//		if (m_vecPos.x < 0 || m_vecPos.y < 0 || 
	//			m_tiles[m_iRow][m_iCol] > EBT_BlockMin||
	//			m_tiles[m_iRowFoot][m_iCol] < EBT_BlockMin){
	//				if(m_pPlayer->Alive() && IntersectRect(&m_intersect,&m_rect,m_pPlayer->GetRect())){
	//					SetActorState(EAS_Hurt);
	//					break;
	//				}
	//				Mirror(fElapseTime);
	//		}
	//		break;

	//	case EAS_Hurt:
	//		//if(m_sLifeCount > 2){
	//		//	SetMonsterFrame(EMA_Die);
	//		//}
	//		//m_fElapsedTime += fElapseTime;
	//		//if(m_fElapsedTime > 10.f){
	//		//	SetMonsterFrame(EMA_Move);
	//		//	m_fDefSpe = m_fEvaluation;
	//		//}
	//		if(m_pPlayer->Alive() && IntersectRect(&m_intersect,&m_rect,m_pPlayer->GetRect())){
	//			//m_sLifeCount++;
	//			//D3DXVECTOR3 PlayerPos;
	//			//PlayerPos = m_pPlayer->GetPosition();
	//			if(m_pPlayer->GetPosition().x > (m_rect.left + m_rect.right) * 0.5f){
	//				m_vecVel.x = -1.f;
	//				m_fDefSpe = 300.f;
	//				m_bMirror = false;
	//			}else{
	//				m_vecVel.x = 1.f;
	//				m_fDefSpe = 300.f;
	//				m_bMirror = true;
	//			}
	//		}
	//		if (m_vecPos.x < 0 || m_vecPos.y < 0 || 
	//			m_tiles[m_iRow][m_iCol] > EBT_BlockMin||
	//			m_tiles[m_iRowFoot][m_iCol] < EBT_BlockMin){
	//				m_vecPos -= m_vecVel * m_fDefSpe * fElapseTime;
	//				m_vecVel = -m_vecVel;
	//				m_bMirror = !m_bMirror;
	//		}
	//		break;
	//	case EAS_Death:
	//		break;
	//	}
	//}
	//return CMonster::TickFSM(fElapseTime);
//}