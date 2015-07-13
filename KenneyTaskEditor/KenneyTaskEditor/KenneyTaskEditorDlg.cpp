#include "stdafx.h"
#include "KenneyTaskEditor.h"
#include "KenneyTaskEditorDlg.h"
#include "tinyxml2/tinyxml2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CKenneyTaskEditorDlg::CKenneyTaskEditorDlg(CWnd* pParent)
	: CDialog(CKenneyTaskEditorDlg::IDD, pParent)
{
	//���캯��
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_MapCount = 0;
	m_SeletedID =-1;
	m_SeletedMapID=-1;
}

void CKenneyTaskEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tcElemTree);
	DDX_Control(pDX, IDC_COMBO1, m_cbNPCSend);
	DDX_Control(pDX, IDC_COMBO2, m_cbNPCReceive);
	DDX_Control(pDX, IDC_COMBO3, m_cbTaskType);
	DDX_Control(pDX, IDC_COMBO5, m_cbItemID);
	DDX_Control(pDX, IDC_COMBO4, m_cbNPCSendMapID);
	DDX_Control(pDX, IDC_COMBO6, m_cbNPCReceiveMapID);
	DDX_Control(pDX, IDC_COMBO7, m_cbItemFromMapID);
}

BEGIN_MESSAGE_MAP(CKenneyTaskEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON4, &CKenneyTaskEditorDlg::OnBnNewTask)
	ON_BN_CLICKED(IDC_BUTTON5, &CKenneyTaskEditorDlg::OnBnDeleteTask)
	ON_BN_CLICKED(IDC_BUTTON6, &CKenneyTaskEditorDlg::OnBnSaveTask)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CKenneyTaskEditorDlg::OnTvnSelchangedTree1)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
//	ON_CBN_SELCHANGE(IDC_COMBO1, &CKenneyTaskEditorDlg::OnCbnSelchangeCombo1)
//	ON_CBN_SELCHANGE(IDC_COMBO2, &CKenneyTaskEditorDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()

