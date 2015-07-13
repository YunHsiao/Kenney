#include "stdafx.h"
#include "Scene.h"
#include "ResourceManager.h"

#include "MonsterBarnacle.h"
#include "MonsterFly.h"
#include "MonsterGroundBiont.h"
#include "MonsterSaw.h"
#include "MonsterFish.h"
#include "MonsterDefine.h"
#include "NPC.h"
#include "GUI.h"
#include "NPCDialog.h"
#include "TaskManager.h"

CScene CScene::s_Scene;
bool g_bDebug(false);

CScene::CScene(void)
:m_pSprite(NULL)
,m_pLine(NULL)
,m_Texture(NULL)
,m_TextureBackground(NULL)
,m_ppBlock(NULL)
,m_pSceneManager(NULL)
,m_nLevelID(0)
,m_iBgCount(1)
,m_iMaxRow(16)
,m_iMaxCol(0)
,m_bisVertIcle(true)
{
	ZeroMemory(&m_matWorld, sizeof(D3DXMATRIX));
	ZeroMemory(&m_matTranslate, sizeof(D3DXMATRIX));
	D3DXMatrixIdentity(&m_matTranslate);
	D3DXMatrixIdentity(&m_matLogo);
	D3DXMatrixIdentity(&m_matWorld);
}

CScene::~CScene(void)
{
	Destroy();
}

void CScene::Destroy()
{	
	SAFE_DELETE(m_player)
	m_mapLevel.clear();
	m_mapPlayerPos.clear();
	m_mapParameters.clear();
	m_pSprite = NULL;
	m_pLine = NULL;
	m_Texture = NULL;
	m_TextureBackground = NULL;
	m_pSceneManager = NULL;
	Clear();
}

void CScene::Clear()
{	 
	m_vecMapElement.clear();
	m_vecPrizes.clear();
	m_vecDoors.clear();
	m_vecBackgroudID.clear();
	D3DXMatrixIdentity(&m_matLogo);

	for (unsigned int i(0); i < m_vecActor.size(); ++i)
		SAFE_DELETE(m_vecActor[i])
		m_vecActor.clear();

	if (m_ppBlock) {
		for (unsigned int i = 0; i < m_iMaxRow; ++i)
			SAFE_DELETE_ARRAY(m_ppBlock[i])
		SAFE_DELETE_ARRAY(m_ppBlock)
	}
}

bool CScene::Initialize(LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTexture, 
						CMANGOSceneManager* pSceneManager, LPD3DXLINE pLine)
{
	m_pSceneManager = pSceneManager;
	m_pLine = pLine;
	if (!LoadMapListByXml("MapData/MapList.xml")) return false;
	if(!LoadParameters("Resource/Monster/Monster.xml"))return false;
	if(!LoadNpcData("Resource/NPC/NpcList.xml"))return false;
	m_pSprite = pSprite;
	m_Texture = pTexture;
	CreatePlayer(ERT_Yellow, D3DXVECTOR3(100.f, 100.f, 0.5f));

	LoadLevel(m_nLevelID);

	return true;
}

bool CScene::LoadMapListByXml(const char* sheet)
{
	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(sheet))
	{
		MessageBox(NULL,TEXT("加载资源时出现错误"),NULL,MB_OK);
		return false;
	}

	tinyxml2::XMLElement* root = Doc.FirstChildElement("Map");
	for(tinyxml2::XMLElement* content = root->FirstChildElement();NULL != content; content = content->NextSiblingElement())
	{
		int nID;
		D3DXVECTOR3 vecPos; vecPos.z = .5f;
		std::string strPath;

		nID = atoi(content->Attribute("LevelID"));
		strPath = content->Attribute("Path");
		
		vecPos.x = (float) atof(content->Attribute("PlayerPositionX"));
		vecPos.y = (float) atof(content->Attribute("PlayerPositionY"));

		m_mapPlayerPos[nID] = vecPos;
		m_mapLevel[nID] = strPath;
	}
	return true;
}

