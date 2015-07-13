#include "StdAfx.h"

#include "Bag.h"

CBag::CBag():
m_iMoney(0), m_iPerson(EIP_None)
{

}

CBag::~CBag()
{

}

bool CBag::AddPersonByID(unsigned int iID)
{
	return AddPerson(IDToItemPerson(iID));
}

bool CBag::SubtractPersonByID(unsigned int iID)
{
	return SubtractPerson(IDToItemPerson(iID));
}

unsigned int CBag::IDToItemPerson(unsigned int iID)
{
	if (iID < Beige || iID > Yellow) return EIP_None;
	return 1 << (iID-Beige);
}

bool CBag::CheckItemPersonByID(unsigned int iID)
{
	unsigned int ePerson = IDToItemPerson(iID);
	bool bState((ePerson & m_iPerson) > 0);
	return bState;
}

unsigned int CBag::ItemPersonToID(unsigned int ePerson)
{
	if (ePerson > EIP_Yellow) return Yellow;
	unsigned int offset(0);
	while (ePerson = (ePerson >> 1)) ++offset;
	return Beige + offset;
}

unsigned int CBag::GetPersonCount() {
	int cnt(0), nPerson(m_iPerson);
	while (nPerson) { nPerson -= (nPerson & -nPerson); ++cnt; }
	return cnt;
}

bool CBag::AddPerson(unsigned int ePerson)
{
	bool bState((m_iPerson & ePerson) > 0); 
	if (!bState) 
		m_iPerson += ePerson; 
	return !bState;
}

void CBag::SetPerson(unsigned int iID)
{
	unsigned int ePerson = IDToItemPerson(iID);
	m_iPerson = ePerson;
}

bool CBag::SubtractPerson(unsigned int ePerson)
{
	bool bState((m_iPerson & ePerson) > 0); 
	if (!bState) 
		m_iPerson += ePerson; 
	return !bState;
}

bool CBag::AddMoney(unsigned int iMoney) 
{ 
	if(iMoney < 0)
		return false;

	m_iMoney += iMoney; 
	return true;
}

bool CBag::SubtractMoney(unsigned int iMoney) 
{ 
	if(iMoney > m_iMoney)
		return false;

	m_iMoney -= iMoney; 
	return true;
}

