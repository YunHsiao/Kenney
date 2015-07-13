#include "StdAfx.h"
#include "TaskDialog.h"
#include "NPCDialog.h"
#include "DlgDefine.h"
#include "TaskDeflne.h"
#include "TaskManager.h"
#include "Task.h"
#include "Scene.h"

CTaskDialog CTaskDialog::m_sTskDialog;

CTaskDialog::CTaskDialog(void):
m_mgSceneManager(NULL)
,m_taskState(EDTS_Provide)
,m_bButton(true)
,m_bButton2(false)
{
	ZeroMemory(&m_tPack,sizeof(TaskPack));
}

CTaskDialog::~CTaskDialog(void)
{
}

void CTaskDialog::Destroy()
{
	m_mgSceneManager = NULL;
}

bool CTaskDialog::Init(CMANGOSceneManager* mgSceneManager)
{
	m_mgSceneManager = mgSceneManager;
	if(m_mgSceneManager == NULL)
		return false;

	if(!m_mgSceneManager->CreateScene("Resource/Layout/TaskDialog.xml")){
		MessageBox( NULL, TEXT(" TaskDialog - 加载 XML 时出现错误 "),NULL,MB_OK );
		return false;
	}

	SetVisible(false);
	return true;
}

void CTaskDialog::SetVisible(bool visible /* = false */)
{
	if(m_mgSceneManager == NULL)
		return;
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->SetVisible(visible);
}

void CTaskDialog::ShowTaskDialog(  int taskid, ENPCDialogTaskState state )
{
	m_iID = taskid;
	switch(state)
	{
	case EDTS_Provide:
		Provide();
		break;
	case EDTS_ProvideUnRegain:
		ProvideUnRegain();
		break;
	case EDTS_RegainUnFinish:
		RegainUnFinish();
		break;
	case EDTS_RegainFinish:
		//RegainFinish(taskid);
		break;
	}
	SetVisible(true);
}