bool CScene::LoadLevel(const int LevelID)
{
	if (LevelID < 0) return false;
	Clear();

	std::string path = m_mapLevel[LevelID];
	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(path.c_str()))
	{
		MessageBox(NULL,TEXT("加载Level资源时出现错误"),NULL,MB_OK);
		return false;
	}

	tinyxml2::XMLElement* root = Doc.FirstChildElement();
	int ElementType(0),max_x(0);
	std::vector<MapElement> monster, NPC;
	for(root->FirstChildElement(); NULL != root; root = root->NextSiblingElement())
	{		
		std::string label(root->Value());
		for(tinyxml2::XMLElement* content = root->FirstChildElement();NULL != content; content = content->NextSiblingElement())
		{
			if ( label == "Trigger" || label == "Collision" ) continue;
			MapElement temMapElement;
			temMapElement.tid = -1;

			temMapElement.id = atoi(content->Attribute("index")) + 1000 * ElementType;
			temMapElement.x = atoi(content->Attribute("x"));
			temMapElement.y = atoi(content->Attribute("y"));
			if(temMapElement.id == 2010)
				temMapElement.swc = atoi(content->Attribute("switch"));
			if (temMapElement.id == 1148 || temMapElement.id == 1150)
				temMapElement.tid = atoi(content->Attribute("TriggerID"));

			temMapElement.rect = CResourceManager::GetIntance()->GetResource(temMapElement.id);

			if (label == "Monster")
				monster.push_back(temMapElement);
			else if (label == "NPC")
				NPC.push_back(temMapElement);
			else if (temMapElement.id == 1000)
				D3DXMatrixTranslation(&m_matLogo, (float)temMapElement.x, 
				(float)temMapElement.y, .5f);
			else
				m_vecMapElement.push_back(temMapElement);

			if (temMapElement.x > max_x)
				max_x = temMapElement.x;
		}

		ElementType++;
	}
	m_iMaxCol = max_x / BLOCKLENGTH + 1;
	m_iBgCount = max_x / 1024 + 1;

	tinyxml2::XMLElement *node;

	// 地图背景的编号
	int BackgroudID = -1;
	node = Doc.FirstChildElement("Backgroud");
	BackgroudID = atoi(node->GetText()) + 1001;

	m_vecBackgroudID[LevelID] = BackgroudID;
	m_TextureBackground = CResourceManager::GetIntance()->GetBackground(m_vecBackgroudID[LevelID]);

	GetBlockInfo();
	m_player->CutScene(m_mapPlayerPos[LevelID], m_ppBlock, (float) m_iMaxCol * BLOCKLENGTH);
	for(unsigned int i(0); i < monster.size(); ++i)
		CreateMonster(monster[i]);
	for(unsigned int i(0); i < NPC.size(); ++i){
		CreateNPC(NPC[i]);
	}
	return true;
}

 void CScene::GetBlockInfo()
 {
	 m_ppBlock = new unsigned int*[m_iMaxRow];
	 for(unsigned int i = 0; i < m_iMaxRow; ++i)
	 {
		 m_ppBlock[i] = new unsigned int[m_iMaxCol];
		 for(unsigned int j = 0; j < m_iMaxCol; ++j) m_ppBlock[i][j] = EBT_None;
	 }

	 for (TilesIt it = m_vecMapElement.begin(); it != m_vecMapElement.end();)
	 {
		 int i = (it->y) / BLOCKLENGTH, j = (it->x) / BLOCKLENGTH;
		 switch (it->id)
		 {
		 // Coins & Heads
		 case 1109:
		 case 1110:
		 case 1111:
		 case 4031:
		 case 4032:
		 case 4033:
		 case 4034:
		 case 4035:
			 if ((m_ppBlock[i][j] & EBT_Prize)==0) 
				 m_ppBlock[i][j] += EBT_Prize;
			 m_vecPrizes.push_back(*it); 
			 it = m_vecMapElement.erase(it);
			 continue;
		 case 1116:
		 case 1117:
		 case 1118:
		 case 1119:
			 // If jewel not acquired
			 if ((m_mapJewel[m_nLevelID] & (1<<(it->id-1116))) == 0) {
				 if ((m_ppBlock[i][j] & EBT_Prize)==0) 
					 m_ppBlock[i][j] += EBT_Prize;
				 m_vecPrizes.push_back(*it); 
			 }
			 it = m_vecMapElement.erase(it);
			 continue;
		 case 1148:
		 case 1150:
			 if ((m_ppBlock[i][j] & EBT_Door)==0) m_ppBlock[i][j] += EBT_Door;
			 m_vecDoors.push_back(*it);
			 break;
		 case 1156:
		 case 1155:
		 case 1152:
			 if ((m_ppBlock[i][j] & EBT_Ladder)==0) m_ppBlock[i][j] += EBT_Ladder;
			 break;
		 case 1183:
		 case 1184:
		 case 1185:
			if ((m_ppBlock[i][j] & EBT_Water)==0) m_ppBlock[i][j] += EBT_Water;
			 break;
		 default:
			 if (it->id <= 1108) { 
				 if (m_ppBlock[i][j] < EBT_BlockMin) 
					m_ppBlock[i][j] += (it->id<<EBT_BlockNum);
			 } else {
				 if ((m_ppBlock[i][j] & EBT_Other)==0) 
					m_ppBlock[i][j] += EBT_Other;
			 }
			 break;
		 }
		 ++it;
	 }
 }

 bool CScene::LoadNpcData(const char* back)
 {
	 tinyxml2::XMLDocument Doc;
	 if(Doc.LoadFile(back)){
		 MessageBox(NULL,TEXT(" NPC - 加载 XML 时出现错误 "),NULL,MB_OK);
		 return false;
	 }
	 tinyxml2::XMLElement* mark = Doc.FirstChildElement();
	 SMonsterParameter talk;
	 for(tinyxml2::XMLElement* content = mark->FirstChildElement(); NULL != content; content = content->NextSiblingElement())
	 {
		 int mapid(0);
		 if(content->Attribute("mapid")){
			 mapid = atoi(content->Attribute("mapid"));
			 talk.NPCId = atoi(content->Attribute("id"));
			 strcpy_s(talk.NPCName, 32, content->Attribute("name"));
			 strcpy_s(talk.NPCDialogInfo,MAX_PATH,content->Attribute("value"));
			 m_multimapNPCList.insert(std::make_pair(mapid,talk));
		 }
	 }
	 return true;
 }
 bool CScene::LoadParameters(const char* back)
 {
	 tinyxml2::XMLDocument Doc;
	 if(Doc.LoadFile(back))
	 {
		 MessageBox(NULL,TEXT("Monster - 加载 XML 时出现错误"),NULL,MB_OK);
		 return false;
	 }
	 tinyxml2::XMLElement* mark = Doc.FirstChildElement();
	 SMonsterParameter mp;
	 for(tinyxml2::XMLElement* content = mark->FirstChildElement(); NULL != content; content = content->NextSiblingElement())
	 {
		 unsigned short id(0);
		 if(content->Attribute("id"))
		 {
			 id					= atoi(content->Attribute("id"));
			 mp.fRestTime		= (float)atoi(content->Attribute("RestTime"));
			 mp.fMirrorTime		= (float)atoi(content->Attribute("MirrorTime"));
			 mp.fDefElapse		= (float)1.f / atoi(content->Attribute("DefElapse"));
			 mp.fGravityWater	= (float)atoi(content->Attribute("GravityWater"));
			 mp.fGravity		= (float)atoi(content->Attribute("Gravity"));
			 mp.fJumpHeight		= (float)atoi(content->Attribute("JumpHeight"));
			 mp.fDefSpe			= (float)atoi(content->Attribute("DefSpe"));
			 m_mapParameters[id] = mp;
		 }
	 }
	 return true;
 }

