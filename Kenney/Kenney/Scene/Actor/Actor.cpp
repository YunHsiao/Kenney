#include "stdafx.h"
#include "Actor.h"

extern bool g_bDebug;

CActor::CActor(std::vector<RECT> vecRole, D3DXVECTOR3 vecPos, LPD3DXSPRITE pSprite, 
			   LPDIRECT3DTEXTURE9 pTex, LPD3DXLINE pLine):
m_eCurState(EAS_Stand),
m_fJumpHeight(20.f),
m_fDefSpe(.01f),
m_fDefAcc(.001f),
m_vecPos(vecPos),
m_bMirror(false),
m_bNPC(false),
m_fDefElapse(1.f/4),
m_fElapsedTime(0.f),
m_vecRole(vecRole),
m_pSprite(pSprite),
m_pLine(pLine),
m_pTex(pTex),
m_tiles(NULL),
m_fGravity(0.f),
m_fGravityWater(0.f),
m_iID(0)
{
	m_iRow = static_cast<unsigned int>(m_vecPos.y) / BLOCKLENGTH;
	m_iCol = static_cast<unsigned int>(m_vecPos.x) / BLOCKLENGTH;
	m_vecSize.x = static_cast<float>(m_vecRole[0].right-m_vecRole[0].left);
	m_vecSize.y = static_cast<float>(m_vecRole[0].bottom-m_vecRole[0].top);
	m_vecSize.z = 0.f;

	D3DXMatrixIdentity(&m_matMirror);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matRotate);
	D3DXMatrixIdentity(&m_matTranslate);
	D3DXMatrixIdentity(&m_matWorld);
	UpdateGrid();

	ZeroMemory(&m_srActor, sizeof(RECT));

	ZeroMemory(&m_vecVel, sizeof(D3DXVECTOR3));
}

CActor::~CActor(void)
{
	if(NULL != m_pSprite)
		m_pSprite = NULL;

	if(NULL != m_pLine)
		m_pLine = NULL;

	if(NULL != m_pTex)
		m_pTex = NULL;
}

//bool CActor::Initialize(float fMoveSpeed, float fJumpHeight, D3DXVECTOR3 vecPos, bool bMirror)
//{
//	m_fMoveSpeed = fMoveSpeed;
//	m_fJumpHeight = fJumpHeight;
//	m_vecPosition = vecPos;
//
//	return true;
//}
	
bool CActor::Tick(float fElapsedTime)
{
	bool bResult = TickFSM(fElapsedTime);
	m_fElapsedTime += fElapsedTime;
	if (m_fElapsedTime >= m_fDefElapse) {
		m_fElapsedTime = 0.f;
		UpdataFrame();
	}
	if (m_bMirror) {
		m_matMirror._11 = -1.f; 
		m_matMirror._41 = m_vecSize.x;
	} else {
		m_matMirror._11 = 1.f;
		m_matMirror._41 = 0.f;
	}
	D3DXMatrixTranslation(&m_matTranslate, m_vecPos.x, m_vecPos.y, 0.5f);
	m_matWorld = m_matMirror * m_matRotate * m_matScale * m_matTranslate;
	return bResult;
}

void CActor::Render(D3DXMATRIX matOffset)
{
	bool bDied((m_eCurState&(EAS_Death|EAS_Hurt)) > 0);
	m_pSprite->SetTransform(&(m_matWorld*matOffset));
	m_pSprite->Draw(m_pTex, &m_srActor, NULL, NULL, bDied ? 
		D3DCOLOR_ARGB(200, 255, 200, 200) : D3DCOLOR_ARGB(255, 255, 255, 255));
	if (g_bDebug) RenderDebug(matOffset);
}

void CActor::RenderDebug(D3DXMATRIX matOffset) {}