#include "stdafx.h"
#include "SceneManager.h"

CSceneManager CSceneManager::s_sm;

CSceneManager::CSceneManager()
{

}

CSceneManager::~CSceneManager() 
{

}

void CSceneManager::Tick(FLOAT fElapsedTime) {

}
//
//CWorld CWorld::s_World;
//
//CWorld::CWorld():
//m_pD3DDevice(NULL),
//m_pSprite(NULL),
//m_pSceneManager(NULL),
//
//m_iCurMapID(-1),
//m_fTriggerCheckTime(0.f),
//
////m_iMapRow(0),
////m_iMapCol(0),
////m_pMapData(NULL),
//
//m_pBrushElement(NULL),
//m_pTriggerElment(NULL)
//
////m_pPlayer(NULL),
////m_pNPCManager(NULL),
////m_pMonsterManager(NULL),
////m_pTaskManager(NULL),
////m_pItemManager(NULL)
//{
//}
//
//CWorld::~CWorld()
//{
//	Destroy();
//}
//
//void CWorld::Destroy()
//{
//	Clear();
//
//	//// ɾ�����
//	//SAFE_DELETE(m_pPlayer)
//
//	//// ɾ��NPC������
//	//SAFE_DELETE(m_pNPCManager)
//
//	//// ɾ��Monster������
//	//SAFE_DELETE(m_pMonsterManager)
//
//	//// ɾ��TaskManager
//	//SAFE_DELETE(m_pTaskManager)
//
//	//// ɾ�����������
//	//SAFE_DELETE(m_pItemManager)
//
//	// �ͷ�D3D�����Դ
//	SAFE_RELEASE(m_pSprite)
//	m_pD3DDevice = NULL;
//	m_pSceneManager = NULL;
//}
//
//void CWorld::Clear()
//{
//	// ���NPC
//	//if (NULL != m_pNPCManager)
//	//	m_pNPCManager->Clear();
//
//	// ���Monster
//	//if (NULL != m_pMonsterManager)
//	//	m_pMonsterManager->Clear();
//
//	//SAFE_DELETE_ARRAY(m_pMapData)
//}
//
//void CWorld::Initialize(LPDIRECT3DDEVICE9 pD3DDevice, CMANGOSceneManager* pSceneManager)
//{
//	m_pD3DDevice = pD3DDevice;
//	m_pSceneManager = pSceneManager;
//
//	// �������ƾ���
//	HRESULT hr;
//	V(D3DXCreateSprite(m_pD3DDevice, &m_pSprite))
//
//	//// ������Ʒ������
//	//m_pItemManager = new CItemManager;
//	//m_pItemManager->Initialize();
//
//	//// �������
//	//m_pPlayer = new CPlayer;
//	//m_pPlayer->Initialize(m_pD3DDevice, m_pSprite, pSceneManager);
//	//m_pPlayer->SetPosition(260, 50);
//
//	//// ����NPC������
//	//m_pNPCManager = new CNPCManager;
//
//	//// ����Monster������
//	//m_pMonsterManager = new CMonsterManager;
//
//	//// �������������
//	//m_pTaskManager = new CTaskManager;
//	//m_pTaskManager->Initialize(pSceneManager);
//}
//
////void CWorld::EnterScene(const char* filename)
////{
//	//tinyxml2::XMLDocument pDoc;
//	//if(pDoc->LoadFile(filename)) return;
//
//	//// �����ǰ����
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
//	//// ����Monster
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
//	//		// ��ɫ��ʼλ��
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
//	// ������ѭ�����
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
//	//		// �л�����
//	//		ChangeScene(m_pTriggerElment[i].m_iTriggerID);
//	//		break;
//	//	}
//	//}
//
//	//// ���´�����ѭ�����ʱ��,ע�ⲻҪ��ֵΪ0.f,���������ۼ����
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
//
////LRESULT CWorld::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
////{
//	//if (NULL == m_pPlayer)
//	//	return 0;
//
//	//return m_pPlayer->MsgProc(hWnd, uMsg, wParam, lParam);
////}