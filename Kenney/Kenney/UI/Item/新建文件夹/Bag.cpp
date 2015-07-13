#include "StdAfx.h"
#include "Bag.h"
#include "Item.h"
#include "ItemManager.h"

CBag::CBag(void)
{

}

CBag::~CBag(void)
{
	Destroy();
}

void CBag::Destroy()
{
	//DELETE!!!!!
	for (BagIT it = m_vecBag.begin(); it != m_vecBag.end(); it++)
	{
		SAFE_DELETE((*it))
	}
}

bool CBag::AddItem(int iItemID, int iCount)
{
	if(0 >= iCount)
		return false;

	SItemInfo* pItemInfo = CItemManager::GetInstance()->GetItemInfo(iItemID);
	if(NULL == pItemInfo)
		return false;

	bool bResult = false;
	if(pItemInfo->bPile)
	{
		//可堆叠
		CItem* pItem = FindItem(iItemID);
		if(NULL == pItem)
			bResult = AddNewItem(iItemID, iCount);
		else
			pItem->IncreaseItem(iCount);
	}
	else
	{
		//不可堆叠
		if(1 != iCount)
			return false;

		bResult = AddNewItem(iItemID, iCount);
	}

	return bResult;
}

bool CBag::AddNewItem(int iItemID, int iCount)
{
	//添加新物品
	//int iFreeBagPos = GetFreeBagPos();
	//if(0 > iFreeBagPos)
	//	return false;

	CItem* pItem = CItemManager::GetInstance()->CreateItem(iItemID, iCount);
	m_vecBag.push_back(pItem);
	//if(NULL == pItem)
	//	return false;

	//m_vecBag.push_back(pItem);
	pItem->SetBag(this, m_vecBag.size() - 1);
	return true;
}

int CBag::GetFreeBagPos()
{
	for (int i = 0; i < (int)m_vecBag.size(); ++i)
	{
		if(NULL == m_vecBag[i])
			return i;
	}

	return -1;
}

CItem* CBag::GetItemByPos(int iBagPos)
{
	if(0 > iBagPos || (int)m_vecBag.size() <= iBagPos)
		return NULL;

	return m_vecBag[iBagPos];
}

bool CBag::RemoveItemByBagPos(int iBagPos, int iCount)
{
	if(0 >= iCount)
		return false;

	CItem* pItem = GetItemByPos(iBagPos);
	if(NULL == pItem)
		return false;

	if(!pItem->DecreaseItem(iCount))
		return false;

	if(pItem->IsEmpty())
	{
		delete m_vecBag[iBagPos];
		m_vecBag[iBagPos] = NULL;
	}

	return true;
}

bool CBag::RemoveItemByItemID(int iItemID, int iCount)
{
	bool bResult = false;
	for (BagIT it = m_vecBag.begin(); it != m_vecBag.end(); ++it)
	{
		if(NULL == (*it))
			continue;

		if((*it)->GetItemInfo()->iTemplateID == iItemID)
			bResult = RemoveItemByBagPos((*it)->GetItsBagPos(), iCount);
	}

	return bResult;
}

CItem* CBag::FindItem(int iItemID)
{
	for(BagIT it = m_vecBag.begin(); it != m_vecBag.end(); it++)
	{
		if((*it)->GetItemInfo()->iTemplateID == iItemID)
			return *it;
	}

	return NULL;
}

int CBag::GetItemCountByItemID(int iItemID)
{
	int iCount = 0;
	for (BagIT it = m_vecBag.begin(); it != m_vecBag.end(); ++it)
	{
		if(NULL == (*it))
			continue;

		if((*it)->GetItemInfo()->iTemplateID == iItemID)
			iCount += (*it)->GetItemCount();
	}

	return iCount;
}

