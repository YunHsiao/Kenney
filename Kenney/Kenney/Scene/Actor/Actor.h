#pragma once

#include "ActorDefine.h"

class CActor 
{
protected:
	CActor(void);
	virtual ~CActor(void);

public:
	//��ͨ����
	BOOL Initialize(EActorType eActType, float fMoveSpeed, float fJumpHeight, D3DXVECTOR3 vecPos, bool bMirror = false);

public:
	//inline����
	inline void SetPosition(D3DXVECTOR3 vecPos){ m_vecPosition = vecPos;}
	inline D3DXVECTOR3* GetPosition(){return &m_vecPosition;}

protected:
	//�̳к���


protected:
	//���麯��


protected:
	// ��ɫ�������Լ�����
	EActorType m_eActorType;	//�е㼦�ߣ���ʱû����̫�����;

	EActorState m_eActorState;

	float m_fMoveSpeed;
	float m_fJumpHeight;

	D3DXVECTOR3 m_vecDirection;
	D3DXVECTOR3 m_vecPosition;

	// ͼ��任����ʾ���
	bool m_bMirror;
	D3DXMATRIX m_matMirror;

	D3DXMATRIX m_matScale;
	D3DXMATRIX m_matRotate;
	D3DXMATRIX m_matTranslate;
	D3DXMATRIX m_matWorld;

	float m_fCurMoveElasped;
	RECT m_srActor;
};