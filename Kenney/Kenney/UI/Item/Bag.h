#ifndef KENNEY_BAG
#define KENNEY_BAG

#include "ItemDefine.h"

class CBag
{
public:
	CBag();
	~CBag();

public:
	bool AddMoney(unsigned int iMoney);
	bool SubtractMoney(unsigned int iMoney);

	void SetPerson(unsigned int iID);
	bool AddPersonByID(unsigned int iID);
	bool SubtractPersonByID(unsigned int iID);

	bool CheckItemPersonByID(unsigned int iID);

public:
	inline unsigned int GetMoney() { return m_iMoney; }
	inline unsigned int GetPerson() { return m_iPerson; }
	unsigned int GetPersonCount();

private:
	bool AddPerson(unsigned int ePerson);
	bool SubtractPerson(unsigned int ePerson);

	unsigned int IDToItemPerson(unsigned int iID);
	unsigned int ItemPersonToID(unsigned int ePerson);

private:
	unsigned int m_iMoney;
	unsigned int m_iPerson;
};

#endif