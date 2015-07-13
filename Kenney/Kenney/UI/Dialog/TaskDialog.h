#ifndef KENNEY_TASKDIALOG
#define KENNEY_TASKDIALOG
#include "TaskDeflne.h"

class CTaskDialog
{
public:
	static CTaskDialog* GetInstance(){ return &m_sTskDialog; }
public:
	CTaskDialog(void);
	~CTaskDialog(void);
public:
	bool Init(CMANGOSceneManager* mgSceneManager);
	void SetVisible(bool visible = false);
	void Destroy();
	void ShowTaskDialog( int taskid, ENPCDialogTaskState state );
	void OnGameGUIEvent(UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext);
private:
	void Provide();
	void ProvideUnRegain();
	void RegainUnFinish();
	//void RegainFinish();
	void ProcessProvideConfirm();

	void giveUpTask();
	//void SubmitTask();
private:
	bool m_bButton;
	bool m_bButton2;
	TaskPack m_tPack;
	int m_iID;
	CMANGOSceneManager* m_mgSceneManager;
	ENPCDialogTaskState m_taskState;
	typedef std::vector< int > stateIt;
private:
	static CTaskDialog m_sTskDialog;
};
#endif
