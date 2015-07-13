#ifndef KENNEY_TASK
#define KENNEY_TASK

#include "TaskDeflne.h"
#include "Actor.h"

class CTask
{
public:
	static CTask* GetInstance(){ return &m_sTask; }
public:
	CTask(void);
	~CTask(void);
public:
	inline void AddNpcRegainFinish( int state){ m_NpcRegainFinish.push_back(state);}
	inline void AddNpcProvideUnRegain( int state){ m_NpcProvideUnRegain.push_back(state);}
	inline void AddNpcRegainUnFinish( int state){ m_NpcRegainUnFinish.push_back(state);}
	inline void AddNpcProvide( int state){ m_NpcProvide.push_back(state);}
	void AddTallyMonstrSumToMap(int id);
	inline std::list< int > *GetNpcProvide(){ return &m_NpcProvide; }
	inline std::list< int > *GetNpcProvideUnRegain(){ return &m_NpcProvideUnRegain; }
	inline std::list< int > *GetNpcRegainFinish(){ return &m_NpcRegainFinish; }
	inline std::list< int > *GetNpcRegainUnFinish(){ return &m_NpcRegainUnFinish; }
	/*void AddTally( int id);*/
	void DeleteTask( int id ,std::list< int > *TaskList );
	void UpdataKillMonster();
	void UpdataGather();
	void UpdataDialogue();
private:
	bool AddTask(int taskid);
	void Destroy();
private:
	std::list< int > m_NpcRegainFinish;		//完成
	std::list< int > m_NpcProvideUnRegain;	//未交接
	std::list< int > m_NpcRegainUnFinish;	//未完成
	std::list< int > m_NpcProvide;			//未接
	std::map< int, int > m_mapTally;
	ETaskType m_TastType;
	int m_iKillMonsterTally;
	int m_iGatherTally;
	int m_iTaskId;

	typedef std::list< int >::iterator taskIt;

private:
	static CTask m_sTask;
};
#endif