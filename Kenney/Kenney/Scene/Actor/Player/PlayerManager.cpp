#include "StdAfx.h"
#include "PlayerManager.h"
#include "Player.h"

CPlayerManager CPlayerManager::sm_PlayerManager;

CPlayerManager::CPlayerManager(void):
m_pSprite(NULL),m_pTex(NULL),
m_pPlayer(NULL)
{

}

CPlayerManager::~CPlayerManager(void)
{
	if(NULL != m_pPlayer)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
}

BOOL CPlayerManager::Initialize(ID3DXSprite* pSprite)
{
	if(NULL == pSprite)
		return FALSE;
	else
		m_pSprite = pSprite;

	if(FAILED(InitializeResource()))
		return FALSE;

	return TRUE;
}

BOOL CPlayerManager::InitializeResource()
{
	//TODO: 用map将不同角色组织好其动作Rect，然后用map将不同角色的mapAct指针放好

	return TRUE;
}

BOOL CPlayerManager::InitializePlayer(ERoleType eRoleType, D3DXVECTOR3 vecPos)
{
	//TODO: 初始化的变量还不完善
	m_pPlayer = new CPlayer;

	m_pPlayer->Initialize(eRoleType, vecPos);
	return TRUE;
}

void CPlayerManager::Tick(float fElaspedTime)
{
	m_pPlayer->Tick(fElaspedTime);
}

void CPlayerManager::Render()
{
	if(NULL == m_pSprite)
		return;

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_pPlayer->Render(m_pSprite, m_pTex);	
	m_pSprite->End();
}
