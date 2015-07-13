#include "..\Mango.h"
#include "MANGOScene.h"
#include "../../../tinyxml2/tinyxml2.h"

double        CMANGOScene::s_fTimeRefresh = 0.0f;
CMANGOControl* CMANGOScene::s_pControlFocus = NULL;        // The control which has focus
CMANGOControl* CMANGOScene::s_pControlPressed = NULL;      // The control currently pressed


struct MANGO_SCREEN_VERTEX
{
	float x, y, z, h;
	D3DCOLOR color;
	float tu, tv;

	static DWORD FVF;
};
DWORD MANGO_SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;


struct MANGO_SCREEN_VERTEX_UNTEX
{
	float x, y, z, h;
	D3DCOLOR color;

	static DWORD FVF;
};
DWORD MANGO_SCREEN_VERTEX_UNTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

//--------------------------------------------------------------------------------------
CMANGOScene::CMANGOScene()
{
	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;

	m_pManager = NULL;
	m_bVisible = true;
	m_bCaption = false;
	m_bMinimized = false;
	m_bDrag = false;
	m_wszCaption[0] = L'\0';
	m_nCaptionHeight = 18;

	m_colorTopLeft = 0;
	m_colorTopRight = 0;
	m_colorBottomLeft = 0;
	m_colorBottomRight = 0;

	m_pCallbackEvent = NULL;
	m_pCallbackEventUserContext = NULL;

	m_fTimeLastRefresh = 0;

	m_pControlMouseOver = NULL;

	m_pNextDialog = this;
	m_pPrevDialog = this;

	m_nDefaultControlID = 0xffff;
	m_bNonUserEvents = false;
	m_bKeyboardInput = false;
	m_bMouseInput = true;
	m_SceneID = 1;
}


//--------------------------------------------------------------------------------------
CMANGOScene::~CMANGOScene()
{
	int i=0;

	RemoveAllControls();

	m_Fonts.RemoveAll();
	m_Textures.RemoveAll();

	for( i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		MANGOElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
		SAFE_DELETE( pElementHolder );
	}

	m_DefaultElements.RemoveAll();
}


//--------------------------------------------------------------------------------------
void CMANGOScene::Init( CMANGOResourceManager* pManager, bool bRegisterDialog ,char*  psCodexFilename )
{
	m_pManager = pManager;
	if( bRegisterDialog )
		pManager->RegisterScene( this );

	SetTexture( 0, MAKEINTRESOURCE(0xFFFF), (HMODULE)0xFFFF );
	InitDefaultElements(psCodexFilename);
}

//--------------------------------------------------------------------------------------
void CMANGOScene::Init( CMANGOResourceManager* pManager, bool bRegisterDialog, LPCWSTR pszControlTextureFilename ,char*  psCodexFilename )
{
	m_pManager = pManager;
	if( bRegisterDialog )
		pManager->RegisterScene( this );
	SetTexture( 0, pszControlTextureFilename );
	InitDefaultElements(psCodexFilename);
}

//--------------------------------------------------------------------------------------
void CMANGOScene::Init( CMANGOResourceManager* pManager, bool bRegisterDialog, LPCWSTR szControlTextureResourceName, HMODULE hControlTextureResourceModule  ,char*  psCodexFilename)
{
	m_pManager = pManager;
	if( bRegisterDialog )
		pManager->RegisterScene( this );

	SetTexture( 0, szControlTextureResourceName, hControlTextureResourceModule );
	InitDefaultElements(psCodexFilename);
}


//--------------------------------------------------------------------------------------
void CMANGOScene::SetCallback( PCALLBACKMANGOGUIEVENT pCallback, void* pUserContext )
{
	// If this assert triggers, you need to call CMANGOScene::Init() first.  This change
	// was made so that the MANGO's GUI could become seperate and optional from MANGO's core.  The 
	// creation and interfacing with CMANGOSceneResourceManager is now the responsibility 
	// of the application if it wishes to use MANGO's GUI.
	assert( m_pManager != NULL && L"To fix call CMANGOScene::Init() first.  See comments for details." ); 

	m_pCallbackEvent = pCallback; 
	m_pCallbackEventUserContext = pUserContext; 
}


//--------------------------------------------------------------------------------------
void CMANGOScene::RemoveControl( int ID )
{
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt( i );
		if( pControl->GetID() == ID )
		{
			// Clean focus first
			ClearFocus();

			// Clear references to this control
			if( s_pControlFocus == pControl )
				s_pControlFocus = NULL;
			if( s_pControlPressed == pControl )
				s_pControlPressed = NULL;
			if( m_pControlMouseOver == pControl )
				m_pControlMouseOver = NULL;

			SAFE_DELETE( pControl );
			m_Controls.Remove( i );

			return;
		}
	}
}


//--------------------------------------------------------------------------------------
void CMANGOScene::RemoveAllControls()
{
	if( s_pControlFocus && s_pControlFocus->m_pDialog == this )
		s_pControlFocus = NULL;
	if( s_pControlPressed && s_pControlPressed->m_pDialog == this )
		s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;

	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt( i );
		SAFE_DELETE( pControl );
	}

	m_Controls.RemoveAll();
}

//--------------------------------------------------------------------------------------
VOID CMANGOScene::SendEvent( UINT nEvent, bool bTriggeredByUser, CMANGOControl* pControl )
{
	// If no callback has been registered there's nowhere to send the event to
	if( m_pCallbackEvent == NULL )
		return;

	// Discard events triggered programatically if these types of events haven't been
	// enabled
	if( !bTriggeredByUser && !m_bNonUserEvents )
		return;
	//Message Callback 回调
	m_pCallbackEvent( nEvent, pControl->GetID(), pControl, m_pCallbackEventUserContext );
}

//--------------------------------------------------------------------------------------
void CMANGOScene::Refresh()
{
	if( s_pControlFocus )
		s_pControlFocus->OnFocusOut();

	if( m_pControlMouseOver )
		m_pControlMouseOver->OnMouseLeave();

	s_pControlFocus = NULL;
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;

	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt(i);
		pControl->Refresh();
	}

	if( m_bKeyboardInput )
		FocusDefaultControl();
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::OnRender( float fElapsedTime )
{   
	// If this assert triggers, you need to call CMANGOSceneResourceManager::On*Device() from inside
	// the application's device callbacks.  See the SDK samples for an example of how to do this.
	assert( m_pManager->GetD3DDevice() && m_pManager->m_pStateBlock && L"To fix hook up CMANGOSceneResourceManager to device callbacks.  See comments for details" );

	// See if the dialog needs to be refreshed
	if( m_fTimeLastRefresh < s_fTimeRefresh )
	{
		m_fTimeLastRefresh = MANGOGetTime();
		Refresh();
	}

	// For invisible dialog, out now.
	if( !m_bVisible ||
		( m_bMinimized && !m_bCaption ) )
		return S_OK;

	IDirect3DDevice9* pd3dDevice = m_pManager->GetD3DDevice();

	// Set up a state block here and restore it when finished drawing all the controls
	m_pManager->m_pStateBlock->Capture();

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED );
	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_RESULTARG, D3DTA_CURRENT );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	BOOL bBackgroundIsVisible = ( m_colorTopLeft | m_colorTopRight | m_colorBottomRight | m_colorBottomLeft ) & 0xff000000;
	if( !m_bMinimized && bBackgroundIsVisible )
	{
		MANGO_SCREEN_VERTEX_UNTEX vertices[4] =
		{
			(float)m_x,           (float)m_y,            0.5f, 1.0f, m_colorTopLeft,
			(float)m_x + m_width, (float)m_y,            0.5f, 1.0f, m_colorTopRight,
			(float)m_x + m_width, (float)m_y + m_height, 0.5f, 1.0f, m_colorBottomRight,
			(float)m_x,           (float)m_y + m_height, 0.5f, 1.0f, m_colorBottomLeft,
		};

		pd3dDevice->SetVertexShader( NULL );
		pd3dDevice->SetPixelShader( NULL );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		pd3dDevice->SetFVF( MANGO_SCREEN_VERTEX_UNTEX::FVF );
		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(MANGO_SCREEN_VERTEX_UNTEX) );
	}

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	MANGOTextureNode* pTextureNode = GetTexture( 0 );
	pd3dDevice->SetTexture( 0, pTextureNode->pTexture );

	m_pManager->m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE );

	// Render the caption if it's enabled.
	if( m_bCaption )
	{
		// DrawSprite will offset the rect down by
		// m_nCaptionHeight, so adjust the rect higher
		// here to negate the effect.
		RECT rc = { 0, -m_nCaptionHeight, m_width, 0 };
		DrawSprite( &m_CapElement, &rc );
		rc.left += 5; // Make a left margin
		WCHAR wszOutput[256];
		StringCchCopy( wszOutput, 256, m_wszCaption );
		if( m_bMinimized )
			StringCchCat( wszOutput, 256, L" (Minimized)" );
		DrawText( wszOutput, &m_CapElement, &rc, true );
	}

	// If the dialog is minimized, skip rendering
	// its controls.
	if( !m_bMinimized )
	{
		//for( int i= 0; i < m_Controls.GetSize(); i++ )
		for( int i= m_Controls.GetSize()-1; i >=0; i-- )
		{
			CMANGOControl* pControl = m_Controls.GetAt(i);   

			// Focused control is drawn last
			if( pControl == s_pControlFocus )
				continue;

			pControl->Render( pd3dDevice, fElapsedTime );
		}

		if( s_pControlFocus != NULL && s_pControlFocus->m_pDialog == this )
			s_pControlFocus->Render( pd3dDevice, fElapsedTime );
	}

	m_pManager->m_pSprite->End();

	m_pManager->m_pStateBlock->Apply();

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight )
{
	// If this assert triggers, you need to call CMANGOScene::Init() first.  This change
	// was made so that the MANGO's GUI could become seperate and optional from MANGO's core.  The 
	// creation and interfacing with CMANGOSceneResourceManager is now the responsibility 
	// of the application if it wishes to use MANGO's GUI.
	assert( m_pManager != NULL && L"To fix call CMANGOScene::Init() first.  See comments for details." ); 

	// Make sure the list is at least as large as the index being set
	UINT i;
	for( i=m_Fonts.GetSize(); i <= index; i++ )
	{
		m_Fonts.Add( -1 );
	}

	int iFont = m_pManager->AddFont( strFaceName, height, weight );
	m_Fonts.SetAt( index, iFont );

	return S_OK;
}

