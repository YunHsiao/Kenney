#include "stdafx.h"
#include "Player.h"
#include "Controller.h"

CPlayer::CPlayer(void):
m_eRoleType(ERT_Beige),
m_eCurState(EPS_Stand),m_eCurAction(EPA_Stand)
{

}

CPlayer::~CPlayer(void)
{

}

BOOL CPlayer::Initialize(ERoleType eRoleType, D3DXVECTOR3 vecPos)
{
	//TODO: 根据角色类型初始化特性数据，非特性数据一起处理

	float fMoveSpeed = 0.f;
	float fJumpHeight = 0.f;

	switch(eRoleType)
	{
	case ERT_Beige:
		fMoveSpeed = 200.f;
		fJumpHeight = 50.f;
		m_iHP = m_iMaxHP = 6;

		break;
	case  ERT_Blue:
		fMoveSpeed = 250.f;
		fJumpHeight = 50.f;
		m_iHP = m_iMaxHP = 6;
		break;
	case  ERT_Green:
		fMoveSpeed = 200.f;
		fJumpHeight = 60.f;
		m_iHP = m_iMaxHP = 6;
		break;
	case ERT_Pink:
		fMoveSpeed = 100.f;
		fJumpHeight = 25.f;
		m_iHP = m_iMaxHP = 8;
		break;
	case ERT_Yellow:
		fMoveSpeed = 300.f;
		fJumpHeight = 60.f;
		m_iHP = m_iMaxHP = 2;
		break;
	}

	if(FAILED(CActor::Initialize(EAT_Player, fMoveSpeed, fJumpHeight, vecPos)))
		return FALSE;

	if(FAILED(CController::GetInstance()->Initialize(this)))
		return FALSE;

	return TRUE;
}

void CPlayer::Tick(float fElaspedTime)
{

}

void CPlayer::Render(ID3DXSprite* pSprite, IDirect3DTexture9* pTex)
{
	if(NULL == pSprite || NULL == pTex)
		return;


}


