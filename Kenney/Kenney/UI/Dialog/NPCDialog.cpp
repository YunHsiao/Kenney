#include "StdAfx.h"
#include "Scene.h"
#include "NPCDialog.h"
#include "DlgDefine.h"
#include "TaskDialog.h"
#include "ShopDialog.h"
#include "TaskManager.h"
#include "Task.h"

CNPCDialog CNPCDialog::m_sNPCDialog;

CNPCDialog::CNPCDialog(void):
m_mgpSceneManager(NULL)
,m_mgNpcDialog(NULL)
,m_iNPCID(0)
,m_iRemainTaskCount(EB_Task1)
{
	ZeroMemory(&m_sParameter,sizeof(SMonsterParameter));
}

CNPCDialog::~CNPCDialog(void)
{
	Destroy();
}

void CNPCDialog::Destroy()
{
	m_mgpSceneManager = NULL;
	m_mgNpcDialog = NULL;
}

bool CNPCDialog::Init(CMANGOSceneManager* mgSceneManager)
{
	m_mgpSceneManager = mgSceneManager;

	if(!m_mgpSceneManager->CreateScene("Resource/Layout/NPCDialog.xml")){
		MessageBox(NULL,TEXT(" NPCDialog - 加载 XML 时出现错误 "),NULL,MB_OK);
		return false;
	}
	m_mgNpcDialog = m_mgpSceneManager->GetSceneByID(ED_NPCDialog);

	SetVisible(false);

	return true;
}

void CNPCDialog::SetVisible(bool visible /* = false */)
{
	if(m_mgNpcDialog == NULL)
		return;
	m_mgNpcDialog->SetVisible(visible);

	if(m_mgNpcDialog->IsVisible())
		FillInfomation();
		ObtainNpcTask();
		m_iRemainTaskCount = EB_Task1;
}

void CNPCDialog::ObtainNpcTask()
{
	HideButton();
	std::list< int > *Provide = CTask::GetInstance()->GetNpcProvide();
	std::list< int > *RegainUnFinish = CTask::GetInstance()->GetNpcRegainUnFinish();
	std::list< int > *ProvideUnRegain = CTask::GetInstance()->GetNpcProvideUnRegain();

	for(std::list< int >::iterator it = ProvideUnRegain->begin(); it != ProvideUnRegain->end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		if(CScene::GetInstance()->GetLevelID() == task.RegainNpcMapID){
			if(m_iNPCID == task.RegainNpcID){
				FillTaskInfo(EDTS_ProvideUnRegain,*it);//未交接
			}
		}
	}
	for(std::list< int >::iterator it = RegainUnFinish->begin(); it != RegainUnFinish->end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		if((task.ProvideNpcMapID != task.RegainNpcMapID) || (task.ProvideNpcID != task.RegainNpcID)){
			if(CScene::GetInstance()->GetLevelID() == task.ProvideNpcMapID){
				if(m_iNPCID == task.ProvideNpcID){
					FillTaskInfo(EDTS_RegainUnFinish,*it);//未完成
				}
			}
		}
	}
	for(std::list< int >::iterator it = RegainUnFinish->begin(); it != RegainUnFinish->end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		if(CScene::GetInstance()->GetLevelID() == task.RegainNpcMapID){
			if(m_iNPCID == task.RegainNpcID){
				FillTaskInfo(EDTS_RegainUnFinish,*it);//未完成
			}
		}
	}
	for(std::list< int >::iterator it = Provide->begin(); it != Provide->end(); ++it){
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(*it);
		if(CScene::GetInstance()->GetLevelID() == task.ProvideNpcMapID){
			if(m_iNPCID == task.ProvideNpcID){
				FillTaskInfo(EDTS_Provide,*it);//未接
			}
		}
	}
	
}

void CNPCDialog::FillTaskInfo(ENPCDialogTaskState state,int id)
{
	if(state == EDTS_RegainFinish ||  EB_Task3 < m_iRemainTaskCount)
		return;
	TCHAR Buffer[MAX_PATH];
	NPCTaskInfo TaskInfo;
		TaskInfo.eTaskState = state;
		TaskInfo.iTaskID = id;
		m_mapTaskInfo[m_iRemainTaskCount] = TaskInfo;
		if(m_iRemainTaskCount > EB_Task3)
			return;
		TaskPack &task = CTaskManager::GetInstance()->GetTaskPack(id);
		MultiByteToWideChar(0,0,task.Name,-1,Buffer,MAX_PATH);
		m_mgNpcDialog->GetButtonByID(m_iRemainTaskCount)->SetText(Buffer);
		ShowButton(m_iRemainTaskCount);
		++m_iRemainTaskCount;
}

void CNPCDialog::FillInfomation()
{
	if(m_mgNpcDialog == NULL)
		return;
	m_sParameter = CScene::GetInstance()->GetmultimapNPCList(CScene::GetInstance()->GetLevelID(),m_iNPCID);
	TCHAR Buffer[MAX_PATH];
	MultiByteToWideChar(0,0,m_sParameter.NPCName,-1, Buffer, MAX_PATH);
	m_mgNpcDialog->GetLableByID(EL_Name)->SetText(Buffer);

	MultiByteToWideChar(0, 0, m_sParameter.NPCDialogInfo, -1, Buffer, MAX_PATH);
	m_mgNpcDialog->GetLableByID(EL_Info)->SetText(Buffer);

}


void CNPCDialog::HideButton()
{
	if(m_mgNpcDialog == NULL)
		return;
	for(int i(EB_Task1); i <= EB_Task3; ++i){
		m_mgNpcDialog->GetButtonByID(i)->SetVisible(false);
	}


}
void CNPCDialog::ShowButton(int id)
{
	if(m_mgNpcDialog == NULL)
		return;
	m_mgNpcDialog->GetButtonByID(id)->SetVisible(true);
}


void CNPCDialog::OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext)
{
	switch(nControlID)
	{
	case EB_Task1:
	case EB_Task2:
	case EB_Task3:
		for(taskIt it = m_mapTaskInfo.begin(); it != m_mapTaskInfo.end(); ++it){
			if(nControlID == it->first){
				this->SetVisible(false);
				CTaskDialog::GetInstance()->ShowTaskDialog(it->second.iTaskID,it->second.eTaskState);
				break;
			}
		}
		break;
	case EB_Shop:
		this->SetVisible(false);
		CShopDialog::GetInstance()->ShowShopDialog(m_iNPCID,true);
		break;
	case EB_Quit:
		SetVisible(false);
		m_mgNpcDialog->GetButtonByID(EB_Task1)->SetVisible(true);
		m_mgNpcDialog->GetButtonByID(EB_Task2)->SetVisible(true);
		m_mgNpcDialog->GetButtonByID(EB_Task3)->SetVisible(true);
		m_mgNpcDialog->GetButtonByID(EB_Shop)->SetVisible(true);
		m_mgNpcDialog->GetButtonByID(EB_Quit)->SetVisible(true);
		break;
	}
}