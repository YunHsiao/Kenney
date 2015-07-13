#include "StdAfx.h"
#include "NPC.h"

CNPC::CNPC(int eNPCType, D3DXVECTOR3 vecPos, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex, 
		   CPlayer* pPlayer, unsigned int** ppTiles, LPD3DXLINE pLine):
CActor(CResourceManager::GetIntance()->GetResourceRange(eNPCType), vecPos, pSprite, pTex, pLine),
m_fIdleTime(3.f), m_fMoveTime(5.f),
m_bFrame(false), m_pPlayer(pPlayer)/*, m_fElapsedTime(0.f)*/
{
	unsigned int t;
	rand_s(&t);
	m_fIdleElapsed = static_cast<float>(t%5);
	rand_s(&t);
	m_fMoveElapsed = static_cast<float>(t%6);
	m_tiles = ppTiles;
	m_eCurState = EAS_Stand;
	m_fDefAcc = 200.f;
	m_fDefSpe = 50.f;
	m_bNPC = true;
	m_iID = eNPCType;
	ZeroMemory(&m_vecAcc, sizeof(D3DXVECTOR3));
}

CNPC::~CNPC(void)
{
	Destroy();
}

void CNPC::Destroy()
{
	if(NULL != m_pPlayer)
		m_pPlayer = NULL;
}

bool CNPC::TickFSM(float fElapsedTime)
{
	UpdateGrid();
	if(0 == CheckPlayerRange())
	{
		//Player不在范围

		if(m_fIdleTime > m_fIdleElapsed)
			Idle(fElapsedTime);
		else if(m_fMoveTime > m_fMoveElapsed)
		{
			//m_fElapsedTime += fElapsedTime;
			//if(m_fElapsedTime > rand() % 3)
			//{
				Move(fElapsedTime);
				//m_fElapsedTime = 0.f;
			//}
		}
	}
	else
	{
		//Player进入范围
		Stand();
	}

	return false;
}

int CNPC::CheckPlayerRange()
{
	D3DXVECTOR3 vecDelta = m_vecPos - m_pPlayer->GetPosition();
	if(NPC_ActiveDistance > abs(vecDelta.x) && BLOCKLENGTH/10 > abs(vecDelta.y))
	{
		if(0 < vecDelta.x)
		{
			SetMirror(true);
			return 1;
		}
		else if (0 > vecDelta.x)
		{
			SetMirror(false);
			return -1;
		}
	}

	return 0;
}

void CNPC::Idle(float fElapsedTime)
{
	//TODO: None状态下待机
	if(!(EAS_Idle & m_eCurState))
		SetActorState(EAS_Idle);
	m_fIdleElapsed += fElapsedTime;

	if(m_fIdleTime < m_fIdleElapsed)
	{
		m_fMoveElapsed = 0.f;
		m_fMoveTime += (pow(-1.f, rand()%2) * (rand() % 10) * 0.1f);
		if(m_fMoveTime > 5.f)
			m_fMoveTime = 5.f;
		else if(m_fMoveTime < 0.f)
			m_fMoveTime = 1.f;
	}
}

void CNPC::Move(float fElapsedTime)
{
	//TODO: Walk状态下NPC移动
	if(!(EAS_Walk & m_eCurState))
	{
		unsigned int t; rand_s(&t);
		m_bMirror = t%2==0;
	}

	if(DetectCollision(fElapsedTime))
		m_bMirror = !m_bMirror;

	if(abs(m_vecVel.x) < m_fDefSpe)
	{	
		m_vecAcc.x = m_fDefAcc;
	}
	else
	{
		m_vecAcc.x = 0.f;
		m_vecVel.x = m_fDefSpe;
	}

	m_vecVel += m_vecAcc * fElapsedTime;
	m_vecPos += m_vecVel * fElapsedTime * (m_bMirror ? -1.f : 1.f);

	SetActorState(EAS_Walk);
	m_fMoveElapsed += fElapsedTime;

	if(m_fMoveTime < m_fMoveElapsed)
	{
		m_fIdleElapsed = 0.f;
		m_fIdleTime += (pow(-1.f, rand()%2) * (rand() % 10) * 0.1f);
		if(m_fIdleTime > 3.f)
			m_fIdleTime = 3.f;
		else if(m_fIdleTime < 0.f)
			m_fIdleTime = 0.5f;
	}
}

bool CNPC::DetectCollision(float fElapsedTime)
{
	if(m_vecPos.x < 0 || m_vecPos.y < 0)
	{
		return true;
	}
	else if(m_bMirror)
	{
		if(m_tiles[m_iRow][m_iCol] > EBT_BlockMin
		|| m_tiles[m_iRow +1][m_iCol] > EBT_BlockMin
		|| m_tiles[m_iRow +2][m_iCol] < EBT_BlockMin)
			return true;
	}
	else if(!m_bMirror)
	{
		if(	m_tiles[m_iRow][m_iCol] > EBT_BlockMin
		|| m_tiles[m_iRow +1][m_iCol +1] > EBT_BlockMin
		|| m_tiles[m_iRow +2][m_iCol +1] < EBT_BlockMin)
			return true;
	}

	return false;
}

void CNPC::Stand()
{
	ZeroMemory(&m_vecVel, sizeof(D3DXVECTOR3));
	SetActorState(EAS_Stand);
}

void CNPC::UpdataFrame()
{
	//从m_vecRole中取帧给 m_srActor
	m_bFrame = !m_bFrame;
	if(EAS_Idle & m_eCurState)
	{
		m_srActor = m_vecRole[ENA_Front];
	}
	else if(EAS_Stand & m_eCurState)
	{
		m_srActor = m_vecRole[ENA_Stand];
	}
	else if(EAS_Walk & m_eCurState)
	{
		m_srActor = m_vecRole[ENA_Walk1 + m_bFrame];
	}
}
