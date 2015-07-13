#ifndef KENNEY_ITEM
#define KENNEY_ITEM

#include "ItemDefine.h"

class CBag;

class CItem
{
public:
	CItem(SItemInfo* pItemInfo, int iCount);
	~CItem();

public:
	void SetBag(CBag* pBag, int iBagPos);
	bool IncreaseItem(int iCount);
	bool DecreaseItem(int iCount);

public:
	inline SItemInfo* GetItemInfo() { return m_pItemInfo; }
	inline int GetItemCount(){ return m_iCount; }

	//inline CBag* GetItsBag(){ return m_pBag; }
	inline int GetItsBagPos(){ return m_iBagPos; }
	inline bool IsEmpty() {return 0 == m_iCount; }

private:
	SItemInfo* m_pItemInfo;
	int m_iCount;		//物品的数量	

	//CBag* m_pBag;
	int m_iBagPos;
};

#endif
