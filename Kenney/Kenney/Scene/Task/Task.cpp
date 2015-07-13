#include "StdAfx.h"
#include "Task.h"
#include "TaskManager.h"
#include "TaskDeflne.h"
#include "ActorDefine.h"
#include "Actor.h"
#include "Scene.h"
#include "NPCDialog.h"


CTask CTask::m_sTask;

CTask::CTask(void):
m_TastType(ETT_KillMonster)
,m_iGatherTally(0)
,m_iKillMonsterTally(0)
,m_iTaskId(0)
{

}

CTask::~CTask(void)
{
	Destroy();
}
void CTask::Destroy()
{
	m_NpcProvide.clear();
	m_NpcProvideUnRegain.clear();
	m_NpcRegainFinish.clear();
	m_NpcRegainUnFinish.clear();
	m_mapTally.clear();
}

void CTask::DeleteTask( int id ,std::list< int > *TaskList )
{
	for(taskIt it = TaskList->begin(); it != TaskList->end();++it){
		if( id == TaskList->front()){
			TaskList->pop_front();
			return;
		}else
		if( id == TaskList->back()){
			TaskList->pop_back();
			return;
		}else
		if( *it == id ){
			it = TaskList->erase(it);
			return;
		}
	}
}
void CTask::UpdataKillMonster()
{
	int Sum(0);
	for(taskIt it = m_NpcRegainUnFinish.begin(); it!= m_NpcRegainUnFinish.end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		switch(task.TaskType)
		{
		case ETT_KillMonster:
			for(std::map< int,int>::iterator mit = m_mapTally.begin(); mit != m_mapTally.end(); ++mit){
					if(CScene::GetInstance()->GetLevelID() == task.LevelID){
						if( mit->first == task.ItemsID){
							if(mit->second >= task.IteamsCount){
								//尝试添加任务Task to ProvideUnRegainList
								if(this->AddTask(*it))
									it = m_NpcRegainUnFinish.erase(it);
								return;
							}
						}
					}
					if( ( task.LevelID == 0 ) && ( task.ItemsID == 0) && ( task.IteamsCount != 0)){
						for(std::map< int,int>::iterator mit = m_mapTally.begin(); mit != m_mapTally.end(); ++mit){
							Sum += mit->second;
						}
						if(Sum >= task.IteamsCount){
							if(this->AddTask(*it))
								it = m_NpcRegainUnFinish.erase(it);
							return;
						}

					}
				}

			break;
		case ETT_GameTask:
		case ETT_Gather:
		case ETT_Dialogue:
			break;
		}
	}
}
void CTask::UpdataGather()
{
	for(taskIt it = m_NpcRegainUnFinish.begin(); it!= m_NpcRegainUnFinish.end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		int iGem = CScene::GetInstance()->GetJewel(task.LevelID);//在这里传入了任务地图的ID号,返回Player所吃掉的宝石
		switch(task.TaskType)
		{
		case ETT_Gather:
			if(iGem >= task.IteamsCount){
				if(this->AddTask(*it))
					it = m_NpcRegainUnFinish.erase(it);
				return;
			}
			if( ( task.LevelID == 0 ) && ( task.ItemsID == 0) && ( task.IteamsCount != 0)){
				//获取了背包里金币的数量,判断条件不成立时尝试添加任务到ProvideUnRegainList
				if( (int)CScene::GetInstance()->GetPlayer()->GetBag()->GetMoney() >= task.IteamsCount){
					if(this->AddTask(*it))
						it = m_NpcRegainUnFinish.erase(it);
					return;
				}
			}
			break;
		case ETT_GameTask:
		case ETT_KillMonster:
		case ETT_Dialogue:
			break;
		}
	}
}
void CTask::UpdataDialogue()
{
	for(taskIt it = m_NpcRegainUnFinish.begin(); it!= m_NpcRegainUnFinish.end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		switch(task.TaskType)
		{		
		case ETT_Dialogue:
			//判断当前Npc所在的地图,与任务中做指定的地图是否相同
			if(task.RegainNpcMapID == CScene::GetInstance()->GetLevelID()){
				//Npc是否相同
				if( CNPCDialog::GetInstance()->GetNPCID() >= task.RegainNpcID){
					if(this->AddTask(*it))
						it = m_NpcRegainUnFinish.erase(it);
					return;
				}
			}
			break;
		case ETT_GameTask:
		case ETT_KillMonster:
		case ETT_Gather:
			break;
		}
	}
}

bool CTask::AddTask(int taskid){
	if(m_NpcProvideUnRegain.empty()){
		AddNpcProvideUnRegain(taskid);
		return true;
	}else{
		for(taskIt pit = m_NpcProvideUnRegain.begin(); pit != m_NpcProvideUnRegain.end();++pit){
			if(taskid == *pit)
				return false;
		}
		if(m_NpcRegainUnFinish.size() != m_NpcProvideUnRegain.size()){
			AddNpcProvideUnRegain(taskid);
			return true;
		}
	}
	return false;
}

void CTask::AddTallyMonstrSumToMap(int id)
{
	if(m_mapTally.empty()){
		m_mapTally[id] = 1;
	}else{
		for(std::map< int, int >::iterator it = m_mapTally.begin(); it != m_mapTally.end(); ++it){
			if(id == it->first){
				++it->second;
				return;
			}
		}
		m_mapTally[id] = 1;
	}

}