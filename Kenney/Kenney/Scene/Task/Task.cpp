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
								//�����������Task to ProvideUnRegainList
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
		int iGem = CScene::GetInstance()->GetJewel(task.LevelID);//�����ﴫ���������ͼ��ID��,����Player���Ե��ı�ʯ
		switch(task.TaskType)
		{
		case ETT_Gather:
			if(iGem >= task.IteamsCount){
				if(this->AddTask(*it))
					it = m_NpcRegainUnFinish.erase(it);
				return;
			}
			if( ( task.LevelID == 0 ) && ( task.ItemsID == 0) && ( task.IteamsCount != 0)){
				//��ȡ�˱������ҵ�����,�ж�����������ʱ�����������ProvideUnRegainList
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
			//�жϵ�ǰNpc���ڵĵ�ͼ,����������ָ���ĵ�ͼ�Ƿ���ͬ
			if(task.RegainNpcMapID == CScene::GetInstance()->GetLevelID()){
				//Npc�Ƿ���ͬ
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