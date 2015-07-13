#ifndef KENNEY_TASKMANAGER
#define KENNEY_TASKMANAGER

#include "TaskDeflne.h"

class CTaskManager
{
public:
	static CTaskManager* GetInstance(){ return &m_sTaskManager; }
public:
	CTaskManager(void);
	~CTaskManager(void);
	bool Init();
	void UpdataKillMonster(float fElapsedTime);
	void UpdataGather(float fElapsedTime);
	void UpdataDialogue(float fElapsedTime);
	inline TaskPack& GetTaskPack( short id ){ return m_mapTask[id]; }
private:
	void LoadTaskList(const char* data);
	void LoadTaskData(const char* data);
	void Destroy();

private:
	typedef std::map<int,TaskPack> mapTask;
	mapTask m_mapTask;
private:
	static CTaskManager m_sTaskManager;
};
#endif