#pragma once

enum ERoleType
{
	ERT_Beige,
	ERT_Blue,
	ERT_Green,
	ERT_Pink,
	ERT_Yellow
};

enum EPlayerState
{
	EPS_Stand,
	EPS_Jump,
	EPS_Duck,
	EPS_BeHited,
	EPS_Move,
	EPS_Swim,		//在水中时，可以人物半透
	EPS_Climb,
	EPS_Death
};

enum EPlayerAction
{
	EPA_Stand = 0,
	EPA_Jump,
	EPA_Duck,
	EPA_BeHited,
	EPA_Move1,
	EPA_Move2,
	EPA_Swim1,
	EPA_Swim2,
	EPA_Climb1,
	EPA_Climb2
};