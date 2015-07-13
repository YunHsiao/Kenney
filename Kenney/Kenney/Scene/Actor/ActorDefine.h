#ifndef KENNEY_ACTOR_DEFINE
#define KENNEY_ACTOR_DEFINE

enum EActorState
{
	EAS_Stand = 1 << 0,
	EAS_Walk = 1 << 1,
	EAS_Duck = 1 << 2,
	EAS_Jump = 1 << 3,
	EAS_Fall = 1 << 4,
	EAS_Climb = 1 << 5,
	EAS_Swim = 1 << 6,		//在水中时，可以人物半透
	EAS_Attack = 1 << 7,
	EAS_Hurt = 1 << 8,
	EAS_Death = 1 << 9,
	EAS_Idle = 1 << 10
};

#endif