int CScene::GetChangingLevelID(D3DXVECTOR3 PlayerPos)
{
	if (PlayerPos.y > BG_HEIGHT) { m_player->FillHP(); return m_nLevelID; }
	for (TilesIt it = m_vecDoors.begin(); it != m_vecDoors.end(); it++)
	{
		if (abs(it->y - PlayerPos.y) + abs(it->x - PlayerPos.x) < BLOCKLENGTH*2)
		{
			m_nLevelID = it->tid;
			if (g_bDebug) m_player->FillHP();
			return m_nLevelID;
		}
	}
	return -1;
}

std::pair<bool, CScene::TilesIt> CScene::GetPrizeIt(unsigned int iRow, unsigned int iCol) {
	int x(iCol * BLOCKLENGTH), y(iRow * BLOCKLENGTH);
	for (TilesIt it(m_vecPrizes.begin()); it != m_vecPrizes.end(); ++it) {
		if (it->x == x && it->y == y) return std::make_pair<bool, TilesIt>(true, it);
	}
	return std::make_pair<bool, TilesIt>(false, m_vecPrizes.end());
}

CScene::TilesIt CScene::PrizeAcquired(TilesIt it) { 
	if (it == m_vecPrizes.end()) return it;
	int iJewel;
	switch (it->id) {
	case 1109:
		m_player->GetBag()->AddMoney(1<<0);
		break;
	case 1110:
		m_player->GetBag()->AddMoney(1<<2);
		break;
	case 1111:
		m_player->GetBag()->AddMoney(1<<1);
		break;
	case 1116:
	case 1117:
	case 1118:
	case 1119:
		iJewel = 1<<(it->id - 1116);
		if ((m_mapJewel[m_nLevelID] & iJewel) == 0) {
			m_mapJewel[m_nLevelID] += iJewel;
			if (m_mapJewel[m_nLevelID] == EJI_All) m_player->FillHP();
		}
		break;
	case 4031:
	case 4032:
	case 4033:
	case 4034:
	case 4035:
		m_player->SetRole(ERT_Beige + it->id-4031);
		m_player->GetBag()->SetPerson(it->id);
		return m_vecPrizes.end();
	}
	int iRow(it->y / BLOCKLENGTH), iCol(it->x / BLOCKLENGTH);
	if (m_ppBlock[iRow][iCol] & EBT_Prize) m_ppBlock[iRow][iCol] -= EBT_Prize;
	CTaskManager::GetInstance()->UpdataGather(0.f);
	return m_vecPrizes.erase(it); 
}

