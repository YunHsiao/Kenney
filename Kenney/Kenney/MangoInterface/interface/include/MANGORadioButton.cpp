#include "..\Mango.h"
#include ".\mangoradiobutton.h"


//--------------------------------------------------------------------------------------
// CMANGORadioButton class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CMANGORadioButton::CMANGORadioButton( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_RADIOBUTTON;
	m_pDialog = pDialog;
}



//--------------------------------------------------------------------------------------
bool CMANGORadioButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

					m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );
					m_bChecked = !m_bChecked;

					m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, true, this );
				}
				return true;
			}
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGORadioButton::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
				{
					m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );
					m_bChecked = !m_bChecked;

					m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, true, this );
				}

				return true;
			}

			break;
		}
	};

	return false;
}

//--------------------------------------------------------------------------------------
void CMANGORadioButton::SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput )
{
	if( bChecked && bClearGroup )
		m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );

	m_bChecked = bChecked;
	m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, bFromInput, this );
}



