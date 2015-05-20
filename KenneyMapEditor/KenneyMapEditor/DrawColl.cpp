#include "StdAfx.h"
#include "DrawColl.h"

CDrawColl::CDrawColl(void)
:m_bIsActive  (FALSE)
,m_bLerp	  (FALSE)
,m_pTexture   (NULL)
,m_dwMapColor (0xFFFFFFFF)
,m_dwLerpColor(0xFF000000)
,m_nCellWidth (8)
,m_nCellHeight(8) 
,m_nNum       (1)
,m_pSprite    (NULL)
{
	m_nRowSelect=-1;
	m_nColSelect=-1;
}

void CDrawColl::Destroy()
{
	// m_pTexture 是悬垂指针
	m_pTexture = NULL;

	ClearColls();
	SAFE_RELEASE(m_pSprite);
}


CDrawColl::~CDrawColl(void)
{
	Destroy();
}

HRESULT CDrawColl::OnCreate(LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DTEXTURE9 pTexture)
{
	HRESULT hr = S_OK;

	m_pTexture = pTexture;

	D3DXCreateSprite(pd3dDevice,&m_pSprite);

	//激活地图
	m_bIsActive = TRUE;

	return hr;
}

void CDrawColl::Update(LONG px, LONG py)
{
	m_nRowSelect = px / m_nCellWidth;
	m_nColSelect = py / m_nCellHeight;
	px = m_nRowSelect * m_nCellWidth;
	py = m_nColSelect * m_nCellHeight;
	if (m_nNum) {
		for (VecPointsIt it(m_vColl.begin()); it != m_vColl.end(); ++it)
			if ((*it)->x == px && (*it)->y == py) return;
		if (m_vColl.empty()) {
			m_vColl.push_front(new D3DXVECTOR2((FLOAT)px, (FLOAT)py));
			ResetInsertIt();
		} else m_cIt = m_vColl.insert(m_cIt, new D3DXVECTOR2((FLOAT)px, (FLOAT)py));
	} else {
		for (VecPointsIt it(m_vColl.begin()); it != m_vColl.end(); ++it) {
			if ((*it)->x == px && (*it)->y == py) {
				SAFE_DELETE(*it)
				m_cIt = m_vColl.erase(it);
				break;
			}
		}
	}
}

HRESULT CDrawColl::Render(POINT& offset)
{
	if(!m_bIsActive || m_vColl.empty())
		return S_OK;

	m_pSprite->Begin(0);
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	if (m_bLerp) {
		D3DXVECTOR2 lerp;
		VecPointsIt j = m_vColl.begin();
		for (UINT i(0); i < m_vColl.size()-1; ++i) ++j;
		for(VecPointsIt i = m_vColl.begin(); i != m_vColl.end(); j = i++)
		{
			FLOAT step = 1.f / D3DXVec2Length(&(**i-**j));
			for (FLOAT s(step); s < 1.f; s += step) {
				D3DXVec2Lerp(&lerp, *j, *i, s);
				matWorld._41=lerp.x-offset.x;
				matWorld._42=lerp.y-offset.y;
				m_pSprite->SetTransform(&matWorld);
				m_pSprite->Draw(m_pTexture, NULL, NULL, NULL, m_dwLerpColor);
			}
			
		}
	}
	for(VecPointsIt it = m_vColl.begin(); it != m_vColl.end(); ++it)
	{
		matWorld._41=(*it)->x-offset.x;
		matWorld._42=(*it)->y-offset.y;
		m_pSprite->SetTransform(&matWorld);
		m_pSprite->Draw(m_pTexture, NULL, NULL, NULL, m_dwMapColor);
	}
	m_pSprite->End();

	return S_OK;
}