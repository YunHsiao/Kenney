//-----------------------------------------------------------------------------
// All Windows must be assigned to a dialog, which handles
// input and rendering for the controls.
//-----------------------------------------------------------------------------

#ifndef MANGO_GUI_SCENEMANAGER_H
#define MANGO_GUI_SCENEMANAGER_H
#pragma once
class CMANGOResourceManager;

class CMANGOSceneManager
{
public:
	CMANGOSceneManager(void);
	~CMANGOSceneManager(void);

	CMANGOScene*	CreateScene(char * strName, INT iSceneID = 0);
	HRESULT			OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT			OnResetDevice();
	void			OnLostDevice();
	void			OnDestroyDevice();
	void			SetCallback( PCALLBACKMANGOGUIEVENT pCallback) {m_pCallbackEvent = pCallback ;}

	bool			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void			OnRender( float fElapsedTime );


	void			AddScene(CMANGOScene* pScene) {m_SceneArray.Add(pScene);}
	CMANGOScene*    GetSceneByID(int ID);
	CMANGOScene*    GetSceneByName(WCHAR *name);
	int				GetSceneIDByName(WCHAR *name);

private:
	CMANGOResourceManager		m_pManager;
	CMangoArray< CMANGOScene* > m_SceneArray;
	PCALLBACKMANGOGUIEVENT		m_pCallbackEvent;
};

#endif