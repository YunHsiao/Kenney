#pragma once

#include "ActorDefine.h"

class CActor 
{
protected:
	CActor(void);
	virtual ~CActor(void);

public:
	//普通函数
	BOOL Initialize(EActorType eActType, float fMoveSpeed, float fJumpHeight, D3DXVECTOR3 vecPos, bool bMirror = false);

public:
	//inline函数
	inline void SetPosition(D3DXVECTOR3 vecPos){ m_vecPosition = vecPos;}
	inline D3DXVECTOR3* GetPosition(){return &m_vecPosition;}

protected:
	//继承函数


protected:
	//纯虚函数


protected:
	// 角色自身属性及数据
	EActorType m_eActorType;	//有点鸡肋，暂时没发现太大的用途

	EActorState m_eActorState;

	float m_fMoveSpeed;
	float m_fJumpHeight;

	D3DXVECTOR3 m_vecDirection;
	D3DXVECTOR3 m_vecPosition;

	// 图像变换与显示相关
	bool m_bMirror;
	D3DXMATRIX m_matMirror;

	D3DXMATRIX m_matScale;
	D3DXMATRIX m_matRotate;
	D3DXMATRIX m_matTranslate;
	D3DXMATRIX m_matWorld;

	float m_fCurMoveElasped;
	RECT m_srActor;
};