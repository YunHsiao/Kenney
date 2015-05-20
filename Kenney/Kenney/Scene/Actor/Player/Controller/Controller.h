#pragma once

class CPlayer;

class CController
{
public:
	CController(void);
	~CController(void);

public:
	static CController* GetInstance(){return &sm_Controller;}

public:
	BOOL Initialize(CPlayer* pPlayer);
	void Tick(float fElaspedTime);

private:
	void TickKeyboard(float fElaspedTime);
	void TickMouse(float fElaspedTime);

private:
	CPlayer* m_pPlayer;

private:
	static CController sm_Controller;
};
