#include "stdafx.h"
#include "Player.h"
#include "Controller.h"

//可以在创建一个Player时，直接进行数据的初始化，然后调用基类初始化函数
//
CPlayer::CPlayer(ERoleType eRoleType, D3DXVECTOR3 vecPos, LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTex, LPD3DXLINE pLine):
CActor(CResourceManager::GetIntance()->GetResourceRange(eRoleType), vecPos, pSprite, pTex, pLine),
m_vecAcc(0.f, 0.f, 0.f),
m_eRoleType(eRoleType),
m_bFirstFrame(true),
m_bJump(false),
m_fMaxCol(0.f),
m_fPxlUp(0.f),
m_iShift(0),
m_iRole(ERT_Beige)
{
	m_fGravity = 500.f;
	m_fGravityWater = 200.f;
	ZeroMemory(&m_coll, sizeof(RECT));
	ZeroMemory(&m_Bag, sizeof(CBag));
	UpdateGrid();
	LoadRoles();
	SetRole(m_eRoleType);
	m_Bag.SetPerson(m_eRoleType-ERT_Beige+Beige);
	UpdataFrame();

}

CPlayer::~CPlayer(void)
{
	Destroy();
}

void CPlayer::Destroy()
{
	m_mapParam.clear();
	m_tiles = NULL;
}

void CPlayer::SetRole(unsigned int eRole, bool bFillHP) {
	SPlayerParameter pp = m_mapParam[eRole];
	m_vecRole = pp.vecRole;
	m_vecColl = pp.vecColl;
	m_vecPxl = pp.vecPxl;
	m_fDefAcc = pp.fDefAcc;
	m_fDefSpe = pp.fDefSpe;
	m_fJumpHeight = pp.fJumpHeight;
	m_fGravity = pp.fGravity;
	m_fGravityWater = pp.fGravityWater;
	m_iRole = pp.iRole;
	m_iMaxHP = pp.iMaxHP;
	if (bFillHP) m_iHP = m_iMaxHP;
}

void CPlayer::LoadRoles() {
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("Resource\\Player\\player.xml")) {
		MessageBox(NULL, TEXT("Resource\\Player\\player.xml Not Found!"), 
			NULL, MB_OK);
		return;
	}
	tinyxml2::XMLElement* node = doc.FirstChildElement()->FirstChildElement();
	do {
		SPlayerParameter pp;
		pp.iRole = atoi(node->Attribute("id"));
		pp.iMaxHP = atoi(node->Attribute("hp"));
		pp.fDefAcc = (float) atof(node->Attribute("acc"));
		pp.fDefSpe = (float) atof(node->Attribute("spe"));
		pp.fJumpHeight = (float) atof(node->Attribute("height"));
		pp.fGravity = (float) atof(node->Attribute("g"));
		pp.fGravityWater = (float) atof(node->Attribute("waterg"));
		pp.vecRole = CResourceManager::GetIntance()->GetResourceRange(pp.iRole);
		pp.vecPxl = CResourceManager::GetIntance()->GetPixelsRange(pp.iRole);
		pp.vecColl = CalcRect(pp.vecPxl, pp.vecRole);
		m_mapParam[pp.iRole] = pp;
	} while (node = node->NextSiblingElement());
}

std::vector<RECT> CPlayer::CalcRect(std::vector<int*> vecPxl, std::vector<RECT> vecRole) {
	std::vector<RECT> vecColl;
	for (unsigned int i(0); i < vecPxl.size(); ++i) {
		RECT r = { vecRole[i].right-vecRole[i].left, vecRole[i].bottom-vecRole[i].top, 0, 0 };
		int w(vecRole[i].right-vecRole[i].left), h(vecRole[i].bottom-vecRole[i].top);
		for (int m(0); m < h; ++m) {
			for (int n(0); n < w; ++n) {
				if (vecPxl[i][m*w+n] & 0xFF000000) {
					if (r.left > n) r.left = n;
					if (r.right < n) r.right = n;
					if (r.top > m) r.top = m;
					if (r.bottom < m) r.bottom = m;
				}
			}
		}
		vecColl.push_back(r);
	}
	return vecColl;
}

void CPlayer::UpdataFrame()
{
	m_bFirstFrame = !m_bFirstFrame;
	if (m_eCurState & EAS_Death) SetFrame(EPA_Hurt);
	else if (m_eCurState & EAS_Climb) SetFrame(EPA_Climb1+(m_bFirstFrame?0:1));
	else if (m_eCurState & EAS_Swim) SetFrame(EPA_Swim1+(m_bFirstFrame?0:1));
	else if (m_eCurState & EAS_Duck) SetFrame(EPA_Duck);
	else if (m_eCurState & EAS_Jump) SetFrame(EPA_Jump);
	else if (m_eCurState & EAS_Attack) SetFrame(EPA_Jump);
	else if (m_eCurState & EAS_Fall) SetFrame(EPA_Hurt);
	else if (m_eCurState & EAS_Walk) SetFrame(EPA_Walk1+(m_bFirstFrame?0:1));
	else if (m_eCurState & EAS_Stand) SetFrame(EPA_Stand);
	else SetFrame(EPA_Front);	
}

void CPlayer::SetFrame(unsigned int ePlayerAction) {
	m_srActor = m_vecRole[ePlayerAction];
	m_coll = m_vecColl[ePlayerAction];
	m_pxl = m_vecPxl[ePlayerAction];
}

void CPlayer::RenderDebug(D3DXMATRIX matOffset) {
	D3DXVECTOR2 vecLine[5] = 
	{
		D3DXVECTOR2(m_rect.left + matOffset._41, m_rect.top + matOffset._42),
		D3DXVECTOR2(m_rect.right + matOffset._41, m_rect.top + matOffset._42),
		D3DXVECTOR2(m_rect.right + matOffset._41, m_rect.bottom + matOffset._42),
		D3DXVECTOR2(m_rect.left + matOffset._41, m_rect.bottom + matOffset._42),
		D3DXVECTOR2(m_rect.left + matOffset._41, m_rect.top + matOffset._42)
	};
	m_pLine->Draw(vecLine, 5, 0x40FF0000);
}