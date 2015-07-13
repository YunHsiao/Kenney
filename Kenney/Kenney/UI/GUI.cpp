#include "StdAfx.h"
#include "GUI.h"
#include "ResourceManager.h"
#include "Bag.h"
#include "ItemDefine.h"

CGUI CGUI::s_GUI;

//  ID Data      索引
#define HP_BLANK 4012	  // 血槽
#define HP_FULL  4013	  // 满格血
#define HP_HALF  4014	  // 半格血
#define NUMBER_0 4001	  // 数字 0
#define MAX_GOLD 9999	  // 规定最高分

CGUI::CGUI(void)
:m_pSprite(NULL)
,m_Texture(NULL)
,m_IsVisible(true)
,m_fLayerDapth(0)
,m_iMaxHP(0)
,m_iCurHP(0)
,m_iGoldNum(-1)
,m_progress(0)
,m_oldGoldNumsize(0)
{
}

CGUI::~CGUI(void)
{
	Destroy();
}

void CGUI::Destroy()
{	
	m_pSprite= NULL;
	m_Texture = NULL;

	m_mtmapUIData.clear();
}

bool CGUI::Initialize(LPD3DXSPRITE pSprite, LPDIRECT3DTEXTURE9 pTexture, float fLayerDapth)
{
	m_pSprite = pSprite;
	m_Texture = pTexture;
	m_fLayerDapth = fLayerDapth;

	if (!LoadUIData("Resource\\UI\\UI.xml")) return false;
	return true;
}