//--------------------------------------------------------------------------------------
MANGOFontNode* CMANGOScene::GetFont( UINT index )
{
	if( NULL == m_pManager )
		return NULL;
	return m_pManager->GetFontNode( m_Fonts.GetAt( index ) );
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::SetTexture( UINT index, LPCWSTR strFilename )
{
	// If this assert triggers, you need to call CMANGOScene::Init() first.  This change
	// was made so that the MANGO's GUI could become seperate and optional from MANGO's core.  The 
	// creation and interfacing with CMANGOSceneResourceManager is now the responsibility 
	// of the application if it wishes to use MANGO's GUI.
	assert( m_pManager != NULL && L"To fix this, call CMANGOScene::Init() first.  See comments for details." ); 

	// Make sure the list is at least as large as the index being set
	for( UINT i=m_Textures.GetSize(); i <= index; i++ )
	{
		m_Textures.Add( -1 );
	}

	int iTexture = m_pManager->AddTexture( strFilename );

	m_Textures.SetAt( index, iTexture );
	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::SetTexture( UINT index, LPCWSTR strResourceName, HMODULE hResourceModule )
{
	// If this assert triggers, you need to call CMANGOScene::Init() first.  This change
	// was made so that the MANGO's GUI could become seperate and optional from MANGO's core.  The 
	// creation and interfacing with CMANGOSceneResourceManager is now the responsibility 
	// of the application if it wishes to use MANGO's GUI.
	assert( m_pManager != NULL && L"To fix this, call CMANGOScene::Init() first.  See comments for details." ); 

	// Make sure the list is at least as large as the index being set
	for( UINT i=m_Textures.GetSize(); i <= index; i++ )
	{
		m_Textures.Add( -1 );
	}

	int iTexture = m_pManager->AddTexture( strResourceName, hResourceModule );

	m_Textures.SetAt( index, iTexture );
	return S_OK;
}


//--------------------------------------------------------------------------------------
MANGOTextureNode* CMANGOScene::GetTexture( UINT index )
{
	if( NULL == m_pManager )
		return NULL;
	return m_pManager->GetTextureNode( m_Textures.GetAt( index ) );
}



//--------------------------------------------------------------------------------------
bool CMANGOScene::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	bool bHandled = false;

	
	// For invisible dialog, do not handle anything.
	if( !m_bVisible )
		return false;

	// If automation command-line switch is on, enable this dialog's keyboard input
	// upon any key press or mouse click.
	if( MANGOGetAutomation() &&
		( WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg || WM_KEYDOWN == uMsg ) )
	{
		m_pManager->EnableKeyboardInputForAllWindow();
	}

	// If caption is enable, check for clicks in the caption area.
	if( m_bCaption )
	{
		if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK )
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			if( mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
				mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight )
			{
				m_bDrag = true;
				SetCapture( MANGOGetHWND() );
				return true;
			}
		} else
			if( uMsg == WM_LBUTTONUP && m_bDrag )
			{
				POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

				if( mousePoint.x >= m_x && mousePoint.x < m_x + m_width &&
					mousePoint.y >= m_y && mousePoint.y < m_y + m_nCaptionHeight )
				{
					ReleaseCapture();
					m_bDrag = false;
					m_bMinimized = !m_bMinimized;
					return true;
				}
			}
	}

	// If the dialog is minimized, don't send any messages to controls.
	if( m_bMinimized )
		return false;

	// If a control is in focus, it belongs to this dialog, and it's enabled, then give
	// it the first chance at handling the message.
	if( s_pControlFocus && 
		s_pControlFocus->m_pDialog == this && 
		s_pControlFocus->GetEnabled() )
	{
		// If the control MsgProc handles it, then we don't.
		if( s_pControlFocus->MsgProc( uMsg, wParam, lParam ) )
			return true;
	}

	switch( uMsg )
	{
	case WM_SIZE:
	case WM_MOVE:
		{
			// Handle sizing and moving messages so that in case the mouse cursor is moved out
			// of an UI control because of the window adjustment, we can properly
			// unhighlight the highlighted control.
			POINT pt = { -1, -1 };
			OnMouseMove( pt );
			break;
		}

	case WM_ACTIVATEAPP:
		// Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
		// as the application is activated/deactivated.  This matches the Windows
		// behavior.
		if( s_pControlFocus && 
			s_pControlFocus->m_pDialog == this && 
			s_pControlFocus->GetEnabled() )
		{
			if( wParam )
				s_pControlFocus->OnFocusIn();
			else
				s_pControlFocus->OnFocusOut();
		}
		break;

		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( s_pControlFocus && 
				s_pControlFocus->m_pDialog == this && 
				s_pControlFocus->GetEnabled() )
			{
				if( s_pControlFocus->HandleKeyboard( uMsg, wParam, lParam ) )
					return true;
			}

			// Not yet handled, see if this matches a control's hotkey
			// Activate the hotkey if the focus doesn't belong to an
			// edit box.
			if( uMsg == WM_KEYDOWN && ( !s_pControlFocus ||
				( s_pControlFocus->GetType() != MANGO_CONTROL_EDITBOX
				&& s_pControlFocus->GetType() != MANGO_CONTROL_IMEEDITBOX ) ) )
			{
				for( int i=0; i < m_Controls.GetSize(); i++ )
				{
					CMANGOControl* pControl = m_Controls.GetAt( i );
					if( pControl->GetHotkey() == wParam )
					{
						pControl->OnHotkey();
						return true;
					}
				}
			}

			// Not yet handled, check for focus messages
			if( uMsg == WM_KEYDOWN )
			{
				// If keyboard input is not enabled, this message should be ignored
				if( !m_bKeyboardInput )
					return false;

				switch( wParam )
				{
				case VK_RIGHT:
				case VK_DOWN:
					if( s_pControlFocus != NULL )
					{
						return OnCycleFocus( true );
					}
					break;

				case VK_LEFT:
				case VK_UP:
					if( s_pControlFocus != NULL )
					{
						return OnCycleFocus( false );
					}
					break;

				case VK_TAB: 
					{
						bool bShiftDown = ((GetKeyState( VK_SHIFT ) & 0x8000) != 0);
						return OnCycleFocus( !bShiftDown );
					}
				}
			}

			break;
		}


		// Mouse messages
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			// If not accepting mouse input, return false to indicate the message should still 
			// be handled by the application (usually to move the camera).
			if( !m_bMouseInput )
				return false;

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			mousePoint.x -= m_x;
			mousePoint.y -= m_y;

			// If caption is enabled, offset the Y coordinate by the negative of its height.
			if( m_bCaption )
				mousePoint.y -= m_nCaptionHeight;

			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if( s_pControlFocus && 
				s_pControlFocus->m_pDialog == this && 
				s_pControlFocus->GetEnabled() )
			{
				if( s_pControlFocus->HandleMouse( uMsg, mousePoint, wParam, lParam ) )
					return true;
			}

			// Not yet handled, see if the mouse is over any controls
			CMANGOControl* pControl = GetControlAtPoint( mousePoint );
			if( pControl != NULL && pControl->GetEnabled() )
			{
				bHandled = pControl->HandleMouse( uMsg, mousePoint, wParam, lParam );
				if( bHandled )
					return true;
			}
			else
			{
				// Mouse not over any controls in this dialog, if there was a control
				// which had focus it just lost it
				if( uMsg == WM_LBUTTONDOWN && 
					s_pControlFocus && 
					s_pControlFocus->m_pDialog == this )
				{
					s_pControlFocus->OnFocusOut();
					s_pControlFocus = NULL;
				}
			}

			// Still not handled, hand this off to the dialog. Return false to indicate the
			// message should still be handled by the application (usually to move the camera).
			switch( uMsg )
			{
			case WM_MOUSEMOVE:
				OnMouseMove( mousePoint );
				return false;
			}

			break;
		}

	case WM_CAPTURECHANGED:
		{
			// The application has lost mouse capture.
			// The dialog object may not have received
			// a WM_MOUSEUP when capture changed. Reset
			// m_bDrag so that the dialog does not mistakenly
			// think the mouse button is still held down.
			if( (HWND)lParam != hWnd )
				m_bDrag = false;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------
CMANGOControl* CMANGOScene::GetControlAtPoint( POINT pt )
{
	// Search through all child controls for the first one which
	// contains the mouse point
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt(i);

		if( pControl == NULL )
		{
			continue;
		}

		// We only return the current control if it is visible
		// and enabled.  Because GetControlAtPoint() is used to do mouse
		// hittest, it makes sense to perform this filtering.
		if( pControl->ContainsPoint( pt ) && pControl->GetEnabled() && pControl->GetVisible() )
		{
			return pControl;
		}
	}

	return NULL;
}


