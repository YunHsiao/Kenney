#include "..\Mango.h"
#include ".\mangoresourcemanager.h"

//--------------------------------------------------------------------------------------
CMANGOResourceManager::CMANGOResourceManager()
{
	m_pd3dDevice = NULL;
	m_pStateBlock = NULL;
	m_pSprite = NULL;
}


//--------------------------------------------------------------------------------------
CMANGOResourceManager::~CMANGOResourceManager()
{
	int i;
	for( i=0; i < m_FontCache.GetSize(); i++ )
	{
		MANGOFontNode* pFontNode = m_FontCache.GetAt( i );
		SAFE_DELETE( pFontNode );
	}
	m_FontCache.RemoveAll();   

	for( i=0; i < m_TextureCache.GetSize(); i++ )
	{
		MANGOTextureNode* pTextureNode = m_TextureCache.GetAt( i );
		SAFE_DELETE( pTextureNode );
	}
	m_TextureCache.RemoveAll();   

	CUniBuffer::Uninitialize();
	CMANGOIMEEditBox::Uninitialize();
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOResourceManager::OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr = S_OK;
	int i=0;

	m_pd3dDevice = pd3dDevice;

	for( i=0; i < m_FontCache.GetSize(); i++ )
	{
		hr = CreateFont( i );
		if( FAILED(hr) )
			return hr;
	}

	for( i=0; i < m_TextureCache.GetSize(); i++ )
	{
		hr = CreateTexture( i );
		if( FAILED(hr) )
			return hr;
	}

	hr = D3DXCreateSprite( pd3dDevice, &m_pSprite );
	if( FAILED(hr) )
		return MANGO_ERR( L"D3DXCreateSprite", hr );

	// Call CMANGOIMEEditBox's StaticOnCreateDevice()
	// to initialize certain window-dependent data.
	CMANGOIMEEditBox::StaticOnCreateDevice();

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOResourceManager::OnResetDevice()
{
	HRESULT hr = S_OK;

	for( int i=0; i < m_FontCache.GetSize(); i++ )
	{
		MANGOFontNode* pFontNode = m_FontCache.GetAt( i );

		if( pFontNode->pFont )
			pFontNode->pFont->OnResetDevice();
	}

	if( m_pSprite )
		m_pSprite->OnResetDevice();

	IDirect3DDevice9* pd3dDevice = MANGOGetD3DDevice();

	V_RETURN( pd3dDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock ) );

	return S_OK;
}

//--------------------------------------------------------------------------------------
bool CMANGOResourceManager::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Let the CMANGOIMEEditBox's static message proc handle the msg.
	// This is because some IME messages must be handled to ensure
	// proper functionalities and the static msg proc ensures that
	// this happens even if no control has the input focus.
	if( CMANGOIMEEditBox::StaticMsgProc( uMsg, wParam, lParam ) )
		return true;

	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOResourceManager::OnLostDevice()
{
	for( int i=0; i < m_FontCache.GetSize(); i++ )
	{
		MANGOFontNode* pFontNode = m_FontCache.GetAt( i );

		if( pFontNode->pFont )
			pFontNode->pFont->OnLostDevice();
	}

	if( m_pSprite )
		m_pSprite->OnLostDevice();

	SAFE_RELEASE( m_pStateBlock  );
}


//--------------------------------------------------------------------------------------
void CMANGOResourceManager::OnDestroyDevice()
{
	int i=0; 

	m_pd3dDevice = NULL;

	// Release the resources but don't clear the cache, as these will need to be
	// recreated if the device is recreated
	for( i=0; i < m_FontCache.GetSize(); i++ )
	{
		MANGOFontNode* pFontNode = m_FontCache.GetAt( i );
		SAFE_RELEASE( pFontNode->pFont );
	}

	for( i=0; i < m_TextureCache.GetSize(); i++ )
	{
		MANGOTextureNode* pTextureNode = m_TextureCache.GetAt( i );
		SAFE_RELEASE( pTextureNode->pTexture );
	}

	SAFE_RELEASE( m_pSprite );
}


