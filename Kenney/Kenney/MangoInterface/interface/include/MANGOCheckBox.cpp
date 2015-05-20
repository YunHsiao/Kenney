


//--------------------------------------------------------------------------------------
// CMANGOCheckBox class
//--------------------------------------------------------------------------------------
#include "..\Mango.h"
#include ".\mangocheckbox.h"
//--------------------------------------------------------------------------------------
CMANGOCheckBox::CMANGOCheckBox( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_CHECKBOX;
	m_pDialog = pDialog;

	m_bChecked = false;
}


//--------------------------------------------------------------------------------------
bool CMANGOCheckBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_SPACE:
				m_bPressed = true;
				return true;
			}
		}

	case WM_KEYUP:
		{
			switch( wParam )
			{
			case VK_SPACE:
				if( m_bPressed == true )
				{
					m_bPressed = false;
					SetCheckedInternal( !m_bChecked, true );
				}
				return true;
			}
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOCheckBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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

				// Button click
				if( ContainsPoint( pt ) )
					SetCheckedInternal( !m_bChecked, true );

				return true;
			}

			break;
		}
	};

	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOCheckBox::SetCheckedInternal( bool bChecked, bool bFromInput ) 
{ 
	m_bChecked = bChecked; 

	m_pDialog->SendEvent( EVENT_CHECKBOX_CHANGED, bFromInput, this ); 
}


//--------------------------------------------------------------------------------------
BOOL CMANGOCheckBox::ContainsPoint( POINT pt ) 
{ 
	return ( PtInRect( &m_rcBoundingBox, pt ) || 
		PtInRect( &m_rcButton, pt ) ); 
}



//--------------------------------------------------------------------------------------
void CMANGOCheckBox::UpdateRects()
{
	CMANGOButton::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + RectHeight( m_rcButton );

	m_rcText = m_rcBoundingBox;
	m_rcText.left += (int) ( 1.25f * RectWidth( m_rcButton ) );
}



//--------------------------------------------------------------------------------------
void CMANGOCheckBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	MANGO_CONTROL_STATE iState = MANGO_STATE_NORMAL;

	if( m_bVisible == false )
		iState = MANGO_STATE_HIDDEN;
	else if( m_bEnabled == false )
		iState = MANGO_STATE_DISABLED;
	else if( m_bPressed )
		iState = MANGO_STATE_PRESSED;
	else if( m_bMouseOver )
		iState = MANGO_STATE_MOUSEOVER;
	else if( m_bHasFocus )
		iState = MANGO_STATE_FOCUS;

	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	float fBlendRate = ( iState == MANGO_STATE_PRESSED ) ? 0.0f : 0.8f;

	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

	m_pDialog->DrawSprite( pElement, &m_rcButton );
	m_pDialog->DrawText( m_strText, pElement, &m_rcText, true );

	if( !m_bChecked )
		//iState = MANGO_STATE_HIDDEN;
		return;

	pElement = m_Elements.GetAt( 1 );

	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcButton );
}


