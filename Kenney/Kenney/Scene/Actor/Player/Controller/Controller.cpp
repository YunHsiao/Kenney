#include "StdAfx.h"
#include "Controller.h"
#include "Player.h"
#include "Scene.h"

CController CController::sm_Controller;

CController::CController(void):
m_pPlayer(NULL)
{

}

CController::~CController(void)
{
	if(NULL != m_pPlayer)
		m_pPlayer = NULL;
}

bool CController::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	if(NULL == m_pPlayer)
		return false;
	
	return true;
}

void CController::Tick(float fElapsedTime)
{
	if(NULL == m_pPlayer)
		return;

	if(EAS_Death == m_pPlayer->GetActorState())
		return;

	TickKeyboard(fElapsedTime);
	TickMouse(fElapsedTime);
}

void CController::TickKeyboard(float fElapsedTime)
{
	// TODO: 响应角色动作
	short sKeyState(0);
	if((::GetAsyncKeyState('A') | ::GetAsyncKeyState(VK_LEFT)) & 0x8000) sKeyState += EKS_Left;
	if((::GetAsyncKeyState('D') | ::GetAsyncKeyState(VK_RIGHT)) & 0x8000) sKeyState += EKS_Right;
	if((::GetAsyncKeyState('W') | ::GetAsyncKeyState(VK_UP)) & 0x8000) sKeyState += EKS_Up;
	if((::GetAsyncKeyState('S') | ::GetAsyncKeyState(VK_DOWN)) & 0x8000) sKeyState += EKS_Down;
	if(::GetAsyncKeyState(VK_SPACE) & 0x8000) sKeyState += EKS_Jump;
	if ((sKeyState & EKS_ErrH) == EKS_ErrH) sKeyState -= EKS_ErrH;
	if ((sKeyState & EKS_ErrV) == EKS_ErrV) sKeyState -= EKS_ErrV;
	
	m_pPlayer->SubtractActorState(EAS_Walk);
	m_pPlayer->SubtractActorState(EAS_Duck);
	m_pPlayer->AddActorState(EAS_Stand);
	m_pPlayer->SubtractActorState(EAS_Jump);
	m_pPlayer->IsUpPressed(false);
	if(sKeyState) {
		// Toggle comment: 
		if ((sKeyState & EKS_ErrH)/* Toggle move while ducking -->*
				&& !(sKeyState & EKS_ErrV)/**/) {
			m_pPlayer->SubtractActorState(EAS_Stand);
			m_pPlayer->AddActorState(EAS_Walk);
		}
		if(sKeyState & EKS_Left) m_pPlayer->SetMirror(true); 
		if(sKeyState & EKS_Right) m_pPlayer->SetMirror(false);
		if(sKeyState & EKS_Up) m_pPlayer->IsUpPressed(true);
		if(sKeyState & EKS_Down) m_pPlayer->AddActorState(EAS_Duck); 
		else if(sKeyState & EKS_Jump) m_pPlayer->AddActorState(EAS_Jump);
	}
}

void CController::TickMouse(float fElapsedTime)
{
	static float s_fElapsedTime = 0;
	s_fElapsedTime += fElapsedTime;
	if (s_fElapsedTime > .1f) {
		s_fElapsedTime = 0.f;
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(MANGOGetHWND(), &pos);
			CScene::GetInstance()->Click(pos);
		}
	}
}