//--------------------------------------------------------------------------------------
bool CMANGOResourceManager::RegisterScene( CMANGOScene *pDialog )
{
	// Check that the dialog isn't already registered.
	for( int i = 0; i < m_Scene.GetSize(); ++i )
		if( m_Scene.GetAt( i ) == pDialog )
			return true;

	// Add to the list.
	if( FAILED( m_Scene.Add( pDialog ) ) )
		return false;

	// Set up next and prev pointers.
	if( m_Scene.GetSize() > 1 )
		m_Scene[m_Scene.GetSize() - 2]->SetNextDialog( pDialog );
	m_Scene[m_Scene.GetSize() - 1]->SetNextDialog( m_Scene[0] );

	return true;
}


//--------------------------------------------------------------------------------------
void CMANGOResourceManager::UnregisterScene( CMANGOScene *pDialog )
{
	// Search for the dialog in the list.
	for( int i = 0; i < m_Scene.GetSize(); ++i )
		if( m_Scene.GetAt( i ) == pDialog )
		{
			m_Scene.Remove( i );
			if( m_Scene.GetSize() > 0 )
			{
				int l, r;

				if( 0 == i )
					l = m_Scene.GetSize() - 1;
				else
					l = i - 1;

				if( m_Scene.GetSize() == i )
					r = 0;
				else
					r = i;

				m_Scene[l]->SetNextDialog( m_Scene[r] );
			}
			return;
		}
}


//--------------------------------------------------------------------------------------
void CMANGOResourceManager::EnableKeyboardInputForAllWindow()
{
	// Enable keyboard input for all registered dialogs
	for( int i = 0; i < m_Scene.GetSize(); ++i )
		m_Scene[i]->EnableKeyboardInput( true );
}


//--------------------------------------------------------------------------------------
int CMANGOResourceManager::AddFont( LPCWSTR strFaceName, LONG height, LONG weight )
{
	// See if this font already exists
	for( int i=0; i < m_FontCache.GetSize(); i++ )
	{
		MANGOFontNode* pFontNode = m_FontCache.GetAt(i);
		size_t nLen = 0;
		StringCchLength( strFaceName, MAX_PATH, &nLen );        
		if( 0 == _wcsnicmp( pFontNode->strFace, strFaceName, nLen ) &&
			pFontNode->nHeight == height &&
			pFontNode->nWeight == weight )
		{
			return i;
		}
	}

	// Add a new font and try to create it
	MANGOFontNode* pNewFontNode = new MANGOFontNode();
	if( pNewFontNode == NULL )
		return -1;

	ZeroMemory( pNewFontNode, sizeof(MANGOFontNode) );
	StringCchCopy( pNewFontNode->strFace, MAX_PATH, strFaceName );
	pNewFontNode->nHeight = height;
	pNewFontNode->nWeight = weight;
	m_FontCache.Add( pNewFontNode );

	int iFont = m_FontCache.GetSize()-1;

	// If a device is available, try to create immediately
	if( m_pd3dDevice )
		CreateFont( iFont );

	return iFont;
}

//--------------------------------------------------------------------------------------
HRESULT CMANGOResourceManager::CreateFont( UINT iFont )
{
	HRESULT hr = S_OK;

	MANGOFontNode* pFontNode = m_FontCache.GetAt( iFont );

	SAFE_RELEASE( pFontNode->pFont );

	V_RETURN( D3DXCreateFont( m_pd3dDevice, pFontNode->nHeight, 0, pFontNode->nWeight, 1, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		pFontNode->strFace, &pFontNode->pFont ) );

	return S_OK;
}


//--------------------------------------------------------------------------------------
int CMANGOResourceManager::AddTexture( LPCWSTR strFilename )
{
	// See if this texture already exists
	for( int i=0; i < m_TextureCache.GetSize(); i++ )
	{
		MANGOTextureNode* pTextureNode = m_TextureCache.GetAt(i);
		size_t nLen = 0;
		StringCchLength( strFilename, MAX_PATH, &nLen );        
		if( pTextureNode->bFileSource &&  // Sources must match
			0 == _wcsnicmp( pTextureNode->strFilename, strFilename, nLen ) )
			return i;
	}

	// Add a new texture and try to create it
	MANGOTextureNode* pNewTextureNode = new MANGOTextureNode();
	if( pNewTextureNode == NULL )
		return -1;

	ZeroMemory( pNewTextureNode, sizeof(MANGOTextureNode) );
	pNewTextureNode->bFileSource = true;
	StringCchCopy( pNewTextureNode->strFilename, MAX_PATH, strFilename );

	m_TextureCache.Add( pNewTextureNode );

	int iTexture = m_TextureCache.GetSize()-1;

	// If a device is available, try to create immediately
	if( m_pd3dDevice )
		CreateTexture( iTexture );

	return iTexture;
}


