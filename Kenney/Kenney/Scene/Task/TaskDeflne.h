#ifndef KENNEY_TASKDEFLNE
#define KENNEY_TASKDEFLNE

// 任务类型
enum ETaskType
{
	ETT_GameTask = 0,	//不指定特定ID的任务类型
	ETT_KillMonster = 1,//需要指定特定ID的任务类型
	ETT_Gather = 2,
	ETT_Dialogue = 3
};
// 玩家针对对NPC任务
enum ENPCDialogTaskState
{
	EDTS_RegainFinish,		// 完成"交接任务方"任务
	EDTS_ProvideUnRegain,	// 未交接"发布任务方"任务k
	EDTS_RegainUnFinish,	// 未完成"交接任务方"任务
	EDTS_Provide			// 未接"发布任务方"任务
};

// ItemsID在任务编辑器意义为从NPC处获取的物品
// 在完整的任务系统中应包括接任务获取任务物品、
// 交任务获取奖励物品、打怪获取任务物品、领取
// 活动物品等多类形式,此处简单处理,一种物品多
// 种用途:
// 1、送物品任务:如果FromNpcID与"发布任务方"NPC相同且ItemsID为有效值则为接任务获取任务物品
//				 如果FromNpcID与"交接任务方"NPC相同且ItemsID为有效值则为交任务获取奖励物品,至于任务物品可通过商店、采集等方式获得
// 2、打怪任务:FromNpcID定义为Monster的ID,如果击杀成功就进行任务计数,如果对应的ItemsID为有效值,就进行掉落物品处理
struct TaskPack
{
	char Name[MAX_PATH];			//任务名称
	int	 TaskID;            		//任务编号
	int  TaskType;          		//任务类型
	int  ProvideNpcID;      		//任务发放npc的id
	int  ProvideNpcMapID;			//任务发放npc所在的地图
	int  RegainNpcID;       		//任务提交npc的id
	int  RegainNpcMapID;			//任务提交npc所在的地图
	int  LevelID;					//任务所属地图编号
	int  ItemsID;					//任务物品ID
	int  IteamsCount;				//物品获取数量
	char Info[MAX_PATH];         	//任务描述
	char Receipt[MAX_PATH];      	//任务接受NPC感谢对话
	char Accept[MAX_PATH];       	//接受任务提示语
	char Refuse[MAX_PATH];       	//拒绝任务提示语
	char Repeat[MAX_PATH];       	//已经接受任务提示语
	char Unfinished[MAX_PATH];   	//不满足条件任务交接npc说的话
	char Finished[MAX_PATH];     	//完成任务时任务交接npc说的话
	int	 Stars;						//任务获得金星
	int	 Gold;              		//任务获得金币
	int	 Silver;            		//任务获得银币
	int  Copper;            		//任务获得铜币
};

#endif