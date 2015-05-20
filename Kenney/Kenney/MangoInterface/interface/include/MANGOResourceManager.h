#ifndef MANGO_GUI_RESMANAGER_H
#define MANGO_GUI_RESMANAGER_H

//-----------------------------------------------------------------------------
// Manages shared resources of dialogs
//-----------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------------
// Structs for shared resources
//--------------------------------------------------------------------------------------
struct MANGOTextureNode
{
	bool bFileSource;  // True if this texture is loaded from a file. False if from resource.
	HMODULE hResourceModule;
	int nResourceID;   // Resource ID. If 0, string-based ID is used and stored in strFilename.
	WCHAR strFilename[MAX_PATH];
	IDirect3DTexture9* pTexture;
	DWORD dwWidth;
	DWORD dwHeight;
};

class CMANGOResourceManager
{
public:
	CMANGOResourceManager();
	~CMANGOResourceManager();

	HRESULT     OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT     OnResetDevice();
	void        OnLostDevice();
	void        OnDestroyDevice();
	bool        MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	int AddFont( LPCWSTR strFaceName, LONG height, LONG weight );
	int AddTexture( LPCWSTR strFilename );
	int AddTexture( LPCWSTR strResourceName, HMODULE hResourceModule );

	MANGOFontNode*     GetFontNode( int iIndex )     { return m_FontCache.GetAt( iIndex ); };
	MANGOTextureNode*  GetTextureNode( int iIndex )  { return m_TextureCache.GetAt( iIndex ); };
	IDirect3DDevice9*  GetD3DDevice()                { return m_pd3dDevice; }

	bool RegisterScene( CMANGOScene *pDialog );
	void UnregisterScene( CMANGOScene *pDialog );
	void EnableKeyboardInputForAllWindow();

	// Shared between all dialogs
	IDirect3DStateBlock9* m_pStateBlock;
	ID3DXSprite*          m_pSprite;				  // Sprite used for drawing

	CMangoArray< CMANGOScene* > m_Scene;              // Dialogs registered

protected:
	CMangoArray< MANGOTextureNode* > m_TextureCache;   // Shared textures
	CMangoArray< MANGOFontNode* >	 m_FontCache;      // Shared fonts

	IDirect3DDevice9* m_pd3dDevice;

	// Resource creation helpers
	HRESULT CreateFont( UINT index );
	HRESULT CreateTexture( UINT index );
};

#endif