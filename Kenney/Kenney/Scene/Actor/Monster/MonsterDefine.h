#ifndef KENNEY_MONSTERDEFINE
#define KENNEY_MONSTERDEFINE

//���������
enum EMonsterType
{
	EMT_Barnacle = 2001,//ʳ�˻�
	EMT_Bee,			//�۷�
	EMT_FishBlue,		//��ɫ����
	EMT_FishGreen,		//��ɫ����
	EMT_FishPink,		//��ɫ����
	EMT_Fly,			//��
	EMT_Frog,			//����
	EMT_Ladybug,		//ư��
	EMT_Mouse,			//����
	EMT_Saw,			//���
	EMT_SawHalf,		//����ľ��
	EMT_SlimeBlock,		//ש��
	EMT_SlimeBlue,		//��ɫŴ��
	EMT_SlimeGreen,		//��ɫŴ��
	EMT_SlimePurple,	//��ɫŴ��
	EMT_Snail,			//��ţ
	EMT_WormGreen,		//��ɫ�����
	EMT_WormPink		//��ɫ�����
};

enum EMonsterAction
{
	EMA_Move = 0,
	EMA_Die,
	EMA_Move2,
	EMA_Hurt
};
#endif