//--------------------------------------------------------------------------------------
int CMANGOResourceManager::AddTexture( LPCWSTR strResourceName, HMODULE hResourceModule )
{
	// See if this texture already exists
	for( int i=0; i < m_TextureCache.GetSize(); i++ )
	{
		MANGOTextureNode* pTextureNode = m_TextureCache.GetAt(i);
		if( !pTextureNode->bFileSource &&      // Sources must match
			pTextureNode->hResourceModule == hResourceModule ) // Module handles must match
		{
			if( IS_INTRESOURCE( strResourceName ) )
			{
				// Integer-based ID
				if( (INT_PTR)strResourceName == pTextureNode->nResourceID )
					return i;
			}
			else
			{
				// String-based ID
				size_t nLen = 0;
				StringCchLength( strResourceName, MAX_PATH, &nLen );        
				if( 0 == _wcsnicmp( pTextureNode->strFilename, strResourceName, nLen ) )
					return i;
			}
		}
	}

	// Add a new texture and try to create it
	MANGOTextureNode* pNewTextureNode = new MANGOTextureNode();
	if( pNewTextureNode == NULL )
		return -1;

	ZeroMemory( pNewTextureNode, sizeof(MANGOTextureNode) );
	pNewTextureNode->hResourceModule = hResourceModule;
	if( IS_INTRESOURCE( strResourceName ) )
	{
		pNewTextureNode->nResourceID = (int)(size_t)strResourceName;
	}
	else
	{
		pNewTextureNode->nResourceID = 0;
		StringCchCopy( pNewTextureNode->strFilename, MAX_PATH, strResourceName );
	}

	m_TextureCache.Add( pNewTextureNode );

	int iTexture = m_TextureCache.GetSize()-1;

	// If a device is available, try to create immediately
	if( m_pd3dDevice )
		CreateTexture( iTexture );

	return iTexture;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOResourceManager::CreateTexture( UINT iTexture )
{
	HRESULT hr = S_OK;

	MANGOTextureNode* pTextureNode = m_TextureCache.GetAt( iTexture );


	D3DXIMAGE_INFO info;

	if( !pTextureNode->bFileSource )
	{
		if( pTextureNode->nResourceID == 0xFFFF && pTextureNode->hResourceModule == (HMODULE) 0xFFFF )
		{
			//hr = MANGOCreateGUITextureFromInternalArray( m_pd3dDevice, &pTextureNode->pTexture, &info );
			//if( FAILED(hr) )
			//	return DXTRACE_ERR( L"D3DXCreateTextureFromFileInMemoryEx", hr );
		}
		else
		{
			LPCWSTR pID = pTextureNode->nResourceID ? (LPCWSTR)(size_t)pTextureNode->nResourceID : pTextureNode->strFilename;

			// Create texture from resource
			hr =  D3DXCreateTextureFromResourceEx( m_pd3dDevice, pTextureNode->hResourceModule, pID, 0, D3DX_DEFAULT, 
				1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
				D3DX_DEFAULT, D3DX_DEFAULT, 0, 
				&info, NULL, &pTextureNode->pTexture );
			if( FAILED(hr) )
				return DXTRACE_ERR( L"D3DXCreateTextureFromResourceEx", hr );
		}
	}
	else
	{
		// Make sure there's a texture to create
		if( pTextureNode->strFilename[0] == 0 )
			return S_OK;

		// Find the texture on the hard drive
		WCHAR strPath[MAX_PATH];
		hr = MANGOFindDXSDKMediaFileCch( strPath, MAX_PATH, pTextureNode->strFilename );
		if( FAILED(hr) )
		{
			return DXTRACE_ERR( L"MANGOFindDXSDKMediaFileCch", hr );
		}

		// Create texture from file
		hr =  D3DXCreateTextureFromFileEx( m_pd3dDevice, strPath, D3DX_DEFAULT, D3DX_DEFAULT, 
			1, 0, D3DFMT_DXT4, D3DPOOL_MANAGED, 
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, 
			&info, NULL, &pTextureNode->pTexture );
		
		if( FAILED(hr) )
		{
			return DXTRACE_ERR( L"D3DXCreateTextureFromFileEx", hr );
		}
	}

	// Store dimensions
	pTextureNode->dwWidth = info.Width;
	pTextureNode->dwHeight = info.Height;

	return S_OK;
}
