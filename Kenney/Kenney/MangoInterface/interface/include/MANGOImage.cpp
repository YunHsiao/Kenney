#include "../Mango.h"

//--------------------------------------------------------------------------------------
// CMANGOImage class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------

CMANGOImage::CMANGOImage( CMANGOScene *pDialog ):
m_bPressed(false)
{
	m_Type = MANGO_CONTROL_IMAGE;
	m_pDialog = pDialog;
}

//--------------------------------------------------------------------------------------
void CMANGOImage::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{  
	//鼠标点击标题栏的时候会闪有待解决
	MANGO_CONTROL_STATE iState = MANGO_STATE_NORMAL;

	if( m_bVisible == false )
	{
		return;
	}
	else if( m_bEnabled == false )
	{
		iState = MANGO_STATE_DISABLED;
	}
	else if( m_bMouseOver )
	{
		iState = MANGO_STATE_MOUSEOVER;
	}
	else if( m_bHasFocus )
	{
		iState = MANGO_STATE_FOCUS;
	}

	float fBlendRate = (iState != MANGO_STATE_DISABLED)? 0.02f : 0.2f;

	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	//pElement->SetTexture( 0, &m_rectTexture);

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate ); 
	m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );
}

//--------------------------------------------------------------------------------------
bool CMANGOImage::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( ContainsPoint( pt ) )
			{
				// Pressed while inside the control
				m_bPressed = true;
				SetCapture( MANGOGetHWND() );

				if( !m_bHasFocus )
					m_pDialog->RequestFocus( this );

				return true;
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
				ReleaseCapture();

				if( !m_pDialog->m_bKeyboardInput )
					m_pDialog->ClearFocus();

				// Button click
				if( ContainsPoint( pt ) )
					m_pDialog->SendEvent( EVENT_IMAGE_CLICKED, true, this );

				return true;
			}

			break;
		}
	};

	return false;
}