int CScene::GetJewel(int nLevelID) {
	int cnt(0), nJewel(m_mapJewel[nLevelID]);
	while (nJewel) { nJewel -= (nJewel & -nJewel); ++cnt; }
	return cnt;
}

SMonsterParameter CScene::GetmultimapNPCList(int mapid,int npcid){
	SMonsterParameter parameter;
	ZeroMemory(&parameter,sizeof(SMonsterParameter));
	std::pair<multimapNPCListIt, multimapNPCListIt> p(m_multimapNPCList.equal_range(mapid));
	for(multimapNPCListIt it(p.first); it != p.second; ++it){
		if(npcid == it->second.NPCId){
			strcpy_s(parameter.NPCName,32,it->second.NPCName);
			strcpy_s(parameter.NPCDialogInfo,MAX_PATH,it->second.NPCDialogInfo);
			break;
		}else{
			continue;
		}
	}

	return parameter;
}
//////////////////////////////////////////////////////////////////////////

void CScene::Tick(float fElapsedTime)
{
	if(NULL == m_pSprite || NULL == m_player) return;

	//切关
	CController::GetInstance()->Tick(fElapsedTime);
	TickActor(fElapsedTime);
	if (m_player->Tick(fElapsedTime)) 
		LoadLevel(GetChangingLevelID(m_player->GetPosition()));
	CGUI::GetInstance()->Tick(m_player->GetHP(), m_player->GetMaxHP(), 
		m_player->GetBag()->GetMoney(), m_mapJewel[m_nLevelID], 
		m_player->GetBag()->GetPerson(), m_player->GetBag()->GetPersonCount(), m_player->GetRole());

	ScrollScreen(m_player->GetPosition());
}

void CScene::ScrollScreen(D3DXVECTOR3 pos)
{
	float x = m_matTranslate._41, y = m_matTranslate._42;
	pos.y += BLOCKLENGTH*2;
	if (pos.x + x > CLIENT_WIDTH * .8f) 
		m_matTranslate._41 = CLIENT_WIDTH * .8f - pos.x;
	else if (pos.x + x < CLIENT_WIDTH * .2f) 
		m_matTranslate._41 = CLIENT_WIDTH * .2f - pos.x;
	if (pos.y + y > CLIENT_HEIGHT * .8f) 
		m_matTranslate._42 = CLIENT_HEIGHT * .8f - pos.y;
	else if (pos.y + y < CLIENT_HEIGHT * .2f) 
		m_matTranslate._42 = CLIENT_HEIGHT * .2f - pos.y;

	if (m_matTranslate._41 > 0.f) m_matTranslate._41 = 0.f;
	else if (m_matTranslate._41 < CLIENT_WIDTH-(float)m_iMaxCol*BLOCKLENGTH) 
  		m_matTranslate._41 = CLIENT_WIDTH-(float)m_iMaxCol*BLOCKLENGTH;
	if (m_matTranslate._42 > 0.f) m_matTranslate._42 = 0.f;
	else if (m_matTranslate._42 < CLIENT_HEIGHT-(float)m_iMaxRow*BLOCKLENGTH) 
		m_matTranslate._42 = CLIENT_HEIGHT-(float)m_iMaxRow*BLOCKLENGTH;
}

