#pragma once

class CGame
{
public:
	CGame(void);
	~CGame(void);

	void	Initialize();
	void	GameRun();
	int		GameExit();

	HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice,const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
	HRESULT OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,	void* pUserContext );
	void	OnLostDevice( void* pUserContext );
	void	OnDestroyDevice( void* pUserContext );

	void	OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
	void	OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

	void	OnGameGUIEvent( UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext );	
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,bool* pbNoFurtherProcessing, void* pUserContext );
	void	KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

protected:
	void InitGame();
	bool Tick(float fElapsedTime);

private:
	CMANGOSceneManager m_SceneManager;
};