bool CGUI::LoadUIData(const char* sheet)
{
	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(sheet))
	{
		MessageBox(NULL,TEXT("加载UI资源时出现错误"),NULL,MB_OK);
		return false;
	}

	tinyxml2::XMLElement* root = Doc.FirstChildElement();;

	unsigned int ElementType(0);
	for(root->FirstChildElement(); NULL != root; root = root->NextSiblingElement())
	{		
		for(tinyxml2::XMLElement* content = root->FirstChildElement();NULL != content; content = content->NextSiblingElement())
		{
			unsigned int UIBlockID;
			UIData temUIData;

			temUIData.eType = (UIType)ElementType;
			temUIData.x = atoi(content->Attribute("x"));
			temUIData.y = atoi(content->Attribute("y"));
			UIBlockID = atoi(content->Attribute("ID"));
			temUIData.rect = CResourceManager::GetIntance()->GetResource(UIBlockID);
			temUIData.visible = false;

			m_mtmapUIData.insert(std::make_pair(UIBlockID, temUIData));		

		}ElementType++;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

void CGUI::Tick(const int iCurHP, const int iMaxHP, const int iGoldNum, 
				const int iJewel, const int iHead, const int iHeadCnt, const int iCurRole)
{
	if(NULL == m_pSprite) return;

	UIType tempType;

 	for (UIBlockDataIt it = m_mtmapUIData.begin(); it != m_mtmapUIData.end(); ++it)
	{
		tempType= it->second.eType;
	 	switch(tempType)
	 	{
	 	case ET_HP:
				TickHP(it,iMaxHP,iCurHP);
			break;
		case ET_HEAD:
				TickHEAD(it, iHead, iHeadCnt, iCurRole);
			break;
		case ET_JEWEL:
				TickJEWEL(it, iJewel);
			break;
		case ET_JEWEL_BLANK:
				it->second.visible = TickJEWEL_BLANK();
			break;
		case ET_GOLD:
				it->second.visible = true;
			break;
		case ET_NUMBER:
				TickNUMBER(it, iGoldNum);
			break;
		}
 	}	
}

void CGUI::TickHP(UIBlockDataIt it,const int iMaxHP, const int iCurHP)
{	
	if (m_iMaxHP == iMaxHP && m_iCurHP == iCurHP)
		return;

	//HP上限值
	if (m_iMaxHP != iMaxHP && it->first == HP_BLANK)
	{
		ChangeItemCount(HP_BLANK, (int)(iMaxHP - m_iMaxHP) / 2);
		m_iMaxHP = iMaxHP;
	}

	//HP当前值
	if (m_iCurHP != iCurHP && (it->first != HP_BLANK))
	{
		if (!((iCurHP) % 2))   // 偶数
		{
			if (iCurHP == iMaxHP && (m_iCurHP % 2) != 0 && m_iCurHP < iMaxHP)  // if player is die	
			ChangeItemCount(HP_FULL,(iCurHP - m_iCurHP) % 2 == 0 ? (iCurHP - m_iCurHP) / 2 : (int)((iCurHP - m_iCurHP) / 2 + 1) );
			else
			ChangeItemCount(HP_FULL,(int)((iCurHP - m_iCurHP) / 2));

			UIBlockDataIt pHP_HALF(--m_mtmapUIData.upper_bound(HP_HALF));
			if (pHP_HALF->second.visible)
			{
				ChangeItemCount(HP_HALF,-1);
			}
		}
		else				   // 奇数
		{
			ChangeItemCount(HP_FULL,-1);
			ChangeItemCount(HP_HALF,1);
		}
		m_iCurHP = iCurHP;
	}
	
}

void CGUI::TickHEAD(UIBlockDataIt it, const int iHead, const int iHeadCnt, const int iCurRole)
{	
	// visible
	int target;
	switch (it->first) {
	case Beige:
		target = EIP_Beige;
		break;
	case Blue:
		target = EIP_Blue;
		break;
	case Green:
		target = EIP_Green;
		break;
	case Pink:
		target = EIP_Pink;
		break;	
	case Yellow:
		target = EIP_Yellow;
		break;
	}
	if (iHead & target) it->second.visible = true;
	else it->second.visible = false;

	// change pos
	if (it->first == iCurRole && it->second.visible)	it->second.x = 32; // 此处可以加特效
	else if (it->first != iCurRole && it->second.visible && vecHead_x.size()) 
		it->second.x = vecHead_x[vecHead_x.size() - 1] + BLOCKLENGTH;

	if (it->second.visible)		vecHead_x.push_back(it->second.x);

	if (vecHead_x.size() >= 100)  
		vecHead_x.clear();			// 此处只是简单优化 可用deque的pop_front 

}

unsigned int CGUI::ClickOnHead(POINT pos)
{
	if(NULL == m_pSprite) return 0;

	RECT rect;
	for (UIBlockDataIt it = m_mtmapUIData.begin(); it != m_mtmapUIData.end(); ++it)
	{
		rect.left = it->second.x; rect.right = it->second.x + BLOCKLENGTH;
		rect.top = it->second.y; rect.bottom = it->second.y + BLOCKLENGTH;
		if (ET_HEAD == it->second.eType && it->second.visible && PtInRect(&rect, pos))
			return it->first;
	}	
	return 0;
}

bool CGUI::TickJEWEL_BLANK()
{
	return m_IsVisible;
}

void CGUI::TickJEWEL(UIBlockDataIt it, const int iJewel)
{
	int target;
	switch (it->first) {
	case 4015:
		target = EJT_Blue;
		break;
	case 4017:
		target = EJT_Green;
		break;
	case 4019:
		target = EJT_Red;
		break;
	case 4021:
		target = EJT_Yellow;
		break;
	}
	if (iJewel & target) it->second.visible = true;
	else it->second.visible = false;
}

void CGUI::TickNUMBER(UIBlockDataIt it, const int iGoldNum)
{
	// 1. 安全检测 (条件判断进行优化)
	if (iGoldNum < 0 || iGoldNum > MAX_GOLD) 	return;
	if (m_iGoldNum == iGoldNum)		return;

	// 2. 解析数据
	std::vector<int> vecGoldNum;
	int itempGoldNum = iGoldNum;

	while(itempGoldNum >= 0)
	{
		int singleFigures = itempGoldNum % 10;
		itempGoldNum /= 10;
		vecGoldNum.push_back(singleFigures); 
		if (!itempGoldNum) break;
	}

	std::vector<int> vecCopy = vecGoldNum;
	std::sort(vecCopy.begin(), vecCopy.end()); 
	std::reverse(vecCopy.begin(), vecCopy.end());
	std::vector<int>::iterator itNum = find(vecCopy.begin(), vecCopy.end(), it->first - NUMBER_0);
	int numPos = vecCopy.end() - itNum;

	std::reverse(vecGoldNum.begin(), vecGoldNum.end());
	// 3. 改变数值
//  cleanData
	if (m_progress == m_oldGoldNumsize)
	{	
		for (UIBlockDataIt iter = m_mtmapUIData.begin(); iter != m_mtmapUIData.end(); )
		{
			if (iter->second.eType == ET_NUMBER && iter->second.visible) {
				iter = m_mtmapUIData.erase(iter); 
				continue;
			}
			++iter;
		} m_progress = 0;
	}
//  changeData
	UIData temUIData;
	for (int i = 0; i < (int)vecGoldNum.size(); ++i)
	{
		 if ((vecGoldNum[i] + NUMBER_0) == it->first && numPos != m_progress)
		 {
			 temUIData = m_mtmapUIData.lower_bound(it->first)->second;
			 temUIData.x += static_cast<int>((BLOCKLENGTH * (i+1)) * 0.6);
			 temUIData.visible = true;
			 m_mtmapUIData.insert(std::make_pair(it->first, temUIData));
			 ++m_progress;
		 }		 
	}

	// 4. 保存当前值
	if (m_progress == vecGoldNum.size()) // 完成进度
		m_iGoldNum = iGoldNum;	
	m_oldGoldNumsize = vecGoldNum.size();
}

void CGUI::ChangeItemCount(const unsigned int itemID, int deltaNumb)
{
	UIData temUIData;
	if (deltaNumb == 0)
		return;
	
	else if (deltaNumb > 0)
	{
		for (int i = 1; i <= deltaNumb; i++)
		{
			temUIData = (--m_mtmapUIData.upper_bound(itemID))->second;

			if (itemID == HP_HALF)
				temUIData.x = (--m_mtmapUIData.upper_bound(HP_FULL))->second.x;

			temUIData.x += BLOCKLENGTH;
			temUIData.visible = true;
			m_mtmapUIData.insert(std::make_pair(itemID, temUIData));	
		}
	}

	else if (deltaNumb < 0)
	{
		for (int i = 1; i <= (deltaNumb * -1); i++)
		{
			UIBlockDataIt beg(m_mtmapUIData.lower_bound(itemID)), 
				end(m_mtmapUIData.upper_bound(itemID));
			if (beg != end) m_mtmapUIData.erase(--end);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CGUI::Render()
{
	if(NULL == m_pSprite) return;

	D3DXMATRIX matUIBrush;

	for (UIBlockDataIt it = m_mtmapUIData.begin(); it != m_mtmapUIData.end(); it++)
	{
		std::pair<UIBlockDataIt,UIBlockDataIt> pUI(m_mtmapUIData.equal_range(it->first));
		UIBlockDataIt itUI(pUI.first);
		
		while(itUI != pUI.second)
		{
			if (!itUI->second.visible) { ++itUI; continue;}		//动态隐藏
			
			D3DXMatrixTranslation(&matUIBrush, (float)itUI->second.x, (float)itUI->second.y, m_fLayerDapth);
			m_pSprite->SetTransform(&matUIBrush);
			m_pSprite->Draw(m_Texture, &itUI->second.rect, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

			++itUI;
		}
	}
}

