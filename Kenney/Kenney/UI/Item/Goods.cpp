#include "StdAfx.h"
#include "Goods.h"
#include "DlgDefine.h"

#include "Scene.h"

CGoods CGoods::m_sGoods;
extern bool g_bDebug;

CGoods::CGoods(void):
m_mgScene(NULL)
,m_mgSceneManager(NULL)
,m_iGoodsID(0)
,m_pBag(0)
,m_iNpcId(0)
{

}

CGoods::~CGoods(void)
{
	Destroy();
}
void CGoods::Destroy()
{
	m_mgSceneManager = NULL;
	m_mgScene = NULL;
	m_pBag = NULL;
}

bool CGoods::Init(CMANGOSceneManager* mgSceneManager)
{
	m_mgSceneManager = mgSceneManager;
	if(!m_mgSceneManager->CreateScene("Resource/Goods/Goods.xml")){
		MessageBox(NULL,TEXT(" Goods - 加载 XML 时出现错误 "),NULL,MB_OK);
		return false;
	}

	SetVisible(false);

	m_pBag = CScene::GetInstance()->GetPlayer()->GetBag();
	if(NULL == m_pBag)
		return false;

	return true;
}
void CGoods::ShowGoods(const int id, bool visible)
{
	this->SetVisible(visible);
	HideGoods();
	switch(id)
	{
	case 3001:
		ShowGood(4031);
		break;
	case 3002:
		ShowGood(4032);
		break;
	case 3003:
		ShowGood(4033);
		break;
	case 3004:
		ShowGood(4034);
		break;
	case 3005:
		ShowGood(4035);
		break;

	}
}
void CGoods::SetVisible( bool visible /* = false */ )
{
	if(m_mgSceneManager == NULL)
		return;
	m_mgSceneManager->GetSceneByID(ED_Goods)->SetVisible(visible);
}

void CGoods::HideGoods()
{
	for(int i(4031); i <= 4035; ++i){
		m_mgSceneManager->GetSceneByID(ED_Goods)->GetButtonByID(i)->SetVisible(false);
	}
}

void CGoods::ShowGood(int id)
{
	if(m_mgSceneManager == NULL)
		return;
	m_mgSceneManager->GetSceneByID(ED_Goods)->GetButtonByID(id)->SetVisible(true);
}
void CGoods::OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext)
{
	int cost(g_bDebug?0:100);
	switch(nControlID)
	{
	case 4031:
		if(m_pBag->SubtractMoney(cost)){
			m_pBag->AddPersonByID(4031);
		}
		break;
	case 4032:
		if(m_pBag->SubtractMoney(cost)){
			m_pBag->AddPersonByID(4032);
		}
		break;
	case 4033:
		if(m_pBag->SubtractMoney(cost)){
			m_pBag->AddPersonByID(4033);
		}
		break;
	case 4034:
		if(m_pBag->SubtractMoney(cost)){
			m_pBag->AddPersonByID(4034);
		}
		break;
	case 4035:
		if(m_pBag->SubtractMoney(cost)){
			m_pBag->AddPersonByID(4035);
		}
		break;
	}
}