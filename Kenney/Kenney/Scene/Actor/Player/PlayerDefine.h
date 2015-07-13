#ifndef KENNY_PLAYER_DEFINE
#define KENNY_PLAYER_DEFINE

enum ERoleType
{	
	ERT_Beige = 3001,
	ERT_Blue = 3002,
	ERT_Green = 3003,
	ERT_Pink = 3004,
	ERT_Yellow = 3005
};

struct SPlayerParameter
{
	int iMaxHP, iRole;
	float fDefAcc, fDefSpe, fJumpHeight, fGravity, fGravityWater;
	std::vector<RECT> vecRole, vecColl;
	std::vector<int*> vecPxl;
};

enum EPlayerAction
{
	EPA_Climb1 = 0,
	EPA_Climb2,
	EPA_Duck,
	EPA_Front,
	EPA_Hurt,
	EPA_Jump,
	EPA_Stand,
	EPA_Swim1,
	EPA_Swim2,
	EPA_Walk1,
	EPA_Walk2
};

enum EGridColl {
	EGC_11 = (1<<0),
	EGC_12 = (1<<1),
	EGC_21 = (1<<2),
	EGC_22 = (1<<3),
	EGC_31 = (1<<4),
	EGC_32 = (1<<5),
	EGC_UP = EGC_11 | EGC_12,
	EGC_MID = EGC_21 | EGC_22,
	EGC_DOWN = EGC_31 | EGC_32,
	EGC_LEFT = EGC_11 | EGC_21 | EGC_31,
	EGC_RIGHT = EGC_12 | EGC_22 | EGC_32,
	EGC_SHORT_LEFT = EGC_11 | EGC_21,
	EGC_SHORT_RIGHT = EGC_12 | EGC_22
};

#endif