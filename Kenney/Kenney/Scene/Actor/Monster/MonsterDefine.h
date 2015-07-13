#ifndef KENNEY_MONSTERDEFINE
#define KENNEY_MONSTERDEFINE

//怪物的类型
enum EMonsterType
{
	EMT_Barnacle = 2001,//食人花
	EMT_Bee,			//蜜蜂
	EMT_FishBlue,		//蓝色的鱼
	EMT_FishGreen,		//绿色的鱼
	EMT_FishPink,		//粉色的鱼
	EMT_Fly,			//飞
	EMT_Frog,			//青蛙
	EMT_Ladybug,		//瓢虫
	EMT_Mouse,			//老鼠
	EMT_Saw,			//锯齿
	EMT_SawHalf,		//半个的锯齿
	EMT_SlimeBlock,		//砖块
	EMT_SlimeBlue,		//蓝色糯米
	EMT_SlimeGreen,		//绿色糯米
	EMT_SlimePurple,	//紫色糯米
	EMT_Snail,			//蜗牛
	EMT_WormGreen,		//绿色的蠕虫
	EMT_WormPink		//粉色的蠕虫
};

enum EMonsterAction
{
	EMA_Move = 0,
	EMA_Die,
	EMA_Move2,
	EMA_Hurt
};
#endif