BOOL CKenneyTaskEditorDlg::OnInitDialog()
{
	m_pImageList = NULL;
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//����ĳ�ʼ��
	//������������������������������������������������������������
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(true);
	m_tcElemTree.ModifyStyle(0xF, TVS_LINESATROOT|TVS_HASLINES|TVS_HASBUTTONS);

	HICON icon[2];
	icon[0]=AfxGetApp()->LoadIcon (IDI_ICON1); // Ŀ¼ͼ��
	icon[1]=AfxGetApp()->LoadIcon (IDI_ICON2); // ����ͼ��

	if(m_pImageList)
		delete m_pImageList;

	m_pImageList = new CImageList;
	m_pImageList->Create(16, 16,  ILC_COLOR24, 2, 2);
	m_pImageList->SetBkColor(RGB(255,255,255));

	for (int i=0; i< 2; ++i)
		m_pImageList->Add(icon[i]);

	m_tcElemTree.SetImageList(m_pImageList, TVSIL_NORMAL);

	//��ʼ����������
	m_cbTaskType.AddString(TEXT("�ۼƼ�������"));
	m_cbTaskType.AddString(TEXT("ָ��ɱ������"));
	m_cbTaskType.AddString(TEXT("ָ���ռ�����"));
	m_cbTaskType.AddString(TEXT("ָ��Npc�Ի�����"));
	//������οؼ��б�
	if(!InitTreeCtrl())
		return FALSE;

	//��ȡNPC�б�
	if(!InitNpcList())
		return FALSE;

	//TODO:��ʼ����Ʒ�б�
	if(!InitItemList())
		return FALSE;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL CKenneyTaskEditorDlg::InitTreeCtrl()
{
	//m_MapCount =0;
	if(FAILED(LoadMapList()))
		return FALSE;

	if(FAILED(LoadTaskBuff()))
		return FALSE;

	UpdateTreeCtrl();
	return TRUE;
}

BOOL CKenneyTaskEditorDlg::LoadMapList()
{
	tinyxml2::XMLDocument pDoc;
	memset(m_sMaplist, 0, sizeof(m_sMaplist));
	memset(&m_sTaskInfo, 0, sizeof(m_sTaskInfo));
	m_tcElemTree.DeleteAllItems();
	m_lTaskBuff.clear();

	char cBuff[64];

	if(pDoc.LoadFile("Task\\TaskList.xml"))
	{
		MessageBox(TEXT("�������б��ļ���û���ҵ�"), TEXT("KenneyTaskEditor"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if(tinyxml2::XMLElement* pRoot = pDoc.FirstChildElement("TASKLIST"))
	{
		for(pRoot=pRoot->FirstChildElement();pRoot;pRoot = pRoot->NextSiblingElement())
		{
			if(!strcmp(pRoot->Value(),"TaskFile"))
			{
				if(pRoot->Attribute("File"))
				{
					strcpy(m_sMaplist[m_MapCount].filepath,pRoot->Attribute("File"));
					sprintf_s(cBuff, 64, "%d", m_MapCount);
					m_cbItemFromMapID.AddString(CU_A2U(cBuff).c_str());
					m_cbNPCReceiveMapID.AddString(CU_A2U(cBuff).c_str());
					m_cbNPCSendMapID.AddString(CU_A2U(cBuff).c_str());
				}
			}
			m_MapCount++;
		}
	}

	return TRUE;
}

BOOL CKenneyTaskEditorDlg::LoadTaskBuff()
{
	tinyxml2::XMLElement* pRoot = NULL;
	tinyxml2::XMLElement* pElement = NULL;

	for(int i = 0; i < m_MapCount; i++)
	{
		tinyxml2::XMLDocument pDoc;

		if(pDoc.LoadFile(m_sMaplist[i].filepath))
		{
			MessageBox(TEXT("�����������ļ���û���ҵ�"), TEXT("����"), MB_OK);
			return FALSE;
		}

		//��ȡ�����б�
		if(pRoot = pDoc.FirstChildElement("TASKSYSTEM"))
		{
			for(pRoot=pRoot->FirstChildElement();pRoot;pRoot = pRoot->NextSiblingElement())
			{
				if(!strcmp(pRoot->Value(),"Task"))
				{
					m_sTaskInfo.MapID = i;
					if(pRoot->Attribute("ID"))
					{
						m_sTaskInfo.TaskID = atoi(pRoot->Attribute("ID"));
					}

					if(pRoot->Attribute("Name"))
					{
						strcpy(m_sTaskInfo.Name,pRoot->Attribute("Name"));
					}

					if(pRoot->Attribute("TaskType"))
					{
						m_sTaskInfo.TaskType = atoi(pRoot->Attribute("TaskType"));
					}

					if(pElement = pRoot->FirstChildElement("ProvideNpcID"))
					{
						m_sTaskInfo.ProvideNpcID = atoi(pElement->GetText());
						if(pElement->Attribute("FromMapID"))
							m_sTaskInfo.ProvideNpcMapID = atoi(pElement->Attribute("FromMapID"));//!!!!!
						//if(pElement->Attribute("Count"))
						//	m_sTaskInfo.ItemsCount = atoi(pElement->Attribute("Count"));
					}

					if(pElement = pRoot->FirstChildElement("RegainNpcID"))
					{
						m_sTaskInfo.RegainNpcID = atoi(pElement->GetText());
						if(pElement->Attribute("FromMapID"))
							m_sTaskInfo.RegainNpcMapID = atoi(pElement->Attribute("FromMapID"));//!!!!!
						//if(pElement->Attribute("Count"))
						//	m_sTaskInfo.ItemsCount = atoi(pElement->Attribute("Count"));
					}

					if(pElement = pRoot->FirstChildElement("ItemsID"))
					{
						m_sTaskInfo.ItemsID = atoi(pElement->GetText());
						if(pElement->Attribute("FromMapID"))
							m_sTaskInfo.ItemsFromMapID = atoi(pElement->Attribute("FromMapID"));//!!!!!
						if(pElement->Attribute("Count"))
							m_sTaskInfo.ItemsCount = atoi(pElement->Attribute("Count"));
					}

					if(pElement = pRoot->FirstChildElement("Info"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Info, "");
						else
							strcpy(m_sTaskInfo.Info,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Receipt"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Receipt, "");
						else
							strcpy(m_sTaskInfo.Receipt,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Accept"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Accept, "");
						else
							strcpy(m_sTaskInfo.Accept,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Refuse"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Refuse, "");
						else
							strcpy(m_sTaskInfo.Refuse,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Repeat"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Repeat, "");
						else
							strcpy(m_sTaskInfo.Repeat,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Unfinished"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Unfinished, "");
						else
							strcpy(m_sTaskInfo.Unfinished,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Finished"))
					{
						if(0 == pElement->GetText())
							strcpy(m_sTaskInfo.Finished, "");
						else
							strcpy(m_sTaskInfo.Finished,pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("Stars"))
					{
						m_sTaskInfo.Stars = atoi(pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("G"))
					{
						m_sTaskInfo.Gold = atoi(pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("S"))
					{
						m_sTaskInfo.Silver = atoi(pElement->GetText());
					}

					if(pElement = pRoot->FirstChildElement("C"))
					{
						m_sTaskInfo.Copper = atoi(pElement->GetText());
					}

					m_lTaskBuff.push_back(m_sTaskInfo);
				}
			}
		}
	}
	return TRUE;
}

void CKenneyTaskEditorDlg::UpdateTreeCtrl()
{
	//ÿ���޸�m_lTaskBuff�󣬵��øú�����������ʾTC�еĸ���
	char cBuff[64];
	m_tcElemTree.DeleteAllItems();
	//��������(��������ַ�����,Ĭ��ͼ������,�����ͼƬ����,����....)
	for (TaskIT it = m_lTaskBuff.begin(); it != m_lTaskBuff.end(); it++)
	{
		sprintf_s(cBuff, 64, "%d", it->MapID);
		HTREEITEM hItem = FindItem(it->MapID);
		if(hItem)
			//��һ��ͬ��
			m_tcElemTree.InsertItem(CU_A2U(it->Name).c_str(), 1, 1, hItem);
		else
		{
			m_tcElemTree.InsertItem(CU_A2U(it->Name).c_str(), 1, 1, 
				m_tcElemTree.InsertItem(CU_A2U(cBuff).c_str(), 0, 0, TVI_ROOT));
		}

		m_tcElemTree.Expand(hItem, TVE_EXPAND);
	}
}

HTREEITEM CKenneyTaskEditorDlg::FindItem(int iMapID)
{
	//����TreeCtrl,����ֵΪiID�Ľڵ㣬��������
	HTREEITEM hItem = m_tcElemTree.GetRootItem();
	CString strItem = TEXT("");

	while(true)
	{
		strItem = m_tcElemTree.GetItemText(hItem);
		if(_ttoi(strItem) == iMapID)
		{
			return hItem;
		}
		else
		{
			if(m_tcElemTree.ItemHasChildren(hItem))
			{
				hItem = m_tcElemTree.GetChildItem(hItem);
			}
			else
			{
				if(m_tcElemTree.GetNextSiblingItem(hItem))
				{
					hItem = m_tcElemTree.GetNextSiblingItem(hItem);
				}
				else
				{
					hItem = m_tcElemTree.GetNextSiblingItem(m_tcElemTree.GetParentItem(hItem));
					if(NULL == hItem)
						return NULL;
				}
			}
		}
	}
}

BOOL CKenneyTaskEditorDlg::InitNpcList()
{
	tinyxml2::XMLDocument pDoc;

	if(pDoc.LoadFile("System\\NPCList.xml"))
	{
		MessageBox(TEXT("��NPC�б��ļ���û���ҵ�"), TEXT("KenneyTaskEdit"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	((CComboBox*)GetDlgItem(IDC_COMBO1))->ResetContent();
	((CComboBox*)GetDlgItem(IDC_COMBO2))->ResetContent();

	if(tinyxml2::XMLElement* pRoot = pDoc.FirstChildElement("NPCLIST"))
	{
		for(pRoot=pRoot->FirstChildElement(); pRoot; pRoot = pRoot->NextSiblingElement())
		{
			if(!strcmp(pRoot->Value(),"NPC"))
			{
				if(pRoot->Attribute("Name"))
				{
					m_cbNPCSend.AddString(CU_A2U(pRoot->Attribute("Name")).c_str());
					m_cbNPCReceive.AddString(CU_A2U(pRoot->Attribute("Name")).c_str());

					m_vNpclist.push_back(CU_A2U(pRoot->Attribute("Name")).c_str());
				}
			}
		}
	}

	return TRUE;
}

BOOL CKenneyTaskEditorDlg::InitItemList()
{
	tinyxml2::XMLDocument pDoc;

	if(pDoc.LoadFile("System\\Item.xml"))
	{
		MessageBox(TEXT("��Item�б��ļ���û���ҵ�"), TEXT("KenneyTaskEdit"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	((CComboBox*)GetDlgItem(IDC_COMBO5))->ResetContent();

	if(tinyxml2::XMLElement* pRoot = pDoc.FirstChildElement("Item"))
	{
		for(pRoot=pRoot->FirstChildElement(); pRoot; pRoot = pRoot->NextSiblingElement())
		{
			if(!strcmp(pRoot->Value(),"Item"))
			{
				if(pRoot->Attribute("Name"))
				{
					//sprintf_s(cBuff, 64, "%d   %d",pRoot->Attribute("Name"), pRoot->Attribute("ID"));
					m_cbItemID.AddString(CU_A2U(pRoot->Attribute("Name")).c_str());
					m_vItemlist.push_back(CU_A2U(pRoot->Attribute("Name")).c_str());
				}
			}
		}
	}

	return TRUE;
}

void CKenneyTaskEditorDlg::OnBnNewTask()
{
	if(!m_tcElemTree.GetSelectedItem())
	{
		MessageBox(TEXT("����ѡ���ͼ"), TEXT("����"), MB_OK | MB_ICONERROR);
		return;
	}

	HTREEITEM hItem = m_tcElemTree.GetParentItem(m_tcElemTree.GetSelectedItem());
	if(NULL == hItem)
	{
		hItem = m_tcElemTree.GetSelectedItem();
	}
	CString strMapID = m_tcElemTree.GetItemText(hItem);

	ClearInfo();

	static int s_iCount = 1;
	char cTemp[64] = "";
	sprintf_s(cTemp, 64, "�½�����%d", s_iCount);
	ZeroMemory(&m_newTask, sizeof(TaskInfo));
	strcpy(m_newTask.Name, cTemp);
	m_newTask.MapID = atoi(CU_U2A(strMapID.GetBuffer(0)).c_str());
	m_lTaskBuff.push_back(m_newTask);
	m_tcElemTree.InsertItem(CU_A2U(cTemp).c_str(), 1, 1, hItem);
	s_iCount++;

	//GetDlgItem(IDC_EDIT20)->SetWindowText(TEXT("�½�����"));

	//CNewTaskDlg NewTaskDlg;
	//if(IDOK == NewTaskDlg.DoModal())
	//{
	//	m_lTaskBuff.push_back(NewTaskDlg.m_newTask);
	//	SaveTask(NewTaskDlg.m_newTask.MapID);

	//	//������οؼ��б�
	//	if(!InitTreeList())
	//		return ;
	//}
}

void CKenneyTaskEditorDlg::OnBnSaveTask()
{
	HTREEITEM hItem = m_tcElemTree.GetSelectedItem();
	//SaveTaskBuff(hItem);

	for (TaskIT it = m_lTaskBuff.begin(); it != m_lTaskBuff.end(); it++)
	{
		if(!strcmp(it->Name, CU_U2A(m_tcElemTree.GetItemText(hItem).GetBuffer()).c_str()))
		{
			CString temp;
			//��������
			GetDlgItem(IDC_EDIT20)->GetWindowText(temp);
			if(temp !="")
			{
				strcpy(it->Name,CU_U2A(temp.GetBuffer()).c_str());
				m_tcElemTree.SetItemText(m_tcElemTree.GetSelectedItem(), CU_A2U(it->Name).c_str());
			}
			else
				MessageBox(TEXT("��������������"),TEXT("����"));

			GetDlgItem(IDC_COMBO3)->GetWindowText(temp);
			if(temp !="")
			{
				if(temp == "�ۼƼ�������")
					it->TaskType = 0;
				else if(temp == "ָ��ɱ������")
					it->TaskType = 1;
				else if(temp == "ָ���ռ�����")
					it->TaskType = 2;
				else if(temp == "ָ��Npc�Ի�����")
					it->TaskType = 3;
			}
			else
				MessageBox(TEXT("��ѡ����������"),TEXT("����"));

			//TODO: �ɷ�NPC ID������NPC ID
			GetDlgItem(IDC_COMBO1)->GetWindowText(temp);
			if(temp !="")
				it->ProvideNpcID = m_cbNPCSend.GetCurSel()+3001;
			else
				MessageBox(TEXT("��ѡ���ɷ�NPC ID"),TEXT("����"));

			GetDlgItem(IDC_COMBO2)->GetWindowText(temp);
			if(temp !="")
				it->RegainNpcID = m_cbNPCReceive.GetCurSel()+3001;
			else
				MessageBox(TEXT("��ѡ�����NPC ID"),TEXT("����"));
			
			//Npc��ͼ
			GetDlgItem(IDC_COMBO4)->GetWindowText(temp);
			if(temp !="")
				it->ProvideNpcMapID = m_cbNPCSendMapID.GetCurSel();
			else
				it->ProvideNpcMapID = 0;

			GetDlgItem(IDC_COMBO6)->GetWindowText(temp);
			if(temp !="")
				it->RegainNpcMapID = m_cbNPCReceiveMapID.GetCurSel();
			else
				it->RegainNpcMapID = 0;

			//������Ʒ
			GetDlgItem(IDC_COMBO5)->GetWindowText(temp);
			if(temp !="")
			{
				if(temp == "ALL")
					it->ItemsID = 0;
				else
					it->ItemsID = m_cbItemID.GetCurSel()+2000;
			}
			else
				MessageBox(TEXT("��ѡ��������ƷID"),TEXT("����"));

			GetDlgItem(IDC_COMBO7)->GetWindowText(temp);
			if(temp !="")
				it->ItemsFromMapID = m_cbItemFromMapID.GetCurSel();
			else
				it->ItemsFromMapID = 0;

			GetDlgItem(IDC_EDIT23)->GetWindowText(temp);	
			if(temp !="")
				it->ItemsCount =atoi(CU_U2A(temp.GetBuffer()).c_str());	
			else
				MessageBox(TEXT("������������Ʒ����"),TEXT("����"));

			//�Ի����
			GetDlgItem(IDC_EDIT6)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Info,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("��������������"),TEXT("����"));

			GetDlgItem(IDC_EDIT5)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Receipt,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("�������������ĸ�л�Ի�"),TEXT("����"));

			GetDlgItem(IDC_EDIT7)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Accept,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("�������������Ի�"),TEXT("����"));

			GetDlgItem(IDC_EDIT8)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Refuse,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("������ܾ�����Ի�"),TEXT("����"));

			//GetDlgItem(IDC_EDIT9)->GetWindowText(temp);
			//if(temp !="")
			//	strcpy(it->Repeat,CU_U2A(temp.GetBuffer()).c_str());
			//else
			//	MessageBox(TEXT("�������Ѿ���������Ի�"),TEXT("����"));

			GetDlgItem(IDC_EDIT10)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Unfinished,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("������δ���������Ի�"),TEXT("����"));

			GetDlgItem(IDC_EDIT12)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Finished,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("�������������Ի�"),TEXT("����"));

			GetDlgItem(IDC_EDIT13)->GetWindowText(temp);
			if(temp !="")
				strcpy(it->Repeat,CU_U2A(temp.GetBuffer()).c_str());
			else
				MessageBox(TEXT("�������������Ի�"),TEXT("����"));


			//������
			GetDlgItem(IDC_EDIT2)->GetWindowText(temp);
			if(temp !="")
				it->Stars = atoi(CU_U2A(temp.GetBuffer()).c_str());
			else
				it->Stars = 0;

			GetDlgItem(IDC_EDIT17)->GetWindowText(temp);
			if(temp !="")
				it->Gold = atoi(CU_U2A(temp.GetBuffer()).c_str());
			else
				it->Gold = 0;

			GetDlgItem(IDC_EDIT18)->GetWindowText(temp);
			if(temp !="")
				it->Silver = atoi(CU_U2A(temp.GetBuffer()).c_str());
			else
				it->Silver = 0;

			GetDlgItem(IDC_EDIT19)->GetWindowText(temp);
			if(temp !="")
				it->Copper = atoi(CU_U2A(temp.GetBuffer()).c_str());
			else
				it->Copper = 0;
		}
	}
	UpdateTreeCtrl();
	//m_tcElemTree.SelectItem(hItem);
	
	//������οؼ��б�
	//if(!InitTreeCtrl())
	//	return;

	//TODO���½��������ܸ��µ�treectrl��Ҳ���ܱ��浽xml
}

void CKenneyTaskEditorDlg::OnBnDeleteTask()
{
	CString TaskName;
	GetDlgItem(IDC_EDIT20)->GetWindowText(TaskName);
	if(TaskName == "")
	{
		MessageBox(TEXT("û��ѡ�е�����Ҫ��ɾ��"), TEXT("ɾ������"), 0);
		return;
	}

	for(TaskIT iter = m_lTaskBuff.begin(); iter != m_lTaskBuff.end(); iter++)
	{
		if(iter->TaskID == m_SeletedID && iter->MapID == m_SeletedMapID)
		{
			m_tcElemTree.DeleteItem(m_tcElemTree.GetSelectedItem());
			m_lTaskBuff.erase(iter);
			ClearInfo();
			break;
		}
	}
}

void CKenneyTaskEditorDlg::SaveDoc()
{
	for(int i = 0; i < m_MapCount; i++)
	{
		tinyxml2::XMLDocument pSaveDoc;

		tinyxml2::XMLElement *pRootElement = pSaveDoc.NewElement("TASKSYSTEM");
		pSaveDoc.InsertEndChild(pRootElement);

		tinyxml2::XMLDeclaration *Declaration = pSaveDoc.NewDeclaration();
		pSaveDoc.InsertFirstChild( Declaration ); 

		TaskIT itbegin = m_lTaskBuff.begin();
		TaskIT itend = m_lTaskBuff.end();

		for (int /*i=0,*/id =0; itbegin != itend; itbegin++/*,i++*/)
		{
			if(itbegin->MapID == i)
			{
				tinyxml2::XMLElement* pTaskElement = pSaveDoc.NewElement("Task");
				pRootElement->LinkEndChild(pTaskElement);

				pTaskElement->SetAttribute("ID",id++);
				pTaskElement->SetAttribute("Name",itbegin->Name);
				pTaskElement->SetAttribute("TaskType",itbegin->TaskType);
				//pTaskElement->SetAttribute("ProvideNpcID",itbegin->ProvideNpcID);
				//pTaskElement->SetAttribute("RegainNpcID",itbegin->RegainNpcID);

				char temp[8];
				tinyxml2::XMLElement* pSubElement = pSaveDoc.NewElement("ProvideNpcID");
				pSubElement->SetAttribute("FromMapID", itbegin->ProvideNpcMapID);
				//pSubElement->SetAttribute("Count",itbegin->ItemsCount);
				sprintf_s(temp,"%d",itbegin->ProvideNpcID);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("RegainNpcID");
				pSubElement->SetAttribute("FromMapID", itbegin->RegainNpcMapID);
				//pSubElement->SetAttribute("Count",itbegin->ItemsCount);
				sprintf_s(temp,"%d",itbegin->RegainNpcID);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("ItemsID");
				pSubElement->SetAttribute("FromMapID", itbegin->ItemsFromMapID);
				pSubElement->SetAttribute("Count",itbegin->ItemsCount);
				sprintf_s(temp,"%d",itbegin->ItemsID);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);

				//pSubElement = pSaveDoc.NewElement("ProvideNpcMapID");
				//sprintf_s(temp,"%d",itbegin->ProvideNpcMapID);
				//pSubElement->SetText(temp);
				//pTaskElement->LinkEndChild(pSubElement);

				//pSubElement = pSaveDoc.NewElement("RegainNpcMapID");
				//sprintf_s(temp,"%d",itbegin->RegainNpcMapID);
				//pSubElement->SetText(temp);
				//pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Info");
				pSubElement->SetText(itbegin->Info);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Receipt");
				pSubElement->SetText(itbegin->Receipt);
				pTaskElement->LinkEndChild(pSubElement); 

				pSubElement = pSaveDoc.NewElement("Accept");
				pSubElement->SetText(itbegin->Accept);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Refuse");
				pSubElement->SetText(itbegin->Refuse);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Repeat");
				pSubElement->SetText(itbegin->Repeat);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Unfinished");
				pSubElement->SetText(itbegin->Unfinished);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Finished");
				pSubElement->SetText(itbegin->Finished);
				pTaskElement->LinkEndChild(pSubElement);

				//pSubElement = pSaveDoc.NewElement("FromMapID");
				//sprintf_s(temp,"%d",itbegin->FromMapID);
				//pSubElement->SetText(temp);
				//pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("Stars");
				sprintf_s(temp,"%d",itbegin->Stars);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("G");
				sprintf_s(temp,"%d",itbegin->Gold);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("S");
				sprintf_s(temp,"%d",itbegin->Silver);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);

				pSubElement = pSaveDoc.NewElement("C");
				sprintf_s(temp,"%d",itbegin->Copper);
				pSubElement->SetText(temp);
				pTaskElement->LinkEndChild(pSubElement);
			}
		}
		pSaveDoc.SaveFile(m_sMaplist[i].filepath);
	}
}

void CKenneyTaskEditorDlg::ClearInfo()
{
	GetDlgItem(IDC_EDIT20)->SetWindowText(L"");		//��������
	GetDlgItem(IDC_COMBO3)->SetWindowText(L"");		//����
	m_cbNPCSend.SetCurSel(CB_ERR);
	m_cbNPCSendMapID.SetCurSel(CB_ERR);
	m_cbNPCReceive.SetCurSel(CB_ERR);
	m_cbNPCReceiveMapID.SetCurSel(CB_ERR);
	//GetDlgItem(IDC_COMBO1)->SetWindowText(L"");		//�ɷ�NPC
	//GetDlgItem(IDC_COMBO2)->SetWindowText(L"");		//����NPC
	//GetDlgItem(IDC_COMBO5)->SetWindowText(L"");		//������Ʒ
	m_cbItemID.SetCurSel(CB_ERR);
	m_cbItemFromMapID.SetCurSel(CB_ERR);
	GetDlgItem(IDC_EDIT23)->SetWindowText(L"");		//��Ʒ����
	//GetDlgItem(IDC_EDIT30)->SetWindowText(L"");		//���񷢲�NPC��ͼID
	//GetDlgItem(IDC_EDIT31)->SetWindowText(L"");		//�������NPC��ͼID
	//GetDlgItem(IDC_EDIT32)->SetWindowText(L"");		//�����ͼID
	//GetDlgItem(IDC_EDIT4)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT6)->SetWindowText(L"");		//��������
	GetDlgItem(IDC_EDIT5)->SetWindowText(L"");		//���������л�Ի�
	GetDlgItem(IDC_EDIT7)->SetWindowText(L"");		//��������Ի�
	GetDlgItem(IDC_EDIT8)->SetWindowText(L"");		//�ܾ�����Ի�
	GetDlgItem(IDC_EDIT10)->SetWindowText(L"");		//�����������Ի�
	GetDlgItem(IDC_EDIT12)->SetWindowText(L"");		//��������л�Ի�
	GetDlgItem(IDC_EDIT13)->SetWindowText(L"");		//�Ѿ���������Ի�
	//GetDlgItem(IDC_EDIT16)->SetWindowText(L"");
	GetDlgItem(IDC_EDIT2)->SetWindowText(L"");		//����
	GetDlgItem(IDC_EDIT17)->SetWindowText(L"");		//���
	GetDlgItem(IDC_EDIT18)->SetWindowText(L"");		//����
	GetDlgItem(IDC_EDIT19)->SetWindowText(L"");		//ͭ��
	//GetDlgItem(IDC_EDIT21)->SetWindowText(L"");
	//GetDlgItem(IDC_EDIT22)->SetWindowText(L"");
}

void CKenneyTaskEditorDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	m_SeletedID =-1;
	char temp[16];
	for(TaskIT iter = m_lTaskBuff.begin(); iter != m_lTaskBuff.end(); iter++)
	{
		//����lTaskBuff��Ԫ�ص�Name��Treectrl�Ľڵ�Ƚϣ��ҳ���ͬ��Ԫ�أ���ͬ�򷵻�0
		if(!strcmp(iter->Name, CU_U2A(m_tcElemTree.GetItemText(pNMTreeView->itemNew.hItem).GetBuffer()).c_str()))
		{
			m_SeletedID = iter->TaskID;
			m_SeletedMapID = iter->MapID;
			GetDlgItem(IDC_EDIT20)->SetWindowText(CU_A2U(iter->Name).c_str());

			if(iter->TaskType == 0)
				GetDlgItem(IDC_COMBO3)->SetWindowText(TEXT("�ۼƼ�������"));
			else if(iter->TaskType == 1)
				GetDlgItem(IDC_COMBO3)->SetWindowText(TEXT("ָ��ɱ������"));
			else if(iter->TaskType == 2)
				GetDlgItem(IDC_COMBO3)->SetWindowText(TEXT("ָ���ռ�����"));
			else if(iter->TaskType == 3)
				GetDlgItem(IDC_COMBO3)->SetWindowText(TEXT("ָ��Npc�Ի�����"));

			m_cbNPCSend.SetCurSel((iter->ProvideNpcID) - 3001);
			m_cbNPCSendMapID.SetCurSel(iter->ProvideNpcMapID);

			m_cbNPCReceive.SetCurSel((iter->RegainNpcID) - 3001);
			m_cbNPCReceiveMapID.SetCurSel(iter->RegainNpcMapID);

			if(iter->ItemsID == 0)
				m_cbItemID.SetCurSel(0);
			else
				m_cbItemID.SetCurSel((iter->ItemsID) - 2000);

			sprintf_s(temp,"%d",iter->ItemsCount);
			GetDlgItem(IDC_EDIT23)->SetWindowText(CU_A2U(temp).c_str());
			m_cbItemFromMapID.SetCurSel(iter->ItemsFromMapID);

			GetDlgItem(IDC_EDIT5)->SetWindowText(CU_A2U(iter->Receipt).c_str());
			GetDlgItem(IDC_EDIT6)->SetWindowText(CU_A2U(iter->Info).c_str());
			GetDlgItem(IDC_EDIT7)->SetWindowText(CU_A2U(iter->Accept).c_str());
			GetDlgItem(IDC_EDIT8)->SetWindowText(CU_A2U(iter->Refuse).c_str());
			GetDlgItem(IDC_EDIT13)->SetWindowText(CU_A2U(iter->Repeat).c_str());

			GetDlgItem(IDC_EDIT10)->SetWindowText(CU_A2U(iter->Unfinished).c_str());
			GetDlgItem(IDC_EDIT12)->SetWindowText(CU_A2U(iter->Finished).c_str());

			sprintf_s(temp,"%d",iter->Stars);
			GetDlgItem(IDC_EDIT2)->SetWindowText(CU_A2U(temp).c_str());
			sprintf_s(temp,"%d",iter->Gold);
			GetDlgItem(IDC_EDIT17)->SetWindowText(CU_A2U(temp).c_str());
			sprintf_s(temp,"%d",iter->Silver);
			GetDlgItem(IDC_EDIT18)->SetWindowText(CU_A2U(temp).c_str());
			sprintf_s(temp,"%d",iter->Copper);
			GetDlgItem(IDC_EDIT19)->SetWindowText(CU_A2U(temp).c_str());
			break;
		}
		else
			ClearInfo();
	}

	*pResult = 0;
}

void CKenneyTaskEditorDlg::OnClose()
{
	// ������ϽǺ��
	int iResoult = MessageBox(TEXT("�Ƿ񱣴浽ԭ�ļ���"), TEXT("KenneyTaskEditor"), MB_YESNOCANCEL | MB_ICONQUESTION);

	switch(iResoult)
	{
	case IDYES:
		SaveDoc();
	case IDNO:
		CDialog::OnClose();
		break;
	}
}

//void CKenneyTaskEditorDlg::OnBnClickedCancel()
//{
//	// ���Ĭ��ȡ����ť
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//
//	OnCancel();
//}

void CKenneyTaskEditorDlg::OnDestroy()
{
	// ��������֮ǰ
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	delete m_pImageList;
}

void CKenneyTaskEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CKenneyTaskEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		m_SeletedID = 1;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CKenneyTaskEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

