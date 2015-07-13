#ifndef __KENNEY_TASKDEFINE_H__
#define __KENNEY_TASKDEFINE_H__



struct SMapList
{
	//char name[32];
	char filepath[64];
	int	 MapID;
};

enum ETaskType
{
	ETT_Normal = 1,
	ETT_Gem = 2,
	ETT_Other = 3
};

enum ETaskState
{
	ETS_NOPROVIDE,        		//任务未接
	ETS_PROVIDE,          		//任务已接正在完成
	ETS_UNFINISHED,       		//任务不满足条件
	ETS_FINISHED,         		//任务交接
	ETS_OVER              		//任务完成
};

struct TaskInfo
{
	char Name[64];          	//任务名称
	int	 TaskID;            	//任务编号
	int  TaskType;          	//任务类型
	int  ProvideNpcID;      	//任务发放npc的id
	int	 ProvideNpcMapID;		//任务发放npc地图Id

	int  RegainNpcID;       	//任务交接npc的id
	int  RegainNpcMapID;		//任务交接npc地图id
	int  ItemsID;				//任务物品ID
	int  ItemsFromMapID;				//任务地图ID
	int  ItemsCount;			//物品获取数量

	char Info[256];         	//任务描述
	char Receipt[256];      	//任务接受NPC感谢对话
	char Accept[256];       	//接受任务提示语
	char Refuse[256];       	//拒绝任务提示语
	char Repeat[256];       	//已经接受任务提示语
	char Unfinished[256];   	//不满足条件任务交接npc说的话
	char Finished[256];     	//完成任务时任务交接npc说的话

	int	 Stars;					//任务获得金星
	int	 Gold;              	//任务获得金币
	int	 Silver;            	//任务获得银币
	int  Copper;            	//任务获得铜币

	int  MapID;             	//任务所属地图编号
	//ETaskState eCurState;		//任务状态
};

#endif