//--------------------------------------------------------------------------------------
bool CMANGOScene::GetControlEnabled( int ID )
{
	CMANGOControl* pControl = GetControlByID( ID );
	if( pControl == NULL )
		return false;

	return pControl->GetEnabled();
}



//--------------------------------------------------------------------------------------
void CMANGOScene::SetControlEnabled( int ID, bool bEnabled )
{
	CMANGOControl* pControl = GetControlByID( ID );
	if( pControl == NULL )
		return;

	pControl->SetEnabled( bEnabled );
}


//--------------------------------------------------------------------------------------
void CMANGOScene::OnMouseUp( POINT pt )
{
	s_pControlPressed = NULL;
	m_pControlMouseOver = NULL;
}


//--------------------------------------------------------------------------------------
void CMANGOScene::OnMouseMove( POINT pt )
{
	// Figure out which control the mouse is over now
	CMANGOControl* pControl = GetControlAtPoint( pt );

	// If the mouse is still over the same control, nothing needs to be done
	if( pControl == m_pControlMouseOver )
		return;

	// Handle mouse leaving the old control
	if( m_pControlMouseOver )
		m_pControlMouseOver->OnMouseLeave();

	// Handle mouse entering the new control
	m_pControlMouseOver = pControl;
	if( pControl != NULL )
		m_pControlMouseOver->OnMouseEnter();
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::SetDefaultElement( UINT nControlType, UINT iElement, CMANGOElement* pElement, int type )
{
	// If this Element type already exist in the list, simply update the stored Element
	for( int i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		MANGOElementHolder* pElementHolder = m_DefaultElements.GetAt( i );

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement &&
			pElementHolder->nResourceType == type )
		{
			pElementHolder->Element = *pElement;
			return S_OK;
		}
	}

	// Otherwise, add a new entry
	MANGOElementHolder* pNewHolder;
	pNewHolder = new MANGOElementHolder;
	if( pNewHolder == NULL )
		return E_OUTOFMEMORY;

	pNewHolder->nControlType = nControlType;
	pNewHolder->iElement = iElement;
	pNewHolder->nResourceType = type;
	pNewHolder->Element = *pElement;

	m_DefaultElements.Add( pNewHolder );
	return S_OK;
}


