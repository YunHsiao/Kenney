#ifndef __KENNEY_TASKDEFINE_H__
#define __KENNEY_TASKDEFINE_H__



struct SMapList
{
	//char name[32];
	char filepath[64];
	int	 MapID;
};

enum ETaskType
{
	ETT_Normal = 1,
	ETT_Gem = 2,
	ETT_Other = 3
};

enum ETaskState
{
	ETS_NOPROVIDE,        		//����δ��
	ETS_PROVIDE,          		//�����ѽ��������
	ETS_UNFINISHED,       		//������������
	ETS_FINISHED,         		//���񽻽�
	ETS_OVER              		//�������
};

struct TaskInfo
{
	char Name[64];          	//��������
	int	 TaskID;            	//������
	int  TaskType;          	//��������
	int  ProvideNpcID;      	//���񷢷�npc��id
	int	 ProvideNpcMapID;		//���񷢷�npc��ͼId

	int  RegainNpcID;       	//���񽻽�npc��id
	int  RegainNpcMapID;		//���񽻽�npc��ͼid
	int  ItemsID;				//������ƷID
	int  ItemsFromMapID;				//�����ͼID
	int  ItemsCount;			//��Ʒ��ȡ����

	char Info[256];         	//��������
	char Receipt[256];      	//�������NPC��л�Ի�
	char Accept[256];       	//����������ʾ��
	char Refuse[256];       	//�ܾ�������ʾ��
	char Repeat[256];       	//�Ѿ�����������ʾ��
	char Unfinished[256];   	//�������������񽻽�npc˵�Ļ�
	char Finished[256];     	//�������ʱ���񽻽�npc˵�Ļ�

	int	 Stars;					//�����ý���
	int	 Gold;              	//�����ý��
	int	 Silver;            	//����������
	int  Copper;            	//������ͭ��

	int  MapID;             	//����������ͼ���
	//ETaskState eCurState;		//����״̬
};

#endif
