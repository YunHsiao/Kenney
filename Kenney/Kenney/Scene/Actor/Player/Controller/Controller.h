#ifndef KENNEY_CONTROLLER
#define KENNEY_CONTROLLER

class CPlayer;

enum EKeyState {
	EKS_Left = (1<<0),
	EKS_Right = (1<<1),
	EKS_Up = (1<<2),
	EKS_Down = (1<<3),
	EKS_Jump = (1<<4),
	EKS_ErrH = EKS_Left | EKS_Right,
	EKS_ErrV = EKS_Up | EKS_Down
};

class CController
{
public:
	CController(void);
	~CController(void);

public:
	static CController* GetInstance() { return &sm_Controller; }

public:
	bool Initialize(CPlayer* pPlayer);
	void Tick(float fElaspedTime);

private:
	void TickKeyboard(float fElapsedTime);
	void TickMouse(float fElapsedTime);

private:
	CPlayer* m_pPlayer;

private:
	static CController sm_Controller;
};

#endif