//--------------------------------------------------------------------------------------
CMANGOElement* CMANGOScene::GetDefaultElement( UINT nControlType, UINT iElement, int type )
{
	for( int i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		MANGOElementHolder* pElementHolder = m_DefaultElements.GetAt( i );

		if( pElementHolder->nControlType == nControlType &&
			pElementHolder->iElement == iElement &&
			pElementHolder->nResourceType == type )
		{
			return &pElementHolder->Element;
		}
	}

	return NULL;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddImage( int ID, int x, int y, int width, int height, float z,bool bIsDefault, CMANGOImage** ppCreated)
{
	HRESULT hr = S_OK;

	CMANGOImage* pImage = new CMANGOImage( this );

	if( ppCreated != NULL )
		*ppCreated = pImage;

	if( pImage == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pImage );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pImage->SetID( ID ); 
	pImage->SetLocation( x, y ,z);
	pImage->SetSize( width, height );
	pImage->m_bIsDefault = bIsDefault;
	sort();
	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddLable( int ID, LPCWSTR strText, int x, int y, int width, int height, float z,bool bIsDefault, CMANGOLable** ppCreated)
{
	HRESULT hr = S_OK;

	CMANGOLable* pLable = new CMANGOLable( this );

	if( ppCreated != NULL )
		*ppCreated = pLable;

	if( pLable == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pLable );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pLable->SetID( ID ); 
	pLable->SetText( strText );
	pLable->SetLocation( x, y ,z);
	pLable->SetSize( width, height );
	pLable->m_bIsDefault = bIsDefault;
	sort();
	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height, float z, int type, UINT nHotkey, bool bIsDefault, CMANGOButton** ppCreated )
{
	HRESULT hr = S_OK;

	CMANGOButton* pButton = new CMANGOButton( this );

	if( ppCreated != NULL )
		*ppCreated = pButton;

	if( pButton == NULL )
		return E_OUTOFMEMORY;
	
	hr = AddControl( pButton, type );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pButton->SetID( ID ); 
	pButton->SetText( strText );
	pButton->SetLocation( x, y ,z);
	pButton->SetSize( width, height );
	pButton->SetHotkey( nHotkey );
	pButton->m_bIsDefault = bIsDefault;
	sort();
	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height, float z,bool bChecked, UINT nHotkey, bool bIsDefault, CMANGOCheckBox** ppCreated)
{
	HRESULT hr = S_OK;

	CMANGOCheckBox* pCheckBox = new CMANGOCheckBox( this );

	if( ppCreated != NULL )
		*ppCreated = pCheckBox;

	if( pCheckBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pCheckBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pCheckBox->SetID( ID ); 
	pCheckBox->SetText( strText );
	pCheckBox->SetLocation( x, y ,z);
	pCheckBox->SetSize( width, height );
	pCheckBox->SetHotkey( nHotkey );
	pCheckBox->m_bIsDefault = bIsDefault;
	pCheckBox->SetChecked( bChecked );
	sort();

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height, float z,bool bChecked, UINT nHotkey, bool bIsDefault, CMANGORadioButton** ppCreated )
{
	HRESULT hr = S_OK;

	CMANGORadioButton* pRadioButton = new CMANGORadioButton( this );

	if( ppCreated != NULL )
		*ppCreated = pRadioButton;

	if( pRadioButton == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pRadioButton );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pRadioButton->SetID( ID ); 
	pRadioButton->SetText( strText );
	pRadioButton->SetButtonGroup( nButtonGroup );
	pRadioButton->SetLocation( x, y ,z);
	pRadioButton->SetSize( width, height );
	pRadioButton->SetHotkey( nHotkey );
	pRadioButton->SetChecked( bChecked );
	pRadioButton->m_bIsDefault = bIsDefault;
	pRadioButton->SetChecked( bChecked );
	sort();

	return S_OK;
}




//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddComboBox( int ID, int x, int y, int width, int height, float z,UINT nHotkey, bool bIsDefault, CMANGOComboBox** ppCreated )
{
	HRESULT hr = S_OK;

	CMANGOComboBox* pComboBox = new CMANGOComboBox( this );

	if( ppCreated != NULL )
		*ppCreated = pComboBox;

	if( pComboBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pComboBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pComboBox->SetID( ID ); 
	pComboBox->SetLocation( x, y ,z);
	pComboBox->SetSize( width, height );
	pComboBox->SetHotkey( nHotkey );
	pComboBox->m_bIsDefault = bIsDefault;
	sort();

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddSlider( int ID, int x, int y, int width, int height ,float z, int min, int max, int value, bool bIsDefault, CMANGOSlider** ppCreated )
{
	HRESULT hr = S_OK;

	CMANGOSlider* pSlider = new CMANGOSlider( this );

	if( ppCreated != NULL )
		*ppCreated = pSlider;

	if( pSlider == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pSlider );
	if( FAILED(hr) )
		return hr;

	// Set the ID and list index
	pSlider->SetID( ID ); 
	pSlider->SetLocation( x, y ,z);
	pSlider->SetSize( width, height );
	pSlider->m_bIsDefault = bIsDefault;
	pSlider->SetRange( min, max );
	pSlider->SetValue( value );
	pSlider->UpdateRects();
	sort();

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z , bool bIsDefault, CMANGOEditBox** ppCreated)
{
	HRESULT hr = S_OK;

	CMANGOEditBox *pEditBox = new CMANGOEditBox( this );

	if( ppCreated != NULL )
		*ppCreated = pEditBox;

	if( pEditBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pEditBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pEditBox->SetID( ID ); 
	pEditBox->SetLocation( x, y ,z);
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;

	if( strText )
		pEditBox->SetText( strText );
	
	sort();

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z, bool bIsDefault, CMANGOIMEEditBox** ppCreated )
{
	HRESULT hr = S_OK;
	CMANGOIMEEditBox *pEditBox = new CMANGOIMEEditBox( this );

	if( ppCreated != NULL )
		*ppCreated = pEditBox;

	if( pEditBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pEditBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pEditBox->SetID( ID ); 
	pEditBox->SetLocation( x, y ,z);
	pEditBox->SetSize( width, height );
	pEditBox->m_bIsDefault = bIsDefault;

	if( strText )
		pEditBox->SetText( strText );

	sort();

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddListBox( int ID, int x, int y, int width, int height ,float z, DWORD dwStyle, CMANGOListBox** ppCreated)
{
	HRESULT hr = S_OK;
	CMANGOListBox *pListBox = new CMANGOListBox( this );

	if( ppCreated != NULL )
		*ppCreated = pListBox;

	if( pListBox == NULL )
		return E_OUTOFMEMORY;

	hr = AddControl( pListBox );
	if( FAILED(hr) )
		return hr;

	// Set the ID and position
	pListBox->SetID( ID );
	pListBox->SetLocation( x, y, z);
	pListBox->SetSize( width, height );
	pListBox->SetStyle( dwStyle );
	sort();

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddScrollBar( int ID, int x, int y, int width, int height ,float z, int iTrackStart, int iTrackEnd, int iTrackPos, int iPageSize, CMANGOScrollBar** ppCreated )
{
	HRESULT hr = S_OK;
	CMANGOScrollBar *pScrollBar = new CMANGOScrollBar(this);

	if (NULL != ppCreated)
		*ppCreated = pScrollBar;

	if (NULL == pScrollBar)
		return E_OUTOFMEMORY;

	hr = AddControl(pScrollBar);
	if (FAILED(hr))
		return hr;

	pScrollBar->SetID(ID);
	pScrollBar->SetLocation(x, y, z);
	pScrollBar->SetSize(width, height);
	pScrollBar->SetTrackRange(iTrackStart, iTrackEnd);
	pScrollBar->SetTrackPos(iTrackPos);
	pScrollBar->SetPageSize(iPageSize);
	sort();

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::InitControl( CMANGOControl* pControl, int type )
{
	HRESULT hr;

	if( pControl == NULL )
		return E_INVALIDARG;

	pControl->m_Index = m_Controls.GetSize();

	// Look for a default Element entries
	for( int i=0; i < m_DefaultElements.GetSize(); i++ )
	{
		MANGOElementHolder* pElementHolder = m_DefaultElements.GetAt( i );
		if( pElementHolder->nControlType == pControl->GetType() 
			&& pElementHolder->nResourceType == type )
			pControl->SetElement( pElementHolder->iElement, &pElementHolder->Element );
	}

	V_RETURN( pControl->OnInit() );

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::AddControl( CMANGOControl* pControl, int type )
{
	HRESULT hr = S_OK;

	hr = InitControl( pControl, type );
	if( FAILED(hr) )
		return DXTRACE_ERR( L"CMANGOScene::InitControl", hr );

	// Add to the list
	hr = m_Controls.Add( pControl );
	if( FAILED(hr) )
	{
		return DXTRACE_ERR( L"CGrowableArray::Add", hr );
	}
	
	return S_OK;
}

//--------------------------------------------------------------------------------------
void CMANGOScene::sort()
{
	//在这里对控件排序.....
	int size=m_Controls.GetSize();
	if(size>1)
	{
		CMANGOControl* pControl;
		for(int i = size-1; i && (m_Controls.GetAt( i )->m_z) < (m_Controls.GetAt( i-1 )->m_z);i--)
		{
			pControl = m_Controls.GetAt( i );
			m_Controls.GetAt( i ) = m_Controls.GetAt( i -1);
			m_Controls.GetAt( i -1) = pControl;
		}
	}
}

//--------------------------------------------------------------------------------------
CMANGOControl* CMANGOScene::GetControlByID( int ID )
{
	// Try to find the control with the given ID
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt( i );

		if( pControl->GetID() == ID )
		{
			return pControl;
		}
	}

	// Not found
	return NULL;
}



//--------------------------------------------------------------------------------------
CMANGOControl* CMANGOScene::GetControl( int ID, UINT nControlType )
{
	// Try to find the control with the given ID
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt( i );

		if( pControl->GetID() == ID && pControl->GetType() == nControlType )
		{
			return pControl;
		}
	}

	// Not found
	return NULL;
}



//--------------------------------------------------------------------------------------
CMANGOControl* CMANGOScene::GetNextControl( CMANGOControl* pControl )
{
	int index = pControl->m_Index + 1;

	CMANGOScene* pDialog = pControl->m_pDialog;

	// Cycle through dialogs in the loop to find the next control. Note
	// that if only one control exists in all looped dialogs it will
	// be the returned 'next' control.
	while( index >= (int) pDialog->m_Controls.GetSize() )
	{
		pDialog = pDialog->m_pNextDialog;
		index = 0;
	}

	return pDialog->m_Controls.GetAt( index );
}

//--------------------------------------------------------------------------------------
CMANGOControl* CMANGOScene::GetPrevControl( CMANGOControl* pControl )
{
	int index = pControl->m_Index - 1;

	CMANGOScene* pDialog = pControl->m_pDialog;

	// Cycle through dialogs in the loop to find the next control. Note
	// that if only one control exists in all looped dialogs it will
	// be the returned 'previous' control.
	while( index < 0 )
	{
		pDialog = pDialog->m_pPrevDialog;
		if( pDialog == NULL )
			pDialog = pControl->m_pDialog;

		index = pDialog->m_Controls.GetSize() - 1;
	}

	return pDialog->m_Controls.GetAt( index );    
}


//--------------------------------------------------------------------------------------
void CMANGOScene::ClearRadioButtonGroup( UINT nButtonGroup )
{
	// Find all radio buttons with the given group number
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt( i );

		if( pControl->GetType() == MANGO_CONTROL_RADIOBUTTON )
		{
			CMANGORadioButton* pRadioButton = (CMANGORadioButton*) pControl;

			if( pRadioButton->GetButtonGroup() == nButtonGroup )
				pRadioButton->SetChecked( false, false );
		}
	}
}



//--------------------------------------------------------------------------------------
void CMANGOScene::ClearComboBox( int ID )
{
	CMANGOComboBox* pComboBox = GetComboBoxByID( ID );
	if( pComboBox == NULL )
		return;

	pComboBox->RemoveAllItems();
}




//--------------------------------------------------------------------------------------
void CMANGOScene::RequestFocus( CMANGOControl* pControl )
{
	if( s_pControlFocus == pControl )
		return;

	if( !pControl->CanHaveFocus() )
		return;

	if( s_pControlFocus )
		s_pControlFocus->OnFocusOut();

	pControl->OnFocusIn();
	s_pControlFocus = pControl;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::DrawRect( RECT* pRect, D3DCOLOR color )
{
	RECT rcScreen = *pRect;
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	MANGO_SCREEN_VERTEX vertices[4] =
	{
		(float) rcScreen.left -0.5f,  (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0,
			(float) rcScreen.right -0.5f, (float) rcScreen.top -0.5f,    0.5f, 1.0f, color, 0, 0, 
			(float) rcScreen.right -0.5f, (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0, 
			(float) rcScreen.left -0.5f,  (float) rcScreen.bottom -0.5f, 0.5f, 1.0f, color, 0, 0,
	};

	IDirect3DDevice9* pd3dDevice = m_pManager->GetD3DDevice();


	// Since we're doing our own drawing here we need to flush the sprites
	m_pManager->m_pSprite->Flush();
	IDirect3DVertexDeclaration9 *pDecl = NULL;
	pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
	pd3dDevice->SetFVF( MANGO_SCREEN_VERTEX::FVF );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vertices, sizeof(MANGO_SCREEN_VERTEX) );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	// Restore the vertex decl
	pd3dDevice->SetVertexDeclaration( pDecl );
	pDecl->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color )
{
	MANGO_SCREEN_VERTEX* vertices = new MANGO_SCREEN_VERTEX[ nNumPoints ];
	if( vertices == NULL )
		return E_OUTOFMEMORY;

	MANGO_SCREEN_VERTEX* pVertex = vertices;
	POINT* pt = apPoints;
	for( UINT i=0; i < nNumPoints; i++ )
	{
		pVertex->x = m_x + (float) pt->x;
		pVertex->y = m_y + (float) pt->y;
		pVertex->z = 0.5f;
		pVertex->h = 1.0f;
		pVertex->color = color;
		pVertex->tu = 0.0f;
		pVertex->tv = 0.0f;

		pVertex++;
		pt++;
	}

	IDirect3DDevice9* pd3dDevice = m_pManager->GetD3DDevice();

	// Since we're doing our own drawing here we need to flush the sprites
	m_pManager->m_pSprite->Flush();
	IDirect3DVertexDeclaration9 *pDecl = NULL;
	pd3dDevice->GetVertexDeclaration( &pDecl );  // Preserve the sprite's current vertex decl
	pd3dDevice->SetFVF( MANGO_SCREEN_VERTEX::FVF );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

	pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, nNumPoints - 1, vertices, sizeof(MANGO_SCREEN_VERTEX) );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	// Restore the vertex decl
	pd3dDevice->SetVertexDeclaration( pDecl );
	pDecl->Release();

	SAFE_DELETE_ARRAY( vertices );
	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::DrawSprite( CMANGOElement* pElement, RECT* prcDest )
{
	// No need to draw fully transparent layers
	if( pElement->TextureColor.Current.a == 0 )
		return S_OK;

	RECT rcTexture = pElement->rcTexture;

	RECT rcScreen = *prcDest;
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	MANGOTextureNode* pTextureNode = GetTexture( pElement->iTexture );
	if( pTextureNode == NULL )
		return E_FAIL;

	float fScaleX = (float) RectWidth( rcScreen ) / RectWidth( rcTexture );
	float fScaleY = (float) RectHeight( rcScreen ) / RectHeight( rcTexture );

	D3DXMATRIXA16 matTransform;
	D3DXMatrixScaling( &matTransform, fScaleX, fScaleY, 1.0f );

	m_pManager->m_pSprite->SetTransform( &matTransform );

	D3DXVECTOR3 vPos( (float)rcScreen.left, (float)rcScreen.top, 0.0f );

	vPos.x /= fScaleX;
	vPos.y /= fScaleY;

	return m_pManager->m_pSprite->Draw( pTextureNode->pTexture, &rcTexture, NULL, &vPos, pElement->TextureColor.Current );
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::CalcTextRect( LPCWSTR strText, CMANGOElement* pElement, RECT* prcDest, int nCount )
{
	HRESULT hr = S_OK;

	MANGOFontNode* pFontNode = GetFont( pElement->iFont );
	if( pFontNode == NULL )
		return E_FAIL;

	DWORD dwTextFormat = pElement->dwTextFormat | DT_CALCRECT;
	// Since we are only computing the rectangle, we don't need a sprite.
	hr = pFontNode->pFont->DrawText( NULL, strText, nCount, prcDest, dwTextFormat, pElement->FontColor.Current );
	if( FAILED(hr) )
		return hr;

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOScene::DrawText( LPCWSTR strText, CMANGOElement* pElement, RECT* prcDest, bool bShadow, int nCount )
{
	HRESULT hr = S_OK;

	// No need to draw fully transparent layers
	if( pElement->FontColor.Current.a == 0 )
		return S_OK;

	RECT rcScreen = *prcDest;
	OffsetRect( &rcScreen, m_x, m_y );

	// If caption is enabled, offset the Y position by its height.
	if( m_bCaption )
		OffsetRect( &rcScreen, 0, m_nCaptionHeight );

	//debug
	//DrawRect( &rcScreen, D3DCOLOR_ARGB(100, 255, 0, 0) );

	D3DXMATRIXA16 matTransform;
	D3DXMatrixIdentity( &matTransform );
	m_pManager->m_pSprite->SetTransform( &matTransform );

	MANGOFontNode* pFontNode = GetFont( pElement->iFont );

	if( bShadow )
	{
		RECT rcShadow = rcScreen;
		OffsetRect( &rcShadow, 1, 1 );
		hr = pFontNode->pFont->DrawText( m_pManager->m_pSprite, strText, nCount, &rcShadow, pElement->dwTextFormat, D3DCOLOR_ARGB(DWORD(pElement->FontColor.Current.a * 255), 0, 0, 0) );
		if( FAILED(hr) )
			return hr;
	}

	hr = pFontNode->pFont->DrawText( m_pManager->m_pSprite, strText, nCount, &rcScreen, pElement->dwTextFormat, pElement->FontColor.Current );
	if( FAILED(hr) )
		return hr;

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CMANGOScene::SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight )
{
	m_colorTopLeft = colorTopLeft;
	m_colorTopRight = colorTopRight;
	m_colorBottomLeft = colorBottomLeft;
	m_colorBottomRight = colorBottomRight;
}


//--------------------------------------------------------------------------------------
void CMANGOScene::SetNextDialog( CMANGOScene* pNextDialog )
{ 
	if( pNextDialog == NULL )
		pNextDialog = this;

	m_pNextDialog = pNextDialog;
	if( pNextDialog )
		m_pNextDialog->m_pPrevDialog = this;
}


//--------------------------------------------------------------------------------------
void CMANGOScene::ClearFocus()
{
	if( s_pControlFocus )
	{
		s_pControlFocus->OnFocusOut();
		s_pControlFocus = NULL;
	}

	ReleaseCapture();
}


//--------------------------------------------------------------------------------------
void CMANGOScene::FocusDefaultControl()
{
	// Check for default control in this dialog
	for( int i=0; i < m_Controls.GetSize(); i++ )
	{
		CMANGOControl* pControl = m_Controls.GetAt( i );
		if( pControl->m_bIsDefault )
		{
			// Remove focus from the current control
			ClearFocus();

			// Give focus to the default control
			s_pControlFocus = pControl;
			s_pControlFocus->OnFocusIn();
			return;
		}
	}
}


//--------------------------------------------------------------------------------------
bool CMANGOScene::OnCycleFocus( bool bForward )
{
	CMANGOControl *pControl = NULL;
	CMANGOScene *pDialog = NULL; // pDialog and pLastDialog are used to track wrapping of
	CMANGOScene *pLastDialog;    // focus from first control to last or vice versa.

	if( s_pControlFocus == NULL )
	{
		// If s_pControlFocus is NULL, we focus the first control of first dialog in
		// the case that bForward is true, and focus the last control of last dialog when
		// bForward is false.
		//
		if( bForward )
		{
			// Search for the first control from the start of the dialog
			// array.
			for( int d = 0; d < m_pManager->m_Scene.GetSize(); ++d )
			{
				pDialog = pLastDialog = m_pManager->m_Scene.GetAt(d);
				if( pDialog && pDialog->m_Controls.GetSize() > 0 )
				{
					pControl = pDialog->m_Controls.GetAt(0);
					break;
				}
			}

			if( !pDialog || !pControl )
			{
				// No dialog has been registered yet or no controls have been
				// added to the dialogs. Cannot proceed.
				return true;
			}
		}
		else
		{
			// Search for the first control from the end of the dialog
			// array.
			for( int d = m_pManager->m_Scene.GetSize() - 1; d >= 0; --d )
			{
				pDialog = pLastDialog = m_pManager->m_Scene.GetAt(d);
				if( pDialog && pDialog->m_Controls.GetSize() > 0 )
				{
					pControl = pDialog->m_Controls.GetAt( pDialog->m_Controls.GetSize() - 1 );
					break;
				}
			}

			if( !pDialog || !pControl )
			{
				// No dialog has been registered yet or no controls have been
				// added to the dialogs. Cannot proceed.
				return true;
			}
		}
	}
	else
		if( s_pControlFocus->m_pDialog != this )
		{
			// If a control belonging to another dialog has focus, let that other
			// dialog handle this event by returning false.
			//
			return false;
		}
		else
		{
			// Focused control belongs to this dialog. Cycle to the
			// next/previous control.
			pLastDialog = s_pControlFocus->m_pDialog;
			pControl = (bForward) ? GetNextControl( s_pControlFocus ) : GetPrevControl( s_pControlFocus );
			pDialog = pControl->m_pDialog;
		}

		for( int i=0; i < 0xffff; i++ )
		{
			// If we just wrapped from last control to first or vice versa,
			// set the focused control to NULL. This state, where no control
			// has focus, allows the camera to work.
			int nLastDialogIndex = m_pManager->m_Scene.IndexOf( pLastDialog );
			int nDialogIndex = m_pManager->m_Scene.IndexOf( pDialog );
			if( ( !bForward && nLastDialogIndex < nDialogIndex ) ||
				( bForward && nDialogIndex < nLastDialogIndex ) )
			{
				if( s_pControlFocus )
					s_pControlFocus->OnFocusOut();
				s_pControlFocus = NULL;
				return true;
			}

			// If we've gone in a full circle then focus doesn't change
			if( pControl == s_pControlFocus )
				return true;

			// If the dialog accepts keybord input and the control can have focus then
			// move focus
			if( pControl->m_pDialog->m_bKeyboardInput && pControl->CanHaveFocus() )
			{
				if( s_pControlFocus )
					s_pControlFocus->OnFocusOut();
				s_pControlFocus = pControl;
				s_pControlFocus->OnFocusIn();
				return true;
			}

			pLastDialog = pDialog;
			pControl = (bForward) ? GetNextControl( pControl ) : GetPrevControl( pControl );
			pDialog = pControl->m_pDialog;
		}

		// If we reached this point, the chain of dialogs didn't form a complete loop
		DXTRACE_ERR( L"CMANGOScene: Multiple dialogs are improperly chained together", E_FAIL );
		return false;
}
void CMANGOScene::InitDefaultElements(char * strName )
{
	CMANGOElement Element;
	RECT rcTexture;


	tinyxml2::XMLDocument m_pDoc;
	tinyxml2::XMLElement* pRoot =NULL;

	if( m_pDoc.LoadFile( strName ) )
	{
		WCHAR temp[128];
		std::wstring tem = CU_A2U(strName);
		StringCbPrintf(temp,128,L"错误代码481，请检查 %s 是否存在",tem);
		MessageBox(NULL,temp,L"文件没有找到", MB_OK);
		return ;
	}
	
	if(pRoot = m_pDoc.FirstChildElement("MANGOGUI"))
	{
		for(pRoot=pRoot->FirstChildElement();pRoot;pRoot = pRoot->NextSiblingElement())
		{
//--------------------------------------------------------------------------
// Default font
//--------------------------------------------------------------------------
			if(!strcmp(pRoot->Value(),"Font"))
			{	
				if(!strcmp(pRoot->Attribute("Type"),"DefaultFont"))
				{	
					int ID,height,weight;
					char FaceName[256];
					//初始化字体.
					if(pRoot->Attribute("IndexID"))
						ID = (atoi(pRoot->Attribute("IndexID")));			//字体宽度
					if(pRoot->Attribute("height"))
						height= (atoi(pRoot->Attribute("height")));		//字体高度
					if(pRoot->Attribute("weight"))
						weight= (atoi(pRoot->Attribute("weight")));		//字体胖度			

					if(pRoot->Attribute("name"))
						strcpy_s(FaceName, 256, pRoot->Attribute("name"));
					std::wstring temp = CU_A2U(FaceName);
					SetFont( ID,temp.c_str(),height,weight );
				
				}  
			}			
//--------------------------------------------------------------------------
// Element for the caption
//--------------------------------------------------------------------------
			else if(!strcmp(pRoot->Value(),"Caption"))
			{	
				int ID;
				//初始化字体.
				if(pRoot->Attribute("FontID"))
					ID = (atoi(pRoot->Attribute("FontID")));			//默认字体类型ID
				m_CapElement.SetFont( ID );
				for(tinyxml2::XMLElement* ptempElement=pRoot->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
				{
					if(!strcmp(ptempElement->Value(),"TexRECT"))
					{	
						int l,t,r,b;
						if(ptempElement->Attribute("Left"))
							l= (atoi(ptempElement->Attribute("Left")));			
						if(ptempElement->Attribute("Top"))
							t= (atoi(ptempElement->Attribute("Top")));					


						if(ptempElement->Attribute("Right"))
							r= (atoi(ptempElement->Attribute("Right")));					
						if(ptempElement->Attribute("Bottom"))
							b= (atoi(ptempElement->Attribute("Bottom")));	
						SetRect( &rcTexture, l, t, r, b );
						m_CapElement.SetTexture( 0, &rcTexture );
					}else
					if(!strcmp(ptempElement->Value(),"TexColor"))
					{	
						int r,g,b,a;
						if(ptempElement->Attribute("R"))
							r= (atoi(ptempElement->Attribute("R")));			
						if(ptempElement->Attribute("G"))
							g= (atoi(ptempElement->Attribute("G")));					
						if(ptempElement->Attribute("B"))
							b= (atoi(ptempElement->Attribute("B")));					
						if(ptempElement->Attribute("A"))
							a= (atoi(ptempElement->Attribute("A")));	
						m_CapElement.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB(a, r, g,b);
								}else
								if(!strcmp(ptempElement->Value(),"FontColor"))
								{	
									int r,g,b,a;
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));				
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));					
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));	
									m_CapElement.FontColor.States[ MANGO_STATE_NORMAL ] =  D3DCOLOR_ARGB(a, r, g, b);
								}
							}	
							m_CapElement.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_VCENTER );
							// Pre-blend as we don't need to transition the state
							m_CapElement.TextureColor.Blend( MANGO_STATE_NORMAL, 10.0f );
							m_CapElement.FontColor.Blend( MANGO_STATE_NORMAL, 10.0f );
				}
//--------------------------------------------------------------------------
// CMANGOImage
//--------------------------------------------------------------------------
			else if(!strcmp(pRoot->Value(),"Image"))
			{
				int r,g,b,a;
				if(pRoot->Attribute("R"))
					r= (atoi(pRoot->Attribute("R")));			
				if(pRoot->Attribute("G"))
					g= (atoi(pRoot->Attribute("G")));					
				if(pRoot->Attribute("B"))
					b= (atoi(pRoot->Attribute("B")));			
				if(pRoot->Attribute("A"))
					a= (atoi(pRoot->Attribute("A")));	

				Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB(255, 255, 255, 255);
				Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
				Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);

				// Assign the Element
				SetDefaultElement( MANGO_CONTROL_IMAGE, 0, &Element );
			}
//--------------------------------------------------------------------------
// CMANGOLable
//--------------------------------------------------------------------------
			else if(!strcmp(pRoot->Value(),"Lable"))
			{
				int ID,r,g,b,a;
				//初始化字体.
				if(pRoot->Attribute("FontID"))
					ID = (atoi(pRoot->Attribute("FontID")));	
				if(pRoot->Attribute("R"))
					r= (atoi(pRoot->Attribute("R")));			
				if(pRoot->Attribute("G"))
					g= (atoi(pRoot->Attribute("G")));					
				if(pRoot->Attribute("B"))
					b= (atoi(pRoot->Attribute("B")));			
				if(pRoot->Attribute("A"))
					a= (atoi(pRoot->Attribute("A")));	
				
				Element.SetFont( ID );
				Element.FontColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );	

				// Assign the Element
				SetDefaultElement( MANGO_CONTROL_LABLE, 0, &Element );
			}
//--------------------------------------------------------------------------
//  CMANGOButton - Button
//--------------------------------------------------------------------------
			else if(!strcmp(pRoot->Value(),"Button"))
			{
				int ID,r,g,b,a;
				//初始化字体.
				if(pRoot->Attribute("FontID"))
					ID = (atoi(pRoot->Attribute("FontID")));
				Element.SetFont( ID );

				for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
				{
					int type(0);
					if(!strcmp(ptempobject->Value(),"ButtonNormal"))
					{
						for(tinyxml2::XMLElement* ptempAttr=ptempobject->FirstChildElement();ptempAttr;ptempAttr = ptempAttr->NextSiblingElement())
						{
							if(ptempAttr->Attribute("Type"))
								type= (atoi(ptempAttr->Attribute("Type")));
							for(tinyxml2::XMLElement* ptempElement=ptempAttr->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
							{
							if(!strcmp(ptempElement->Value(),"TexRECT"))
							{
								int l,t,r,b;
								if(ptempElement->Attribute("Left"))
									l= (atoi(ptempElement->Attribute("Left")));			
								if(ptempElement->Attribute("Top"))
									t= (atoi(ptempElement->Attribute("Top")));		
								if(ptempElement->Attribute("Right"))
									r= (atoi(ptempElement->Attribute("Right")));					
								if(ptempElement->Attribute("Bottom"))
									b= (atoi(ptempElement->Attribute("Bottom")));	
								SetRect( &rcTexture, l, t, r, b );	
								Element.SetTexture( 0, &rcTexture );
							}else
							if(!strcmp(ptempElement->Value(),"TexColorNormal"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB(a, r, g, b);
							}else
							if(!strcmp(ptempElement->Value(),"TexColorPress"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_PRESSED ] = D3DCOLOR_ARGB(a, r, g, b);
							}else
							if(!strcmp(ptempElement->Value(),"FontColorMouseOver"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.FontColor.States[ MANGO_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(a, r, g, b);
							}
							}
							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_BUTTON, 0, &Element, type );
						}
					}else
					{
						if(!strcmp(ptempobject->Value(),"ButtonPress"))
						{
							for(tinyxml2::XMLElement* ptempAttr=ptempobject->FirstChildElement();ptempAttr;ptempAttr = ptempAttr->NextSiblingElement())
							{
								if(ptempAttr->Attribute("Type"))
									type= (atoi(ptempAttr->Attribute("Type")));
								for(tinyxml2::XMLElement* ptempElement=ptempAttr->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
								{
								if(!strcmp(ptempElement->Value(),"TexRECT"))
								{
									int l,t,r,b;
									if(ptempElement->Attribute("Left"))
										l= (atoi(ptempElement->Attribute("Left")));			
									if(ptempElement->Attribute("Top"))
										t= (atoi(ptempElement->Attribute("Top")));		
									if(ptempElement->Attribute("Right"))
										r= (atoi(ptempElement->Attribute("Right")));					
									if(ptempElement->Attribute("Bottom"))
										b= (atoi(ptempElement->Attribute("Bottom")));	
									SetRect( &rcTexture, l, t, r, b );	
									Element.SetTexture( 0, &rcTexture, D3DCOLOR_ARGB(0, 255, 255, 255) );
								}else
								if(!strcmp(ptempElement->Value(),"FontColorMouseOver"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_MOUSEOVER ] = D3DCOLOR_ARGB(a, r, g, b);
								}else
								if(!strcmp(ptempElement->Value(),"TexColorPress"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_PRESSED ] = D3DCOLOR_ARGB(a, r, g, b);
								}else
								if(!strcmp(ptempElement->Value(),"TexColorFocus"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB(a, r, g, b);
								}	
								}
								// Assign the Element
								SetDefaultElement( MANGO_CONTROL_BUTTON, 1, &Element, type );
							}
						}

					}
				}

			}else
//--------------------------------------------------------------------------
//  CMANGOCheckBox
//--------------------------------------------------------------------------
			if(!strcmp(pRoot->Value(),"CheckBox"))
			{
				int ID,r,g,b,a;
				//初始化字体.
				if(pRoot->Attribute("FontID"))
					ID = (atoi(pRoot->Attribute("FontID")));
				for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
				{
					//--------------------------------------------------------------------------
					//  CMANGOCheckBox - CheckBoxNormal
					//--------------------------------------------------------------------------
				  if(!strcmp(ptempobject->Value(),"CheckBoxNormal"))
				  {
					for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
					{
						if(!strcmp(ptempElement->Value(),"TexRECT"))
						{
							int l,t,r,b;
							if(ptempElement->Attribute("Left"))
								l= (atoi(ptempElement->Attribute("Left")));			
							if(ptempElement->Attribute("Top"))
								t= (atoi(ptempElement->Attribute("Top")));		
							if(ptempElement->Attribute("Right"))
								r= (atoi(ptempElement->Attribute("Right")));					
							if(ptempElement->Attribute("Bottom"))
								b= (atoi(ptempElement->Attribute("Bottom")));	
							SetRect( &rcTexture, l, t, r, b );	
							Element.SetTexture( 0, &rcTexture );
						}else
						if(!strcmp(ptempElement->Value(),"FontColor"))
						{
							if(ptempElement->Attribute("R"))
								r= (atoi(ptempElement->Attribute("R")));			
							if(ptempElement->Attribute("G"))
								g= (atoi(ptempElement->Attribute("G")));					
							if(ptempElement->Attribute("B"))
								b= (atoi(ptempElement->Attribute("B")));			
							if(ptempElement->Attribute("A"))
								a= (atoi(ptempElement->Attribute("A")));
							
							Element.SetFont( ID, D3DCOLOR_ARGB(a, r, g, b), DT_LEFT | DT_VCENTER );
						}else
						if(!strcmp(ptempElement->Value(),"FontColorDisable"))
						{
							if(ptempElement->Attribute("R"))
								r= (atoi(ptempElement->Attribute("R")));			
							if(ptempElement->Attribute("G"))
								g= (atoi(ptempElement->Attribute("G")));					
							if(ptempElement->Attribute("B"))
								b= (atoi(ptempElement->Attribute("B")));			
							if(ptempElement->Attribute("A"))
								a= (atoi(ptempElement->Attribute("A")));
							Element.FontColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );
						}else
						if(!strcmp(ptempElement->Value(),"TexColor"))
						{
							if(ptempElement->Attribute("R"))
								r= (atoi(ptempElement->Attribute("R")));			
							if(ptempElement->Attribute("G"))
								g= (atoi(ptempElement->Attribute("G")));					
							if(ptempElement->Attribute("B"))
								b= (atoi(ptempElement->Attribute("B")));			
							if(ptempElement->Attribute("A"))
								a= (atoi(ptempElement->Attribute("A")));
							Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB(a, r, g, b);
						}else
						if(!strcmp(ptempElement->Value(),"TexColorPress"))
						{
							if(ptempElement->Attribute("R"))
								r= (atoi(ptempElement->Attribute("R")));			
							if(ptempElement->Attribute("G"))
								g= (atoi(ptempElement->Attribute("G")));					
							if(ptempElement->Attribute("B"))
								b= (atoi(ptempElement->Attribute("B")));			
							if(ptempElement->Attribute("A"))
								a= (atoi(ptempElement->Attribute("A")));
							Element.TextureColor.States[ MANGO_STATE_PRESSED ] = D3DCOLOR_ARGB(a, r, g, b);
						}else
						if(!strcmp(ptempElement->Value(),"TexColorFocus"))
						{
							if(ptempElement->Attribute("R"))
								r= (atoi(ptempElement->Attribute("R")));			
							if(ptempElement->Attribute("G"))
								g= (atoi(ptempElement->Attribute("G")));					
							if(ptempElement->Attribute("B"))
								b= (atoi(ptempElement->Attribute("B")));			
							if(ptempElement->Attribute("A"))
								a= (atoi(ptempElement->Attribute("A")));
							Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB(a, r, g, b);
						}	
					}
					// Assign the Element
					SetDefaultElement( MANGO_CONTROL_CHECKBOX, 0, &Element );
				  }else
				  //--------------------------------------------------------------------------
				  //  CMANGOCheckBox - CheckBoxSelect
				  //--------------------------------------------------------------------------
				  {
					 if(!strcmp(ptempobject->Value(),"CheckBoxSelect"))
					 {
						 for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
						 {
							 if(!strcmp(ptempElement->Value(),"TexRECT"))
							 {
								 int l,t,r,b;
								 if(ptempElement->Attribute("Left"))
									 l= (atoi(ptempElement->Attribute("Left")));			
								 if(ptempElement->Attribute("Top"))
									 t= (atoi(ptempElement->Attribute("Top")));		
								 if(ptempElement->Attribute("Right"))
									 r= (atoi(ptempElement->Attribute("Right")));					
								 if(ptempElement->Attribute("Bottom"))
									 b= (atoi(ptempElement->Attribute("Bottom")));	
								 SetRect( &rcTexture, l, t, r, b );	
								 Element.SetTexture( 0, &rcTexture );
							 }
						 }
						 // Assign the Element
						 SetDefaultElement( MANGO_CONTROL_CHECKBOX, 1, &Element );
					 }
				  }

				}
			}else
//--------------------------------------------------------------------------
//  CMANGORadioButton
//--------------------------------------------------------------------------
			if(!strcmp(pRoot->Value(),"RadioButton"))
			{
				int ID,r,g,b,a;
				//初始化字体.
				if(pRoot->Attribute("FontID"))
					ID = (atoi(pRoot->Attribute("FontID")));

				for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
				{
					if(!strcmp(ptempobject->Value(),"RadioButtonNormal"))
					{
						for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
						{
							if(!strcmp(ptempElement->Value(),"TexRECT"))
							{
								int l,t,r,b;
								if(ptempElement->Attribute("Left"))
									l= (atoi(ptempElement->Attribute("Left")));			
								if(ptempElement->Attribute("Top"))
									t= (atoi(ptempElement->Attribute("Top")));		
								if(ptempElement->Attribute("Right"))
									r= (atoi(ptempElement->Attribute("Right")));					
								if(ptempElement->Attribute("Bottom"))
									b= (atoi(ptempElement->Attribute("Bottom")));	
								SetRect( &rcTexture, l, t, r, b );	
								Element.SetTexture( 0, &rcTexture );
							}else
							if(!strcmp(ptempElement->Value(),"FontColor"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));

								Element.SetFont( ID, D3DCOLOR_ARGB(a, r, g, b), DT_LEFT | DT_VCENTER );
							}else
							if(!strcmp(ptempElement->Value(),"FontColorDisable"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.FontColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );
							}else
							if(!strcmp(ptempElement->Value(),"TexColor"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB(a, r, g, b);
							}else
							if(!strcmp(ptempElement->Value(),"TexColorPress"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_PRESSED ] = D3DCOLOR_ARGB(a, r, g, b);
							}else
							if(!strcmp(ptempElement->Value(),"TexColorFocus"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB(a, r, g, b);
							}	
						}
					// Assign the Element
					SetDefaultElement( MANGO_CONTROL_RADIOBUTTON, 0, &Element );
					}else
						//-------------------------------------
						// CMANGORadioButton - Check
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"RadioButtonSelect"))
						{
						   for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
						    {
								if(!strcmp(ptempElement->Value(),"TexRECT"))
								{
									int l,t,r,b;
									if(ptempElement->Attribute("Left"))
										l= (atoi(ptempElement->Attribute("Left")));			
									if(ptempElement->Attribute("Top"))
										t= (atoi(ptempElement->Attribute("Top")));		
									if(ptempElement->Attribute("Right"))
										r= (atoi(ptempElement->Attribute("Right")));					
									if(ptempElement->Attribute("Bottom"))
										b= (atoi(ptempElement->Attribute("Bottom")));	
									SetRect( &rcTexture, l, t, r, b );	
									Element.SetTexture( 0, &rcTexture );								
								}
						    }
							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_RADIOBUTTON, 1, &Element );
						}
				}
	
			}else
//--------------------------------------------------------------------------
// CMANGOComboBox
//--------------------------------------------------------------------------
			if(!strcmp(pRoot->Value(),"ComboBox"))
			{
				int ID,r,g,b,a;
				//初始化字体.
				if(pRoot->Attribute("FontID"))
					ID = (atoi(pRoot->Attribute("FontID")));
				Element.SetFont( 0 );

				for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
				{
					//-----------------------------------
					// CMANGOComboBox ---EditBox
					//-----------------------------------
					if(!strcmp(ptempobject->Value(),"ComboBoxEditBox"))
					{
						for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
						{
							if(!strcmp(ptempElement->Value(),"TexRECT"))
							{
								int l,t,r,b;
								if(ptempElement->Attribute("Left"))
									l= (atoi(ptempElement->Attribute("Left")));			
								if(ptempElement->Attribute("Top"))
									t= (atoi(ptempElement->Attribute("Top")));		
								if(ptempElement->Attribute("Right"))
									r= (atoi(ptempElement->Attribute("Right")));					
								if(ptempElement->Attribute("Bottom"))
									b= (atoi(ptempElement->Attribute("Bottom")));	
								SetRect( &rcTexture, l, t, r, b );	
								Element.SetTexture( 0, &rcTexture );	
							}else
							if(!strcmp(ptempElement->Value(),"FontColorMouseOver"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));

								Element.FontColor.States[ MANGO_STATE_MOUSEOVER ] = D3DCOLOR_ARGB( a, r, g, b );
							}else
							if(!strcmp(ptempElement->Value(),"FontColorDisable"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.FontColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );
							}else
							if(!strcmp(ptempElement->Value(),"FontColorPress"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.FontColor.States[ MANGO_STATE_PRESSED ] = D3DCOLOR_ARGB( a, r, g, b );
							}else
							if(!strcmp(ptempElement->Value(),"TexColor"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB( a, r, g, b );
							}else
							if(!strcmp(ptempElement->Value(),"TexColorFocus"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB( a, r, g, b );
							}else
							if(!strcmp(ptempElement->Value(),"TexColorDisable"))
							{
								if(ptempElement->Attribute("R"))
									r= (atoi(ptempElement->Attribute("R")));			
								if(ptempElement->Attribute("G"))
									g= (atoi(ptempElement->Attribute("G")));					
								if(ptempElement->Attribute("B"))
									b= (atoi(ptempElement->Attribute("B")));			
								if(ptempElement->Attribute("A"))
									a= (atoi(ptempElement->Attribute("A")));
								Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );
							}
						}
						// Assign the Element
						SetDefaultElement( MANGO_CONTROL_COMBOBOX, 0, &Element );		
					}else					
						//-----------------------------------
						// CMANGOComboBox ---Button
						//-----------------------------------
						if(!strcmp(ptempobject->Value(),"ComboBoxButton"))
						{
							for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
							{
								if(!strcmp(ptempElement->Value(),"TexRECT"))
								{
									int l,t,r,b;
									if(ptempElement->Attribute("Left"))
										l= (atoi(ptempElement->Attribute("Left")));			
									if(ptempElement->Attribute("Top"))
										t= (atoi(ptempElement->Attribute("Top")));		
									if(ptempElement->Attribute("Right"))
										r= (atoi(ptempElement->Attribute("Right")));					
									if(ptempElement->Attribute("Bottom"))
										b= (atoi(ptempElement->Attribute("Bottom")));	
									SetRect( &rcTexture, l, t, r, b );	
									Element.SetTexture( 0, &rcTexture );	
								}else
								if(!strcmp(ptempElement->Value(),"TexColor"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB( a, r, g, b );
								}else
								if(!strcmp(ptempElement->Value(),"TexColorFocus"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB( a, r, g, b );
								}else
								if(!strcmp(ptempElement->Value(),"TexColorDisable"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );
								}else
								if(!strcmp(ptempElement->Value(),"TexColorDisable"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB( a, r, g, b );
								}else
								if(!strcmp(ptempElement->Value(),"TexColorPress"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));
									Element.TextureColor.States[ MANGO_STATE_PRESSED ] = D3DCOLOR_ARGB( a, r, g, b );
								}
							}
							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_COMBOBOX, 1, &Element );
						}else	
						//-------------------------------------
						// CMANGOComboBox - Dropdown
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"ComboBoxDropdown"))
						{
							for(tinyxml2::XMLElement* ptempElement=ptempobject->FirstChildElement();ptempElement;ptempElement = ptempElement->NextSiblingElement())
							{
								if(!strcmp(ptempElement->Value(),"TexRECT"))
								{
									int l,t,r,b;
									if(ptempElement->Attribute("Left"))
										l= (atoi(ptempElement->Attribute("Left")));			
									if(ptempElement->Attribute("Top"))
										t= (atoi(ptempElement->Attribute("Top")));		
									if(ptempElement->Attribute("Right"))
										r= (atoi(ptempElement->Attribute("Right")));					
									if(ptempElement->Attribute("Bottom"))
										b= (atoi(ptempElement->Attribute("Bottom")));	
									SetRect( &rcTexture, l, t, r, b );	
									Element.SetTexture( 0, &rcTexture );	
								}else
								if(!strcmp(ptempElement->Value(),"FontColor"))
								{
									if(ptempElement->Attribute("R"))
										r= (atoi(ptempElement->Attribute("R")));			
									if(ptempElement->Attribute("G"))
										g= (atoi(ptempElement->Attribute("G")));					
									if(ptempElement->Attribute("B"))
										b= (atoi(ptempElement->Attribute("B")));			
									if(ptempElement->Attribute("A"))
										a= (atoi(ptempElement->Attribute("A")));									
									Element.SetFont( ID, D3DCOLOR_ARGB(a, r, g, b), DT_LEFT | DT_TOP );	
								}
							}
						// Assign the Element
						SetDefaultElement( MANGO_CONTROL_COMBOBOX, 2, &Element );
						}else
						//-------------------------------------
						// CMANGOComboBox - Selection
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"ComboBoxSelection"))
						{
							SetRect( &rcTexture, 12, 163, 239, 183 );
							Element.SetTexture( 0, &rcTexture );
							Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_COMBOBOX, 3, &Element );
						}
				}				
			}else
//--------------------------------------------------------------------------
// CMANGOSlider
//--------------------------------------------------------------------------
				if(!strcmp(pRoot->Value(),"Slider"))
				{	
					//-------------------------------------
					// CMANGOSlider - Track
					//-------------------------------------
					for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
					{
						if(!strcmp(ptempobject->Value(),"SliderTrack"))
						{					
							SetRect( &rcTexture, 248, 155, 512, 171 );
							Element.SetTexture( 0, &rcTexture );
							Element.TextureColor.States[ MANGO_STATE_NORMAL ] = D3DCOLOR_ARGB(150, 255, 255, 255);
							Element.TextureColor.States[ MANGO_STATE_FOCUS ] = D3DCOLOR_ARGB(200, 255, 255, 255);
							Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB(70, 255, 255, 255);

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_SLIDER, 0, &Element );

						}else
						//-------------------------------------
						// CMANGOSlider - Button
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"SliderButton"))
						{
							SetRect( &rcTexture, 192, 135, 215, 160 );
							Element.SetTexture( 0, &rcTexture );

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_SLIDER, 1, &Element );
						}

					}

				}else
//--------------------------------------------------------------------------
// CMANGOScrollBar - Track
//--------------------------------------------------------------------------
				if(!strcmp(pRoot->Value(),"ScrollBar"))
				{		
					int nScrollBarStartX = 267;
					int nScrollBarStartY = 3;
					//-------------------------------------
					// CMANGOScrollBar - Track
					//-------------------------------------
					for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
					{
						if(!strcmp(ptempobject->Value(),"ScrollBarTrack"))
						{					
							SetRect( &rcTexture,nScrollBarStartX, 25, 282, 129  );
							Element.SetTexture( 0, &rcTexture );
							Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_SCROLLBAR, 0, &Element );

						}else
						//-------------------------------------
						// CMANGOScrollBar - Up Arrow
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"ScrollBarUpArrow"))
						{
							SetRect( &rcTexture,nScrollBarStartX, nScrollBarStartY, nScrollBarStartX + 15, nScrollBarStartY + 22  );
							Element.SetTexture( 0, &rcTexture );
							Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_SCROLLBAR, 1, &Element );
						}else
						//-------------------------------------
						// CMANGOScrollBar - Down Arrow
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"ScrollBarDownArrow"))
						{
							SetRect( &rcTexture, nScrollBarStartX, 131, nScrollBarStartX + 15, 131+22 );
							Element.SetTexture( 0, &rcTexture );
							Element.TextureColor.States[ MANGO_STATE_DISABLED ] = D3DCOLOR_ARGB(255, 200, 200, 200);


							//// Assign the Element
							SetDefaultElement( MANGO_CONTROL_SCROLLBAR, 2, &Element );
						}else
						//-------------------------------------
						// CMANGOScrollBar - Button
						//-------------------------------------
						if(!strcmp(ptempobject->Value(),"ScrollBarButton"))
						{
							SetRect( &rcTexture, 309, 50, 321, 77 );
							Element.SetTexture( 0, &rcTexture );

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_SCROLLBAR, 3, &Element );
						}
					}
				}else
