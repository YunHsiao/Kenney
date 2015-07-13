#ifndef KENNEY_ITEMMANAGER
#define KENNEY_ITEMMANAGER

#include "ItemDefine.h"

class CItem;

class CItemManager
{
public:
	CItemManager(void);
	~CItemManager(void);

public:
	static CItemManager* GetInstance(){ return &sm_ItemManager; }

public:
	bool Initialize(const char* cFile);
	CItem* CreateItem(int iTemplateID, int iCount = 1);		//NEW£¡£¡£¡£¡£¡
	SItemInfo* GetItemInfo(int iTemplateID);

private:
	EItemType CItemManager::IntToEItemType(int i);

private:
	typedef std::map<int, SItemInfo> Items;
	typedef Items::iterator ItemsIT;
	Items m_mapItems;

private:
	static CItemManager sm_ItemManager;
};

#endif
