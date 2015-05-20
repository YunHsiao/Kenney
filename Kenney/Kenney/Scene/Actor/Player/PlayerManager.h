#pragma once

#include "Player.h"

class CPlayerManager
{
public:
	CPlayerManager(void);
	~CPlayerManager(void);

public:
	static CPlayerManager* GetInstance(){return &sm_PlayerManager;}

public:
	BOOL Initialize(ID3DXSprite* pSprite);
	BOOL InitializePlayer(ERoleType eRoleType, D3DXVECTOR3 vecPos);
	void Tick(float fElaspedTime);
	void Render();

public:


private:
	BOOL InitializeResource();

private:
	ID3DXSprite* m_pSprite;
	IDirect3DTexture9* m_pTex;

	std::map<ERoleType, std::map<EPlayerAction, RECT>*> m_mapRoleRes;
	typedef std::map<ERoleType, std::map<EPlayerAction, RECT>*> RoleResIT;

	std::map<EPlayerAction, RECT> m_mapPlayerAct;
	typedef std::map<EPlayerAction, RECT>::iterator PlayerActIT;

	CPlayer* m_pPlayer;

private:
	static CPlayerManager sm_PlayerManager;
};
