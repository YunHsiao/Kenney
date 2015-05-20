#include "stdafx.h"
#include "Actor.h"

CActor::CActor(void):
m_eActorState(EAS_Alive),
m_bMirror(false),
m_fCurMoveElasped(0.f),
m_fMoveSpeed(0.f),m_fJumpHeight(0.f)
{
	D3DXMatrixIdentity(&m_matMirror);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matRotate);
	D3DXMatrixIdentity(&m_matTranslate);
	D3DXMatrixIdentity(&m_matWorld);

	ZeroMemory(&m_srActor, sizeof(RECT));
	ZeroMemory(&m_eActorType, sizeof(EActorType));

	ZeroMemory(&m_vecDirection, sizeof(D3DXVECTOR3));
	ZeroMemory(&m_vecPosition, sizeof(D3DXVECTOR3));
}

CActor::~CActor(void)
{

}

BOOL CActor::Initialize(EActorType eActType, float fMoveSpeed, float fJumpHeight, D3DXVECTOR3 vecPos, bool bMirror)
{
	m_eActorType = eActType;
	m_fMoveSpeed = fMoveSpeed;
	m_fJumpHeight = fJumpHeight;
	m_vecPosition = vecPos;

	return TRUE;
}
