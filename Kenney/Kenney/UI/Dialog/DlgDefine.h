#ifndef KENNEY_DIALOGDEFINE
#define KENNEY_DIALOGDEFINE

//enum Type{
//	ET_Dialog = 1
//};

enum Define{
	ED_NPCDialog = 1,
	ED_TaskDialog,
	ED_ShopDialog,
	ED_Goods = 7
};

enum Label{
	EL_Name = 1201,
	EL_Info,
	EL_Reward = 1251
};

enum ButtonID{
	EB_Task1 = 1301,
	EB_Task2,
	EB_Task3,
	EB_Shop = 1401,
	EB_Quit = 1501
};


//// 玩家针对对话NPC任务状态
//enum ENPCDialogTaskState
//{
//	EDTS_RegainFinish,		// 完成"交接任务方"任务
//	EDTS_ProvideUnRegain,	// 未交接"发布任务方"任务
//	EDTS_RegainUnFinish,	// 未完成"交接任务方"任务
//	EDTS_Provide			// 未接"发布任务方"任务
//};

#endif