//--------------------------------------------------------------------------
// CMANGOEditBox
// Element assignment:
//   0 - text area
//   1 - top left border
//   2 - top border
//   3 - top right border
//   4 - left border
//   5 - right border
//   6 - lower left border
//   7 - lower border
//   8 - lower right border
//--------------------------------------------------------------------------
				if(!strcmp(pRoot->Value(),"EditBox"))
				{
					Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 0, 0 ), DT_LEFT | DT_TOP );

					// Assign the style
					SetRect( &rcTexture, 100, 113, 130, 130 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 0, &Element );
					SetRect( &rcTexture, 0, 105, 5, 113 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 1, &Element );
					SetRect( &rcTexture, 5, 105, 35, 113 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 2, &Element );
					SetRect( &rcTexture, 183, 105, 190, 113 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 3, &Element );
					SetRect( &rcTexture, 0, 113, 5, 123 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 4, &Element );
					SetRect( &rcTexture, 186, 113, 190, 123 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 5, &Element );
					SetRect( &rcTexture, 0, 126, 5, 133 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 6, &Element );
					SetRect( &rcTexture, 5, 126, 35, 133 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 7, &Element );
					SetRect( &rcTexture, 186, 126, 190, 133 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_EDITBOX, 8, &Element );
				}else
//--------------------------------------------------------------------------
// CMANGOIMEEditBox
//--------------------------------------------------------------------------
				if(!strcmp(pRoot->Value(),"IMEEditBox"))
				{
					Element.SetFont( 0, D3DCOLOR_ARGB( 255, 0, 255, 0 ), DT_LEFT | DT_TOP );

					// Assign the style
					SetRect( &rcTexture, 100, 113, 130, 130 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 0, &Element );
					SetRect( &rcTexture, 0, 105, 5, 113 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 1, &Element );
					SetRect( &rcTexture, 5, 105, 35, 113 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 2, &Element );
					SetRect( &rcTexture, 183, 105, 190, 113 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 3, &Element );
					SetRect( &rcTexture, 0, 113, 5, 123 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 4, &Element );
					SetRect( &rcTexture, 186, 113, 190, 123 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 5, &Element );
					SetRect( &rcTexture, 0, 126, 5, 133 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 6, &Element );
					SetRect( &rcTexture, 5, 126, 35, 133 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 7, &Element );
					SetRect( &rcTexture, 186, 126, 190, 133 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 8, &Element );

					// Element 9 for IME text, and indicator button
					SetRect( &rcTexture, 100, 0, 128, 42 );
					Element.SetTexture( 0, &rcTexture );
					SetDefaultElement( MANGO_CONTROL_IMEEDITBOX, 9, &Element );
				}else
