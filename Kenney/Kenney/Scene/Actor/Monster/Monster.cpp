#include "StdAfx.h"
#include "Monster.h"
#include "Scene.h"
#include "Task.h"
#include "TaskManager.h"

extern bool g_bDebug;

CMonster::CMonster(std::vector<RECT> pRole, D3DXVECTOR3 vecPos, 
				   LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex, LPD3DXLINE pLine):
CActor(pRole,vecPos,pSprite,pTex, pLine)
,m_eMonsterFrame(EMA_Move)
,m_fElapsedTime(0.f)
,m_fMirrorTime(3.f)
,m_fRestTime(3.f)
,m_bStopFrames(false)
,m_bTimeDetection(true)
,m_bDeathTally(true)
{
	m_eCurState = EAS_Duck;
	ZeroMemory(&m_rect,sizeof(RECT));
	ZeroMemory(&m_intersect,sizeof(RECT));
}

CMonster::~CMonster(void)
{
	Destroy();
}

void CMonster::Destroy()
{
	m_pPlayer = NULL;
}

void CMonster::UpdataFrame()
{
	switch(m_eCurState){
		case EAS_Stand:
			m_eMonsterFrame = EMA_Move;
			break;
		case EAS_Duck:
			m_eMonsterFrame = m_eMonsterFrame?EMA_Move:EMA_Move2;
			break;
		case EAS_Hurt:
			m_eMonsterFrame = EMA_Hurt;
			break;
			break;
		case EAS_Death:
			m_eMonsterFrame = EMA_Die;
			break;
	}
	if(m_eCurState & EAS_Death){
		if(m_bDeathTally){
			CTask::GetInstance()->AddTallyMonstrSumToMap(m_iID);
			CTaskManager::GetInstance()->UpdataKillMonster(0.f);
			m_bDeathTally = false;
		}
	}else{
		m_srActor = m_vecRole[m_eMonsterFrame];
	}
}

bool CMonster::TickFSM(float fElapsedTime)
{
	switch(m_eCurState)
	{
	case EAS_Death:
		m_fElapsedTime += fElapsedTime;
		if(m_fElapsedTime > 1.5f)
			return true;
		break;
	case EAS_Hurt:
		//受伤的状态不一样，所以分开到每个类中去写了
		break;
	}
	return false;
}

void CMonster::Death()
{	
	SetActorState(EAS_Death);
	D3DXMatrixScaling(&m_matScale, 1.f, .1f, 1.f);
	m_vecVel.x = 0.f;
	m_vecPos.y += BLOCKLENGTH-6;
	m_fElapsedTime = 0.f;
}

bool CMonster::DetectionCollision(RECT intersect, const RECT MonsterRect,const RECT* PlayerRect)
{
	if (m_pPlayer->Alive() && IntersectRect(&intersect, &MonsterRect, PlayerRect)) {
		if(g_bDebug){
			Death();
			return false;
		} else {
			unsigned long iWidth = intersect.right-intersect.left;
			unsigned long iHeight = intersect.bottom-intersect.top;
			if (m_pPlayer->IsFalling() && iWidth > iHeight) {
				Death(); m_pPlayer->Bounce();
			} else {
				m_pPlayer->Die();
			}
			return false;
		}
	}
	return true;
}

bool CMonster::IntersectRect(RECT* its, const RECT* mst, const RECT* ply) {
	if (!::IntersectRect(its, mst, ply)) return false;

	long pTop = ply->top - its->top, pLeft = ply->left - its->left,
		mTop = mst->top - its->top, mLeft = mst->left - its->left; 
	int *pMst = m_vecPxl[m_eMonsterFrame], *pPly = m_pPlayer->GetPxl();

	int m, n, w(its->right-its->left), h(its->bottom-its->top);
	for (m = 0; m < h; ++m) for (n = 0; n < w; ++n) {
		if ((pMst[(m-mTop)*BLOCKLENGTH+(n-mLeft)] & 0xFF000000) &&
			(pPly[(m-pTop)*BLOCKLENGTH+(n-pLeft)] & 0xFF000000)) {
				return true;
		}
	}
	return false;
}