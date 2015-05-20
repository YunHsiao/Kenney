#pragma once

#include "Actor.h"
#include "PlayerDefine.h"

//class CController;

class CPlayer : public CActor 
{
public:
	CPlayer(void);
	~CPlayer(void);

public:
	BOOL Initialize(ERoleType eRoleType, D3DXVECTOR3 vecPos);
	void Tick(float fElaspedTime);
	void Render(ID3DXSprite* pSprite, IDirect3DTexture9* pTex);

public:
	inline void SelectRole(){}

private:


private:
	ERoleType m_eRoleType;

	EPlayerState m_eCurState;
	EPlayerAction m_eCurAction;

	int m_iMaxHP;
	int m_iHP;

	std::map<EPlayerAction, RECT>* m_pPlayerAct;
};