void CScene::TickActor(float fElapsedTime) {
	for (ActorIt it(m_vecActor.begin()); it != m_vecActor.end();) {
		if ((*it)->Tick(fElapsedTime)) {
			SAFE_DELETE(*it)
			it = m_vecActor.erase(it);
			if (it == m_vecActor.end() || (*it) == m_vecActor.back())
				break;
		}
		++it;
	}
	//调试开关
	if(GetAsyncKeyState(VK_F11) & 0x8000) {
		g_bDebug = true;
		//WriteDebugInfo();
		//CResourceManager::GetIntance()->WritePixels(3001);
	}if(GetAsyncKeyState(VK_F10) & 0x8000) {
		g_bDebug = false;
	}
}

void CScene::WriteDebugInfo() {
	std::ofstream out("m_ppBlock.txt");
	char buff[4096] = {0};
	for(unsigned int i = 0; i < m_iMaxRow; ++i) {
		for(unsigned int j = 0; j < m_iMaxCol; ++j) {
			char b[16] = {0};
			unsigned int o(m_ppBlock[i][j]);
			/* Toggle Full Binary -->*/
			if (o > EBT_BlockMin) {
				char t[16] = {0};
				unsigned int block(o>>EBT_BlockNum),
					special(o-(block<<EBT_BlockNum));
				_itoa_s(block, b, 10);
				if (special) {
					_itoa_s(special, t, 2);
					strcat_s(b, -1, "+");
					strcat_s(b, -1, t);
				}
			} else /**/
				_itoa_s(o, b, 2);
			strcat_s(buff, -1, b);
			strcat_s(buff, -1, "\t");
		}
		strcat_s(buff, -1, "\n");
	}
	out << buff; out.close();
}

//////////////////////////////////////////////////////////////////////////

void CScene::Render()
{
	if(NULL == m_pSprite || NULL == m_player) return;
	RenderLogo();
	RenderBackground();
	RenderBrush();
	RenderActor();
	CGUI::GetInstance()->Render();
}

