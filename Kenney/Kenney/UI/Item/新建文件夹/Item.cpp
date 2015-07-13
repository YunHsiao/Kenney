#include "StdAfx.h"
#include "Item.h"

CItem::CItem(SItemInfo* pItemInfo, int iCount):
m_pItemInfo(pItemInfo), m_iCount(iCount),
/*m_pBag(NULL), */m_iBagPos(-1)
{

}

CItem::~CItem()
{
	if(NULL != m_pItemInfo)
		m_pItemInfo = NULL;

	//if(NULL != m_pBag)
	//	m_pBag = NULL;
}

void CItem::SetBag(CBag* pBag, int iBagPos)
{
	//m_pBag = pBag;
	m_iBagPos = iBagPos;
}

bool CItem::IncreaseItem(int iCount)
{
	if(0 >= iCount)
		return false;

	m_iCount += iCount;
	return true;
}

bool CItem::DecreaseItem(int iCount)
{
	if(0 >= iCount)
		return false;

	if(m_iCount < iCount)
		return false;

	m_iCount -= iCount;
	return true;
}
