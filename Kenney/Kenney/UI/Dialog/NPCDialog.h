#ifndef KENNEY_NPCDIALOG
#define KENNEY_NPCDIALOG

#include "DlgDefine.h"
#include "Scene.h"
#include "TaskDeflne.h"

class CNPCDialog
{
private:
	struct NPCTaskInfo
	{
		int iTaskID;						// ����ID
		ENPCDialogTaskState eTaskState;		// ��ǰ����״̬
	};
public:
	static CNPCDialog* GetInstance(){ return &m_sNPCDialog; }
public:
	CNPCDialog(void);
	~CNPCDialog(void);
public:
	bool Init(CMANGOSceneManager* mgSceneManager);
	void SetVisible(bool visible = false);
	void Destroy();
	void OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext);
	inline void SetNPCID(int id){ m_iNPCID = id; }
	inline int GetNPCID(){ return m_iNPCID;}
private:
	void ObtainNpcTask();
	void FillInfomation();
	void FillTaskInfo(ENPCDialogTaskState state,int id);
	void HideButton();
	void ShowButton(int id);
private:
	int m_iNPCID;
	SMonsterParameter m_sParameter;
	CMANGOSceneManager* m_mgpSceneManager;
	CMANGOScene* m_mgNpcDialog;

	int m_iRemainTaskCount;		// �����ʾ3��������ϰ�ť����
	std::map<int, NPCTaskInfo> m_mapTaskInfo;
	typedef std::map<int, NPCTaskInfo>::iterator taskIt;
private:
	static CNPCDialog m_sNPCDialog;
};
#endif