void CScene::RenderLogo() {
	if (m_matLogo._41) {
		m_pSprite->SetTransform(&(m_matLogo*m_matTranslate));
		m_pSprite->Draw(CResourceManager::GetIntance()->GetBackground(1000), 
			NULL, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CScene::RenderBackground()
{
	D3DXMATRIX matBackground, matBgTranslate;
	D3DXMatrixIdentity(&matBackground);

	for (int i = 0; i != m_iBgCount; i++)
	{
		D3DXMatrixTranslation(&matBackground, i * 1024.f, 0, 1.f);
		matBgTranslate = m_matTranslate;
		matBgTranslate._41 /= 2.f;
		m_matWorld = matBackground * matBgTranslate;
		m_pSprite->SetTransform(&m_matWorld);

		m_pSprite->Draw(m_TextureBackground, NULL, NULL, NULL, D3DCOLOR_ARGB(128, 255, 255, 255));
	}

}
void CScene::RenderBrush()
{
	D3DXMATRIX matBrush;
 	for (TilesIt it = m_vecMapElement.begin(); it != m_vecMapElement.end(); ++it)
 	{
		D3DXMatrixTranslation(&matBrush, (float)it->x, (float)it->y, 1.f);
		m_matWorld = matBrush * m_matTranslate;
		m_pSprite->SetTransform(&m_matWorld);
		m_pSprite->Draw(m_Texture, &(it->rect), NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	for (TilesIt it = m_vecPrizes.begin(); it != m_vecPrizes.end(); ++it)
	{
		D3DXMatrixTranslation(&matBrush, (float)it->x, (float)it->y, 1.f);
		m_matWorld = matBrush * m_matTranslate;
		m_pSprite->SetTransform(&m_matWorld);
		m_pSprite->Draw(m_Texture, &(it->rect), NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CScene::RenderActor() {
	m_player->Render(m_matTranslate);
	for (unsigned int i(0); i < m_vecActor.size(); ++i)
		m_vecActor[i]->Render(m_matTranslate);
}

void CScene::CreatePlayer(ERoleType eRoleType, D3DXVECTOR3 vecPos)
{
	if (m_player) return;
	m_player = new CPlayer(eRoleType, vecPos, m_pSprite, m_Texture, m_pLine);
	CController::GetInstance()->Initialize(m_player);
}

void CScene::CreateMonster(MapElement& mapElement) {

	switch(mapElement.id)
	{
	case EMT_Barnacle:
		m_vecActor.push_back(new CMonsterBarnacle(
			CResourceManager::GetIntance()->GetResourceRange(mapElement.id),
			D3DXVECTOR3((float)mapElement.x, (float)mapElement.y, 0.f), 
			m_pLine, m_pSprite, m_Texture, m_ppBlock, m_player, 
			mapElement.id));
		break;
	case EMT_Bee:
	case EMT_Fly:
		m_vecActor.push_back(new CMonsterFly(
			CResourceManager::GetIntance()->GetResourceRange(mapElement.id),
			D3DXVECTOR3((float)mapElement.x, (float)mapElement.y, 0.f), 
			m_pLine, m_pSprite, m_Texture, m_ppBlock, m_player, 
			mapElement.id));
		break;
	case EMT_FishBlue:
	case EMT_FishGreen:
	case EMT_FishPink:
		m_vecActor.push_back(new CMonsterFish(
			CResourceManager::GetIntance()->GetResourceRange(mapElement.id),
			D3DXVECTOR3((float)mapElement.x, (float)mapElement.y, 0.f), 
			m_pLine, m_pSprite, m_Texture, m_ppBlock, m_player, 
			mapElement.id));
		break;
	case EMT_Saw:
	case EMT_SawHalf:
		m_vecActor.push_back(new CMonsterSaw(
			CResourceManager::GetIntance()->GetResourceRange(mapElement.id),
			D3DXVECTOR3((float)mapElement.x, (float)mapElement.y, 0.f), 
			m_pLine, m_pSprite, m_Texture, m_ppBlock, m_player, 
			mapElement.id, mapElement.swc));
		break;
	case EMT_Frog:
	case EMT_Ladybug:
	case EMT_Mouse:
	case EMT_SlimeBlock:
	case EMT_SlimeBlue:
	case EMT_SlimeGreen:
	case EMT_SlimePurple:
	case EMT_Snail:
	case EMT_WormGreen:
	case EMT_WormPink:
		m_vecActor.push_back(new CMonsterGroundBiont(
			CResourceManager::GetIntance()->GetResourceRange(mapElement.id),
			D3DXVECTOR3((float)mapElement.x, (float)mapElement.y, 0.f), 
			m_pLine, m_pSprite, m_Texture, m_ppBlock, m_player, 
			mapElement.id));
		break;
	}
}

void CScene::CreateNPC(MapElement& mapElement) {
	m_vecActor.push_back(new CNPC(mapElement.id, 
		D3DXVECTOR3((float)mapElement.x, (float)mapElement.y, .5f), 
		m_pSprite, m_Texture, m_player, m_ppBlock, m_pLine));
}

void CScene::Click(POINT pos) {
	for(unsigned int i(0); i < m_vecActor.size(); ++i)
	{
		if(m_vecActor[i]->IsNPC()) {
			D3DXVECTOR3 vPos = m_vecActor[i]->GetPosition(), vSize = m_vecActor[i]->GetSize();
			vPos.x += m_matTranslate._41;
			vPos.y += m_matTranslate._42;
			RECT rect = { static_cast<long>(vPos.x+vSize.x*.258f), 
				static_cast<long>(vPos.y+vSize.y*.414f), 
				static_cast<long>(vPos.x+vSize.x*.719f), 
				static_cast<long>(vPos.y+vSize.y) };
			if (PtInRect(&rect, pos)) {
				CNPCDialog::GetInstance()->SetNPCID(m_vecActor[i]->GetID());
				CTaskManager::GetInstance()->UpdataDialogue(0.f);
				CTaskManager::GetInstance()->UpdataKillMonster(0.f);
				CTaskManager::GetInstance()->UpdataGather(0.f);
				CNPCDialog::GetInstance()->SetVisible(true);
			}

		}
	}
	unsigned int iHead(0);
	if (iHead = CGUI::GetInstance()->ClickOnHead(pos)) {
		m_player->SetRoleByHeadId(iHead, false);
	}
}