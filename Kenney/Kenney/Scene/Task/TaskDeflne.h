#ifndef KENNEY_TASKDEFLNE
#define KENNEY_TASKDEFLNE

// ��������
enum ETaskType
{
	ETT_GameTask = 0,	//��ָ���ض�ID����������
	ETT_KillMonster = 1,//��Ҫָ���ض�ID����������
	ETT_Gather = 2,
	ETT_Dialogue = 3
};
// �����Զ�NPC����
enum ENPCDialogTaskState
{
	EDTS_RegainFinish,		// ���"��������"����
	EDTS_ProvideUnRegain,	// δ����"��������"����k
	EDTS_RegainUnFinish,	// δ���"��������"����
	EDTS_Provide			// δ��"��������"����
};

// ItemsID������༭������Ϊ��NPC����ȡ����Ʒ
// ������������ϵͳ��Ӧ�����������ȡ������Ʒ��
// �������ȡ������Ʒ����ֻ�ȡ������Ʒ����ȡ
// ���Ʒ�ȶ�����ʽ,�˴��򵥴���,һ����Ʒ��
// ����;:
// 1������Ʒ����:���FromNpcID��"��������"NPC��ͬ��ItemsIDΪ��Чֵ��Ϊ�������ȡ������Ʒ
//				 ���FromNpcID��"��������"NPC��ͬ��ItemsIDΪ��Чֵ��Ϊ�������ȡ������Ʒ,����������Ʒ��ͨ���̵ꡢ�ɼ��ȷ�ʽ���
// 2���������:FromNpcID����ΪMonster��ID,�����ɱ�ɹ��ͽ����������,�����Ӧ��ItemsIDΪ��Чֵ,�ͽ��е�����Ʒ����
struct TaskPack
{
	char Name[MAX_PATH];			//��������
	int	 TaskID;            		//������
	int  TaskType;          		//��������
	int  ProvideNpcID;      		//���񷢷�npc��id
	int  ProvideNpcMapID;			//���񷢷�npc���ڵĵ�ͼ
	int  RegainNpcID;       		//�����ύnpc��id
	int  RegainNpcMapID;			//�����ύnpc���ڵĵ�ͼ
	int  LevelID;					//����������ͼ���
	int  ItemsID;					//������ƷID
	int  IteamsCount;				//��Ʒ��ȡ����
	char Info[MAX_PATH];         	//��������
	char Receipt[MAX_PATH];      	//�������NPC��л�Ի�
	char Accept[MAX_PATH];       	//����������ʾ��
	char Refuse[MAX_PATH];       	//�ܾ�������ʾ��
	char Repeat[MAX_PATH];       	//�Ѿ�����������ʾ��
	char Unfinished[MAX_PATH];   	//�������������񽻽�npc˵�Ļ�
	char Finished[MAX_PATH];     	//�������ʱ���񽻽�npc˵�Ļ�
	int	 Stars;						//�����ý���
	int	 Gold;              		//�����ý��
	int	 Silver;            		//����������
	int  Copper;            		//������ͭ��
};

#endif