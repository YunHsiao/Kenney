#include "StdAfx.h"
#include "ItemManager.h"
#include "ItemDefine.h"
#include "Item.h"

CItemManager CItemManager::sm_ItemManager;

CItemManager::CItemManager(void)
{

}

CItemManager::~CItemManager(void)
{

}

bool CItemManager::Initialize(const char* cFile)
{
	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(cFile))
	{
		MessageBox(NULL, TEXT("¼ÓÔØÎïÆ·Ê§°Ü£¡£¡£¡"), TEXT(""), MB_OK | MB_ICONERROR);
		return false;
	}

	SItemInfo ItemInfo;
	ZeroMemory(&ItemInfo, sizeof(SItemInfo));
	tinyxml2::XMLElement* pRoot = Doc.FirstChildElement("Item");
	for (tinyxml2::XMLElement* pElement = pRoot->FirstChildElement(); NULL != pElement;pElement = pElement->NextSiblingElement())
	{
		ItemInfo.iTemplateID = atoi(pElement->Attribute("ID"));
		ItemInfo.strName = pElement->Attribute("Name");
		ItemInfo.eItemType = IntToEItemType(atoi(pElement->Attribute("Type")));
		ItemInfo.bPile = atoi(pElement->Attribute("Pile")) != 0;

		m_mapItems[ItemInfo.iTemplateID] = ItemInfo;
	}

	return true;
}

EItemType CItemManager::IntToEItemType(int i)
{
	switch(i)
	{
	case 1:
		return EIT_Money;

	case 2:
		return EIT_BUFF;
	}
	return EIT_Money;
}

CItem* CItemManager::CreateItem(int iTemplateID, int iCount)
{

	SItemInfo* pItemInfo = GetItemInfo(iTemplateID);
	if(NULL == pItemInfo)
		return NULL;
	else
	{
		CItem* pItem = new CItem(pItemInfo, iCount);
		return pItem;
	}
}

SItemInfo* CItemManager::GetItemInfo(int iTemplateID)
{
	if(m_mapItems.end() == m_mapItems.find(iTemplateID))
		return NULL;

	return &m_mapItems[iTemplateID];
}
