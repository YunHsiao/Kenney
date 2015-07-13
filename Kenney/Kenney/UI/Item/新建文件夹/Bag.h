#ifndef KENNEY_BAG
#define KENNEY_BAG

class CPlayer;
class CItem;

class CBag
{
public:
	CBag(void);
	~CBag(void);

public:
	bool AddItem(int iItemID, int iCount);
	CItem* GetItemByPos(int iBagPos);
	int GetItemCountByItemID(int iItemID);
	bool RemoveItemByBagPos(int iBagPos, int iCount);
	bool RemoveItemByItemID(int iItemID, int iCount);

private:
	void Destroy();		//DELETE£¡£¡£¡£¡£¡
	CItem* FindItem(int iItemID);
	bool AddNewItem(int iItemID, int iCount);
	int GetFreeBagPos();

private:
	typedef std::vector<CItem*> Bag;
	typedef Bag::iterator BagIT;
	Bag m_vecBag;
};

#endif
