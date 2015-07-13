#ifndef __KENNEY_TASKEDITOR_DLG_H__
#define __KENNEY_TASKEDITOR_DLG_H__

#include "afxcmn.h"
#include "afxwin.h"

#include "TaskDefine.h"

#define MAXTASK 128

// CKenneyTaskEditorDlg 对话框
class CKenneyTaskEditorDlg : public CDialog
{
// 构造

public:
	CKenneyTaskEditorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_KenneyTaskEditor_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual		BOOL OnInitDialog();

public:
	BOOL InitTreeCtrl();
	BOOL InitNpcList();
	BOOL InitItemList();

	BOOL LoadMapList();
	BOOL LoadTaskBuff();
	HTREEITEM FindItem(int iMapID);

	void ClearInfo();
	//void SaveTaskBuff(HTREEITEM hItem);
	void SaveDoc();
	void UpdateTreeCtrl();

private:
	HICON		m_hIcon;

	SMapList	m_sMaplist[256];
	int			m_MapCount;
	int			m_SeletedID;
	int			m_SeletedMapID;

	//int			m_iNpcMaxCount;
	std::vector<CString> m_vNpclist;
	std::vector<CString> m_vItemlist;

	TaskInfo	m_sTaskInfo;
	TaskInfo	m_newTask;

	std::list<TaskInfo> m_lTaskBuff;
	typedef std::list<TaskInfo>::iterator TaskIT;

public:
	afx_msg void OnBnNewTask();
	afx_msg void OnBnDeleteTask();
	afx_msg void OnBnSaveTask();

	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);

	//关闭步骤1.2.3
	afx_msg void OnClose();					//1
	//afx_msg void OnBnClickedCancel();		//2
	afx_msg void OnDestroy();				//3
	afx_msg void OnCbnSelchangeCombo4();

	CTreeCtrl m_tcElemTree;
	CImageList* m_pImageList;
	CComboBox m_cbTaskType;

	CComboBox m_cbNPCSend;
	CComboBox m_cbNPCSendMapID;

	CComboBox m_cbNPCReceive;
	CComboBox m_cbNPCReceiveMapID;

	CComboBox m_cbItemID;
	CComboBox m_cbItemFromMapID;
//	afx_msg void OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnCbnSelchangeCombo1();
//	afx_msg void OnCbnSelchangeCombo2();
};

#endif