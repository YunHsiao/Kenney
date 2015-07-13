#include "StdAfx.h"
#include "ShopDialog.h"
#include "DlgDefine.h"
#include "Goods.h"


CShopDialog CShopDialog::m_sShopDialog;

CShopDialog::CShopDialog(void):
m_mgSceneManager(NULL)
{
}

CShopDialog::~CShopDialog(void)
{
	Destroy();
}

void CShopDialog::Destroy()
{
	m_mgSceneManager = NULL;
}

bool CShopDialog::Init(CMANGOSceneManager* mgSceneManager)
{
	m_mgSceneManager = mgSceneManager;
	if(m_mgSceneManager == NULL)
		return false;
	if(!m_mgSceneManager->CreateScene("Resource/Layout/ShopDialog.xml")){
		MessageBox( NULL,TEXT(" ShopDialog - 加载 XML 时出现错误 "),NULL,MB_OK );
		return false;
	}

	SetVisible(false);
	return true;
}

void CShopDialog::ShowShopDialog(const int id, bool visible)
{
	this->SetVisible(visible);
	CGoods::GetInstance()->ShowGoods(id,visible);
}
void CShopDialog::SetVisible(bool visible /* = false */)
{
	if(m_mgSceneManager == NULL)
		return;

	m_mgSceneManager->GetSceneByID(ED_ShopDialog)->SetVisible(visible);
	CGoods::GetInstance()->SetVisible(visible);

}

void CShopDialog::OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext)
{
	switch(nControlID)
	{
	case EB_Quit:
		this->SetVisible(false);
		m_mgSceneManager->GetSceneByID(ED_ShopDialog)->GetButtonByID(EB_Quit)->SetVisible(true);
		break;
	}
}