#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "Controller.h"
#include "GUI.h"
#include "NPCDialog.h"
#include "TaskDialog.h"
#include "ShopDialog.h"
#include "Goods.h"
#include "TaskManager.h"

CSceneManager CSceneManager::s_sm;

CSceneManager::CSceneManager()
:m_pD3DDevice(NULL)
,m_pSprite(NULL)
,m_pSceneManager(NULL)
,m_pTexture(NULL)
{

}

CSceneManager::~CSceneManager() 
{
	Destroy();
}

void CSceneManager::Destroy()
{
	//Clear();

	// 释放D3D相关资源
	CResourceManager::GetIntance()->Destroy();
	CScene::GetInstance()->Destroy();
	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pLine);
	m_pSceneManager = NULL;
	m_pSprite = NULL;
	m_pD3DDevice = NULL;
}

void CSceneManager::Clear()
{
	//SAFE_DELETE_ARRAY(m_pBrushElement)
}

bool CSceneManager::Initialize(LPDIRECT3DDEVICE9 pD3DDevice, CMANGOSceneManager* pSceneManager)
{
	m_pD3DDevice = pD3DDevice;
	m_pSceneManager = pSceneManager;

	// 创建绘制精灵
	HRESULT hr;
	if (FAILED(hr = D3DXCreateSprite(m_pD3DDevice, &m_pSprite))) return false;
	if (FAILED(hr = D3DXCreateLine(m_pD3DDevice, &m_pLine))) return false;

	if (!CResourceManager::GetIntance()->Initialize("Resource/Block/spritesheet.png", "Resource/Block/spritesheet.xml", "Resource/Background/background.xml"))
		return false;

	m_pTexture = CResourceManager::GetIntance()->GetAtlas();

	// 初始化场景
	if (!CScene::GetInstance()->Initialize(m_pSprite, m_pTexture, m_pSceneManager, m_pLine))
		return false;

	if (!CGUI::GetInstance()->Initialize(m_pSprite, m_pTexture))
		return false;

	if(!CNPCDialog::GetInstance()->Init(m_pSceneManager)) 
		return false;

	if(!CTaskDialog::GetInstance()->Init(m_pSceneManager)) 
		return false;

	if(!CShopDialog::GetInstance()->Init(m_pSceneManager)) 
		return false;

	if (!CGoods::GetInstance()->Init(m_pSceneManager))
		return false;

	if(!CTaskManager::GetInstance()->Init())
		return false;

	return true;
}

void CSceneManager::Tick(float fElapsedTime) 
{
	CScene::GetInstance()->Tick(fElapsedTime);
}

void CSceneManager::Render() 
{
	// Render总入口
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	CScene::GetInstance()->Render();

	m_pSprite->End();
}


LRESULT CSceneManager::MsgProc(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
	
	return 1;
}

////void CWorld::EnterScene(const char* filename)
////{
//	//tinyxml2::XMLDocument pDoc;
//	//if(pDoc->LoadFile(filename)) return;
//
//	//// 清除当前数据
//	//Clear();
//
//	//tinyxml2::XMLElement* pRoot = pDoc.FirstChildElement("SceneInclude");
//	//tinyxml2::XMLNode* _p = pRoot->FirstChild("MapData");
//	//tinyxml2::XMLElement* _eleve = _p->FirstChildElement("Path");
//	//LoadMapData(_eleve->Attribute("File"));
//
//	//_p = _p->NextSibling("Scene");
//	//_eleve = _p->FirstChildElement("Path");
//	//LoadMapResource(_eleve->Attribute("File"));
//
//	//_p = _p->NextSibling("Sound");
//	//_eleve = _p->FirstChildElement("Path");
//	//LoadSound(_eleve->Attribute("File"));
//
//	//// 加载Monster
//	//if (NULL != m_pMonsterManager)
//	//{
//	//	srand(timeGetTime());
//	//	m_pMonsterManager->LoadMonster(m_pD3DDevice, m_pSprite, m_iCurMapID);
//	//}
////}
//
////void CWorld::ChangeScene(int iTriggerID)
////{
//	//tinyxml2::XMLDocument pDoc;
//	//if(pDoc->LoadFile("Scene/SceneChange.xml")) return;
//
//	//tinyxml2::XMLElement* pRoot = pDoc.FirstChildElement("SceneChange");
//	//for(tinyxml2::XMLElement* _eleve = pRoot->FirstChildElement("Scene"); NULL != _eleve; _eleve = _eleve->NextSiblingElement())
//	//{
//	//	int ID = atoi(_eleve->Attribute("id"));
//	//	if (m_iCurMapID == ID)
//	//	{
//	//		int triggerid = atoi(_eleve->Attribute("triggerid"));
//	//		if (triggerid != iTriggerID)
//	//			continue;
//
//	//		// 角色起始位置
//	//		int iPosX = atof(_eleve->Attribute("X"));
//	//		int iPosY = atof(_eleve->Attribute("Y"));
//	//		if (NULL != m_pPlayer)
//	//		{
//	//			m_pPlayer->SetPosition(iPosX, iPosY);
//	//			m_pPlayer->SetActionType(EPA_Stand);
//	//		}
//
//	//		EnterScene(_eleve->Attribute("File"));
//	//		break;
//	//	}
//	//}
////}
//
//bool CWorld::Tick(float fElapsedTime)
//{
//	// 触发器循环检测
//	//TickTrigger(fElapsedTime);
//
//	//if (NULL != m_pPlayer)
//	//	m_pPlayer->Tick(fElapsedTime);
//
//	//if (NULL != m_pNPCManager)
//	//	m_pNPCManager->Tick(fElapsedTime);
//
//	//if (NULL != m_pMonsterManager)
//	//	m_pMonsterManager->Tick(fElapsedTime);
//
//	return true;
//}
//
////bool CWorld::TickTrigger(float fElapsedTime)
////{
//	//m_fTriggerCheckTime += fElapsedTime;
//	//if (WORLD_TRIGGER_CHECK_TIME > m_fTriggerCheckTime)
//	//	return true;
//
//	//for (int i = 0; i < m_iTriggerCount; ++i)
//	//{
//	//	if (CheckTrigger(&m_pTriggerElment[i]))
//	//	{
//	//		// 切换场景
//	//		ChangeScene(m_pTriggerElment[i].m_iTriggerID);
//	//		break;
//	//	}
//	//}
//
//	//// 更新触发器循环检测时间,注意不要赋值为0.f,否则会产生累计误差
//	//m_fTriggerCheckTime -= WORLD_TRIGGER_CHECK_TIME;
//	//return true;
////}
//
////bool CWorld::CheckTrigger(MapElement* pTrigger)
////{
//	//if (NULL == pTrigger || NULL == m_pPlayer)
//	//	return false;
//
//	//D3DXVECTOR2 ptPlayer = m_pPlayer->GetPosition();
//	//D3DXVECTOR2 ptTrigger(pTrigger->m_fImgX + 16, pTrigger->m_fImgY + 16);
//	//float fDistance = D3DXVec2Length(&(ptPlayer - ptTrigger));
//	//if (WORLD_TRIGGER_CHECK_DISTANCE >= fDistance)
//	//	return true;
//
//	//return false;
////}
