#ifndef KENNEY_NPCDEFINE
#define KENNEY_NPCDEFINE

enum ENPCType
{	
	ENT_Yellow = 3001,
	ENT_Pink = 3002,
	ENT_Green = 3003,
	ENT_Blue = 3004,
	ENT_Beige = 3005
};

enum ENPCAction
{
	ENA_Climb1 = 0,
	ENA_Climb2,
	ENA_Duck,
	ENA_Front,
	ENA_Hurt,
	ENA_Jump,
	ENA_Stand,
	ENA_Swim1,
	ENA_Swim2,
	ENA_Walk1,
	ENA_Walk2
};

#define NPC_ActiveDistance 128
#endif