//--------------------------------------------------------------------------
// CMANGOListBox
//--------------------------------------------------------------------------
				if(!strcmp(pRoot->Value(),"ListBox"))
				{
					for(tinyxml2::XMLElement* ptempobject=pRoot->FirstChildElement();ptempobject;ptempobject = ptempobject->NextSiblingElement())
					{
						//--------------------------------
						// CMANGOListBox - Main
						//--------------------------------
						if(!strcmp(ptempobject->Value(),"ListBoxMain"))
						{
							SetRect( &rcTexture, 13, 123, 241, 160 );
							Element.SetTexture( 0, &rcTexture );
							Element.SetFont( 0, D3DCOLOR_ARGB(255, 0, 0, 0), DT_LEFT | DT_TOP );

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_LISTBOX, 0, &Element );
						}else
						//--------------------------------
						// CMANGOListBox - Selection
						//--------------------------------
						if(!strcmp(ptempobject->Value(),"ListBoxSelection"))
						{
							SetRect( &rcTexture, 0, 162, 160, 180 );
							Element.SetTexture( 0, &rcTexture );
							Element.SetFont( 0, D3DCOLOR_ARGB(255, 255, 255, 255), DT_LEFT | DT_TOP );

							// Assign the Element
							SetDefaultElement( MANGO_CONTROL_LISTBOX, 1, &Element );
						}
					}
				}

		}
	}
}