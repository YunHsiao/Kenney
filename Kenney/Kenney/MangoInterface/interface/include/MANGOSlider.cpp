#include "..\Mango.h"
#include ".\mangoslider.h"


//--------------------------------------------------------------------------------------
CMANGOSlider::CMANGOSlider( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_SLIDER;
	m_pDialog = pDialog;

	m_nMin = 0;
	m_nMax = 100;
	m_nValue = 50;

	m_bPressed = false;
}


//--------------------------------------------------------------------------------------
BOOL CMANGOSlider::ContainsPoint( POINT pt ) 
{ 
	return ( PtInRect( &m_rcBoundingBox, pt ) || 
		PtInRect( &m_rcButton, pt ) ); 
}


//--------------------------------------------------------------------------------------
void CMANGOSlider::UpdateRects()
{
	CMANGOControl::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + RectHeight( m_rcButton );
	OffsetRect( &m_rcButton, -RectWidth( m_rcButton )/2, 0 );

	m_nButtonX = (int) ( (m_nValue - m_nMin) * (float)RectWidth( m_rcBoundingBox ) / (m_nMax - m_nMin) );
	OffsetRect( &m_rcButton, m_nButtonX, 0 );
}

int CMANGOSlider::ValueFromPos( int x )
{ 
	float fValuePerPixel = (float)(m_nMax - m_nMin) / RectWidth( m_rcBoundingBox );
	return (int) (0.5f + m_nMin + fValuePerPixel * (x - m_rcBoundingBox.left)) ; 
}

//--------------------------------------------------------------------------------------
bool CMANGOSlider::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_HOME:
				SetValueInternal( m_nMin, true );
				return true;

			case VK_END:
				SetValueInternal( m_nMax, true );
				return true;

			case VK_LEFT:
			case VK_DOWN:
				SetValueInternal( m_nValue - 1, true );
				return true;

			case VK_RIGHT:
			case VK_UP:
				SetValueInternal( m_nValue + 1, true );
				return true;

			case VK_NEXT:
				SetValueInternal( m_nValue - ( 10 > (m_nMax - m_nMin) / 10 ? 10 : (m_nMax - m_nMin) / 10 ), true );
				return true;

			case VK_PRIOR:
				SetValueInternal( m_nValue + ( 10 > (m_nMax - m_nMin) / 10 ? 10 : (m_nMax - m_nMin) / 10 ), true );
				return true;
			}
			break;
		}
	}


	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOSlider::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( PtInRect( &m_rcButton, pt ) )
			{
				// Pressed while inside the control
				m_bPressed = true;
				SetCapture( MANGOGetHWND() );

				m_nDragX = pt.x;
				//m_nDragY = pt.y;
				m_nDragOffset = m_nButtonX - m_nDragX;

				//m_nDragValue = m_nValue;

				if( !m_bHasFocus )
					m_pDialog->RequestFocus( this );

				return true;
			}

			if( PtInRect( &m_rcBoundingBox, pt ) )
			{
				m_nDragX = pt.x;
				m_nDragOffset = 0;               
				m_bPressed = true;

				if( !m_bHasFocus )
					m_pDialog->RequestFocus( this );

				if( pt.x > m_nButtonX + m_x )
				{
					SetValueInternal( m_nValue + 1, true );
					return true;
				}

				if( pt.x < m_nButtonX + m_x )
				{
					SetValueInternal( m_nValue - 1, true );
					return true;
				}
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
				ReleaseCapture();
				m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, true, this );

				return true;
			}

			break;
		}

	case WM_MOUSEMOVE:
		{
			if( m_bPressed )
			{
				SetValueInternal( ValueFromPos( m_x + pt.x + m_nDragOffset ), true );
				return true;
			}

			break;
		}

	case WM_MOUSEWHEEL:
		{
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA;
			SetValueInternal( m_nValue - nScrollAmount, true );
			return true;
		}
	};

	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOSlider::SetRange( int nMin, int nMax ) 
{
	m_nMin = nMin;
	m_nMax = nMax;

	SetValueInternal( m_nValue, false );
}


//--------------------------------------------------------------------------------------
void CMANGOSlider::SetValueInternal( int nValue, bool bFromInput )
{
	// Clamp to range
	nValue = __max( m_nMin, nValue );
	nValue = __min( m_nMax, nValue );

	if( nValue == m_nValue )
		return;

	m_nValue = nValue;
	UpdateRects();

	m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, bFromInput, this );
}


//--------------------------------------------------------------------------------------
void CMANGOSlider::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	MANGO_CONTROL_STATE iState = MANGO_STATE_NORMAL;

	if( m_bVisible == false )
	{
		iState = MANGO_STATE_HIDDEN;
	}
	else if( m_bEnabled == false )
	{
		iState = MANGO_STATE_DISABLED;
	}
	else if( m_bPressed )
	{
		iState = MANGO_STATE_PRESSED;
	}
	else if( m_bMouseOver )
	{
		iState = MANGO_STATE_MOUSEOVER;
	}
	else if( m_bHasFocus )
	{
		iState = MANGO_STATE_FOCUS;
	}

	float fBlendRate = ( iState == MANGO_STATE_PRESSED ) ? 0.0f : 0.8f;

	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate ); 
	m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );

	//TODO: remove magic numbers
	pElement = m_Elements.GetAt( 1 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcButton );
}


