#include "StdAfx.h"
#include "Controller.h"
#include "Player.h"

CController CController::sm_Controller;

CController::CController(void):
m_pPlayer(NULL)
{

}

CController::~CController(void)
{

}

BOOL CController::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	if(NULL == m_pPlayer)
		return FALSE;
	
	return TRUE;
}

void CController::Tick(float fElaspedTime)
{
	if(NULL == m_pPlayer)
		return false;

	bool bLeft = ::GetAsyncKeyState('A') & 0x8000;
	bool bRight = ::GetAsyncKeyState('D') & 0x8000;
	bool bEnter = ::GetAsyncKeyState('W') & 0x8000;
	bool bDuck = ::GetAsyncKeyState('S') & 0x8000;
	bool bJump = ::GetAsyncKeyState(VK_SPACE) & 0x8000;

	EPlayerState eState = EPS_Stand;

}

void CController::TickKeyboard(float fElaspedTime)
{
	// TODO: 响应角色动作
}

void CController::TickMouse(float fElaspedTime)
{
	// TODO: 响应点击NPC等	
}