#include "StdAfx.h"
#include "TaskManager.h"
#include "TaskDeflne.h"
#include "Task.h"
CTaskManager CTaskManager::m_sTaskManager;

CTaskManager::CTaskManager(void)
{
}

CTaskManager::~CTaskManager(void)
{
	Destroy();
}

void CTaskManager::Destroy()
{
	m_mapTask.clear();
}

bool CTaskManager::Init(){
	LoadTaskList("Resource/Task/TaskList.xml");
	return true;
}
void CTaskManager::LoadTaskList(const char* data)
{
	tinyxml2::XMLDocument Doc;
	if(Doc.LoadFile(data)){
		MessageBox(NULL,TEXT(" TaskManager - 加载资源时出现错误 "),NULL,MB_OK);
		return;
	}
	int id(0);
	char route[MAX_PATH];
	tinyxml2::XMLElement* mark = Doc.FirstChildElement();
	for(mark->FirstChildElement(); NULL != mark; mark = mark->NextSiblingElement())
	{
		for(tinyxml2::XMLElement* content = mark->FirstChildElement();
			NULL != content; content = content->NextSiblingElement())
		{
			if(!strcmp(content->Value(),"Level")){
				id = atoi(content->Attribute("ID"));
				strcpy_s(route,MAX_PATH,content->Attribute("Path"));
				LoadTaskData(route);
			}
		}
	}

}
void CTaskManager::LoadTaskData(const char* data){
	tinyxml2::XMLDocument Doc;

	if(Doc.LoadFile(data)){
		MessageBox(NULL,TEXT(" TaskManager2 - 加载资源时出现错误 "),NULL,MB_OK);
		return;
	}

	TaskPack pack;
	tinyxml2::XMLElement* pElement;
	ZeroMemory(&pack,sizeof(pack));
	if(tinyxml2::XMLElement* mark = Doc.FirstChildElement("TASKSYSTEM")){
	for(tinyxml2::XMLElement* content = mark->FirstChildElement(); NULL != content; content = content->NextSiblingElement()){
		if(!strcmp(content->Value(),"Task")){
			if(content->Attribute("ID")){
				pack.TaskID = atoi(content->Attribute("ID"));
			}
			if(content->Attribute("Name")){
				strcpy_s(pack.Name,MAX_PATH,content->Attribute("Name"));
			}
			if(content->Attribute("TaskType")){
				pack.TaskType = atoi(content->Attribute("TaskType"));
			}
			if(pElement = content->FirstChildElement("ProvideNpcID"))
			{
				pack.ProvideNpcID = atoi(pElement->GetText());
				if(pElement->Attribute("FromMapID"))
					pack.ProvideNpcMapID = atoi(pElement->Attribute("FromMapID"));
			}
			if(pElement = content->FirstChildElement("RegainNpcID"))
			{
				pack.RegainNpcID = atoi(pElement->GetText());
				if(pElement->Attribute("FromMapID"))
					pack.RegainNpcMapID = atoi(pElement->Attribute("FromMapID"));
			}
			if(pElement = content->FirstChildElement("ItemsID")){
				pack.ItemsID = atoi(pElement->GetText());

				if(pElement->Attribute("FromMapID")){
					pack.LevelID = atoi(pElement->Attribute("FromMapID"));
				}
				if(pElement->Attribute("Count")){
					pack.IteamsCount = atoi(pElement->Attribute("Count"));
				}
			}
			if(pElement = content->FirstChildElement("Info")){
				strcpy_s(pack.Info,MAX_PATH,pElement->GetText());
			}

			if(pElement = content->FirstChildElement("Receipt")){
				strcpy_s(pack.Receipt,MAX_PATH,pElement->GetText());
			}
			if(pElement = content->FirstChildElement("Accept")){
				strcpy_s(pack.Accept,MAX_PATH,pElement->GetText());
			}
			if(pElement = content->FirstChildElement("Refuse")){
				strcpy_s(pack.Refuse,MAX_PATH,pElement->GetText());
			}
			if(pElement = content->FirstChildElement("Repeat")){
				strcpy_s(pack.Repeat,MAX_PATH,pElement->GetText());
			}
			if(pElement = content->FirstChildElement("Unfinished")){
				strcpy_s(pack.Unfinished,MAX_PATH,pElement->GetText());
			}
			if(pElement = content->FirstChildElement("Finished")){
				strcpy_s(pack.Finished,MAX_PATH,pElement->GetText());
			}
			if(pElement = content->FirstChildElement("Stars")){
				pack.Stars = atoi(pElement->GetText());
			}
			if(pElement = content->FirstChildElement("G")){
				pack.Gold = atoi(pElement->GetText());
			}
			if(pElement = content->FirstChildElement("S")){
				pack.Silver = atoi(pElement->GetText());
			}
			if(pElement = content->FirstChildElement("C")){
				pack.Copper = atoi(pElement->GetText());
			}

		}
		m_mapTask[pack.TaskID] = pack;
		CTask::GetInstance()->AddNpcProvide(pack.TaskID);
	}
}


}

void CTaskManager::UpdataKillMonster(float fElapsedTime)
{
	CTask::GetInstance()->UpdataKillMonster();
}
void CTaskManager::UpdataGather(float fElapsedTime)
{
	CTask::GetInstance()->UpdataGather();
}
void CTaskManager::UpdataDialogue(float fElapsedTime)
{
	CTask::GetInstance()->UpdataDialogue();
}