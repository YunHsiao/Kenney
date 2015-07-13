#ifndef KENNEY_ITEMDEFINE
#define KENNEY_ITEMDEFINE

enum EItemPerson
{
	EIP_None = 0,
	EIP_Beige = 1 << 0,
	EIP_Blue = 1 << 1,
	EIP_Green = 1 << 2,
	EIP_Pink = 1 << 3,
	EIP_Yellow = 1 << 4,
};

#define Beige	4031
#define Blue	4032
#define Green	4033
#define Pink	4034
#define Yellow	4035

enum EItemType
{
	EIT_Money = 1,
	EIT_BUFF
	//EIT_Monster
};

struct SItemInfo
{
	int iTemplateID;			//物品模板ID
	std::string strName;
	EItemType eItemType;		//物品类型

	//int	iAffectHP;				//影响血量
	//int iAffectDefenc;			//影响防御力
	
	bool bPile;
};

#endif