void CTaskDialog::ProvideUnRegain()
{
	TaskPack &pack = CTaskManager::GetInstance()->GetTaskPack(m_iID);
	TCHAR Buffer[MAX_PATH];
	char Reward[MAX_PATH];
	MultiByteToWideChar(0,0,pack.Finished,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Name)->SetText(Buffer);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetText(TEXT("Submit"));
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetText(TEXT("give up"));
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetVisible(true);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetVisible(true);

	pack.Copper += ( pack.Gold * 4 ) + ( pack.Silver * 2 );
	sprintf_s(Reward, MAX_PATH, "奖励: 金币×%d",pack.Copper);
	MultiByteToWideChar(0,0,Reward,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetText(Buffer);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetVisible(true);
	if(m_bButton2){
		if( ( pack.Gold != 0 ) || (pack.Silver != 0) || (pack.Copper != 0) ){
			CScene::GetInstance()->GetPlayer()->GetBag()->AddMoney(pack.Copper);
		}
		SetVisible(false);
		CTask::GetInstance()->DeleteTask(m_iID,CTask::GetInstance()->GetNpcProvideUnRegain());
		CTask::GetInstance()->AddNpcRegainFinish(m_iID);
	}
	m_bButton2 = true;

}

void CTaskDialog::Provide()
{
	TaskPack &pack = CTaskManager::GetInstance()->GetTaskPack(m_iID);
	TCHAR Buffer[MAX_PATH];
	char Reward[MAX_PATH];
	MultiByteToWideChar(0,0,pack.Info,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Name)->SetText(Buffer);

	MultiByteToWideChar(0,0,pack.Accept,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetText(Buffer);

	MultiByteToWideChar(0,0,pack.Refuse,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetText(Buffer);

	if( ( pack.Gold == 0 ) && (pack.Silver == 0) && (pack.Copper == 0)){
		m_bButton = true;
		return;
	}

	pack.Copper += ( pack.Gold * 4 ) + ( pack.Silver * 2 );
	sprintf_s(Reward, MAX_PATH, "奖励: 金币×%d",pack.Copper);
	MultiByteToWideChar(0,0,Reward,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetText(Buffer);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetVisible(true);

	m_bButton = true;
}

void CTaskDialog::RegainUnFinish()
{
	TaskPack &pack = CTaskManager::GetInstance()->GetTaskPack(m_iID);
	TCHAR Buffer[MAX_PATH];
	char Reward[MAX_PATH];
	if( ( pack.ProvideNpcMapID == pack.RegainNpcMapID ) && ( pack.ProvideNpcID == pack.RegainNpcID )){
		MultiByteToWideChar(0,0,pack.Unfinished,-1, Buffer, MAX_PATH);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Name)->SetText(Buffer);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetVisible(false);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetVisible(false);
	}
	if( ( CScene::GetInstance()->GetLevelID() == pack.ProvideNpcMapID ) && (CNPCDialog::GetInstance()->GetNPCID() == pack.ProvideNpcID)){
		MultiByteToWideChar(0,0,pack.Repeat,-1, Buffer, MAX_PATH);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Name)->SetText(Buffer);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetVisible(false);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetVisible(false);
	}

	if( ( CScene::GetInstance()->GetLevelID() == pack.RegainNpcMapID ) && (CNPCDialog::GetInstance()->GetNPCID() == pack.RegainNpcID)){
		MultiByteToWideChar(0,0,pack.Unfinished,-1, Buffer, MAX_PATH);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Name)->SetText(Buffer);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetVisible(false);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetVisible(false);
	}

	if( ( pack.Gold == 0 ) && (pack.Silver == 0) && (pack.Copper == 0)){
		m_bButton = true;
		return;
	}

	pack.Copper += ( pack.Gold * 4 ) + ( pack.Silver * 2 );
	sprintf_s(Reward, MAX_PATH, "奖励: 金币×%d",pack.Copper);
	MultiByteToWideChar(0,0,Reward,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetText(Buffer);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetVisible(true);
}

void CTaskDialog::ProcessProvideConfirm()
{
	TaskPack &pack = CTaskManager::GetInstance()->GetTaskPack(m_iID);
	TCHAR Buffer[MAX_PATH];
	MultiByteToWideChar(0,0,pack.Receipt,-1, Buffer, MAX_PATH);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Name)->SetText(Buffer);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetVisible(false);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetVisible(false);
	m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetLableByID(EL_Reward)->SetVisible(false);
	CTask::GetInstance()->DeleteTask(m_iID,CTask::GetInstance()->GetNpcProvide());
	CTask::GetInstance()->AddNpcRegainUnFinish(m_iID);
	switch(pack.TaskType)
	{
	case ETT_KillMonster:
		CTask::GetInstance()->UpdataKillMonster();
		break;
	case ETT_Dialogue:
		CTask::GetInstance()->UpdataDialogue();
		break;
	case ETT_Gather:
		CTask::GetInstance()->UpdataGather();
		break;
	case ETT_GameTask:
		CTask::GetInstance()->UpdataKillMonster();
		CTask::GetInstance()->UpdataGather();
		break;
	}
	m_bButton = false;
}

void CTaskDialog::giveUpTask(){
	SetVisible(false);
	CTask::GetInstance()->DeleteTask(m_iID,CTask::GetInstance()->GetNpcProvideUnRegain());
	CTask::GetInstance()->DeleteTask(m_iID,CTask::GetInstance()->GetNpcRegainUnFinish());
	CTask::GetInstance()->AddNpcProvide(m_iID);
}
void CTaskDialog::OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext)
{
	switch(nControlID)
	{
	//这里用Task1用来接受按钮
	case EB_Task1:
		m_bButton ? ProcessProvideConfirm() : ProvideUnRegain();
		m_bButton2 = false;
		break;
	//Task2拒绝按钮
	case EB_Task2:
		m_bButton ? SetVisible(false) : giveUpTask();
		break;
	case EB_Quit:
		this->SetVisible(false);
		m_bButton2 = false;
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Quit)->SetVisible(true);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task1)->SetVisible(true);
		m_mgSceneManager->GetSceneByID(ED_TaskDialog)->GetButtonByID(EB_Task2)->SetVisible(true);
		break;
	}
}