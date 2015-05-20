#include "..\Mango.h"
#include ".\mangoscrollbar.h"

//--------------------------------------------------------------------------------------
// CMANGOScrollBar class
//--------------------------------------------------------------------------------------

// Minimum scroll bar thumb size
#define SCROLLBAR_MINTHUMBSIZE 8

// Delay and repeat period when clicking on the scroll bar arrows
#define SCROLLBAR_ARROWCLICK_DELAY  0.33
#define SCROLLBAR_ARROWCLICK_REPEAT 0.05

//--------------------------------------------------------------------------------------
CMANGOScrollBar::CMANGOScrollBar( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_SCROLLBAR;
	m_pDialog = pDialog;

	m_bShowThumb = true;
	m_bDrag = false;

	SetRect( &m_rcUpButton, 0, 0, 0, 0 );
	SetRect( &m_rcDownButton, 0, 0, 0, 0 );
	SetRect( &m_rcTrack, 0, 0, 0, 0 );
	SetRect( &m_rcThumb, 0, 0, 0, 0 );
	m_nPosition = 0;
	m_nPageSize = 1;
	m_nStart = 0;
	m_nEnd = 1;
	m_Arrow = CLEAR;
	m_dArrowTS = 0.0;
}


//--------------------------------------------------------------------------------------
CMANGOScrollBar::~CMANGOScrollBar()
{
}


//--------------------------------------------------------------------------------------
void CMANGOScrollBar::UpdateRects()
{
	CMANGOControl::UpdateRects();

	// Make the buttons square

	SetRect( &m_rcUpButton, m_rcBoundingBox.left, m_rcBoundingBox.top,
		m_rcBoundingBox.right, m_rcBoundingBox.top + RectWidth( m_rcBoundingBox ) );
	SetRect( &m_rcDownButton, m_rcBoundingBox.left, m_rcBoundingBox.bottom - RectWidth( m_rcBoundingBox ),
		m_rcBoundingBox.right, m_rcBoundingBox.bottom );
	SetRect( &m_rcTrack, m_rcUpButton.left, m_rcUpButton.bottom,
		m_rcDownButton.right, m_rcDownButton.top );
	m_rcThumb.left = m_rcUpButton.left;
	m_rcThumb.right = m_rcUpButton.right;

	UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
// Compute the dimension of the scroll thumb
void CMANGOScrollBar::UpdateThumbRect()
{
	if( m_nEnd - m_nStart > m_nPageSize )
	{
		int nThumbHeight = __max( RectHeight( m_rcTrack ) * m_nPageSize / ( m_nEnd - m_nStart ), SCROLLBAR_MINTHUMBSIZE );
		int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
		m_rcThumb.top = m_rcTrack.top + ( m_nPosition - m_nStart ) * ( RectHeight( m_rcTrack ) - nThumbHeight )
			/ nMaxPosition;
		m_rcThumb.bottom = m_rcThumb.top + nThumbHeight;
		m_bShowThumb = true;

	} 
	else
	{
		// No content to scroll
		m_rcThumb.bottom = m_rcThumb.top;
		m_bShowThumb = false;
	}
}


//--------------------------------------------------------------------------------------
// Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
// value scrolls up.
void CMANGOScrollBar::Scroll( int nDelta )
{
	// Perform scroll
	m_nPosition += nDelta;

	// Cap position
	Cap();

	// Update thumb position
	UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
void CMANGOScrollBar::ShowItem( int nIndex )
{
	// Cap the index

	if( nIndex < 0 )
		nIndex = 0;

	if( nIndex >= m_nEnd )
		nIndex = m_nEnd - 1;

	// Adjust position

	if( m_nPosition > nIndex )
		m_nPosition = nIndex;
	else
		if( m_nPosition + m_nPageSize <= nIndex )
			m_nPosition = nIndex - m_nPageSize + 1;

	UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
bool CMANGOScrollBar::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOScrollBar::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	static int ThumbOffsetY;

	m_LastMouse = pt;
	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			// Check for click on up button

			if( PtInRect( &m_rcUpButton, pt ) )
			{
				SetCapture( MANGOGetHWND() );
				if( m_nPosition > m_nStart )
					--m_nPosition;
				UpdateThumbRect();
				m_Arrow = CLICKED_UP;
				m_dArrowTS = MANGOGetTime();
				return true;
			}

			// Check for click on down button

			if( PtInRect( &m_rcDownButton, pt ) )
			{
				SetCapture( MANGOGetHWND() );
				if( m_nPosition + m_nPageSize < m_nEnd )
					++m_nPosition;
				UpdateThumbRect();
				m_Arrow = CLICKED_DOWN;
				m_dArrowTS = MANGOGetTime();
				return true;
			}

			// Check for click on thumb

			if( PtInRect( &m_rcThumb, pt ) )
			{
				SetCapture( MANGOGetHWND() );
				m_bDrag = true;
				ThumbOffsetY = pt.y - m_rcThumb.top;
				return true;
			}

			// Check for click on track

			if( m_rcThumb.left <= pt.x &&
				m_rcThumb.right > pt.x )
			{
				SetCapture( MANGOGetHWND() );
				if( m_rcThumb.top > pt.y &&
					m_rcTrack.top <= pt.y )
				{
					Scroll( -( m_nPageSize - 1 ) );
					return true;
				} else
					if( m_rcThumb.bottom <= pt.y &&
						m_rcTrack.bottom > pt.y )
					{
						Scroll( m_nPageSize - 1 );
						return true;
					}
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			m_bDrag = false;
			ReleaseCapture();
			UpdateThumbRect();
			m_Arrow = CLEAR;
			break;
		}

	case WM_MOUSEMOVE:
		{
			if( m_bDrag )
			{
				m_rcThumb.bottom += pt.y - ThumbOffsetY - m_rcThumb.top;
				m_rcThumb.top = pt.y - ThumbOffsetY;
				if( m_rcThumb.top < m_rcTrack.top )
					OffsetRect( &m_rcThumb, 0, m_rcTrack.top - m_rcThumb.top );
				else
					if( m_rcThumb.bottom > m_rcTrack.bottom )
						OffsetRect( &m_rcThumb, 0, m_rcTrack.bottom - m_rcThumb.bottom );

				// Compute first item index based on thumb position

				int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;  // Largest possible index for first item
				int nMaxThumb = RectHeight( m_rcTrack ) - RectHeight( m_rcThumb );  // Largest possible thumb position from the top

				m_nPosition = m_nStart +
					( m_rcThumb.top - m_rcTrack.top +
					nMaxThumb / ( nMaxFirstItem * 2 ) ) * // Shift by half a row to avoid last row covered by only one pixel
					nMaxFirstItem  / nMaxThumb;

				return true;
			}

			break;
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOScrollBar::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( WM_CAPTURECHANGED == uMsg )
	{
		// The application just lost mouse capture. We may not have gotten
		// the WM_MOUSEUP message, so reset m_bDrag here.
		if( (HWND)lParam != MANGOGetHWND() )
			m_bDrag = false;
	}

	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOScrollBar::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	// Check if the arrow button has been held for a while.
	// If so, update the thumb position to simulate repeated
	// scroll.
	if( m_Arrow != CLEAR )
	{
		double dCurrTime = MANGOGetTime();
		if( PtInRect( &m_rcUpButton, m_LastMouse ) )
		{
			switch( m_Arrow )
			{
			case CLICKED_UP:
				if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
				{
					Scroll( -1 );
					m_Arrow = HELD_UP;
					m_dArrowTS = dCurrTime;
				}
				break;
			case HELD_UP:
				if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
				{
					Scroll( -1 );
					m_dArrowTS = dCurrTime;
				}
				break;
			}
		} else
			if( PtInRect( &m_rcDownButton, m_LastMouse ) )
			{
				switch( m_Arrow )
				{
				case CLICKED_DOWN:
					if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_Arrow = HELD_DOWN;
						m_dArrowTS = dCurrTime;
					}
					break;
				case HELD_DOWN:
					if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_dArrowTS = dCurrTime;
					}
					break;
				}
			}
	}

	MANGO_CONTROL_STATE iState = MANGO_STATE_NORMAL;

	if( m_bVisible == false )
		iState = MANGO_STATE_HIDDEN;
	else if( m_bEnabled == false || m_bShowThumb == false )
		iState = MANGO_STATE_DISABLED;
	else if( m_bMouseOver )
		iState = MANGO_STATE_MOUSEOVER;
	else if( m_bHasFocus )
		iState = MANGO_STATE_FOCUS;


	float fBlendRate = ( iState == MANGO_STATE_PRESSED ) ? 0.0f : 0.8f;

	// Background track layer
	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcTrack );

	// Up Arrow
	pElement = m_Elements.GetAt( 1 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcUpButton );

	// Down Arrow
	pElement = m_Elements.GetAt( 2 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcDownButton );

	// Thumb button
	pElement = m_Elements.GetAt( 3 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcThumb );

}


//--------------------------------------------------------------------------------------
void CMANGOScrollBar::SetTrackRange( int nStart, int nEnd )
{
	m_nStart = nStart; m_nEnd = nEnd;
	Cap();
	UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
void CMANGOScrollBar::Cap()  // Clips position at boundaries. Ensures it stays within legal range.
{
	if( m_nPosition < m_nStart ||
		m_nEnd - m_nStart <= m_nPageSize )
	{
		m_nPosition = m_nStart;
	}
	else
		if( m_nPosition + m_nPageSize > m_nEnd )
			m_nPosition = m_nEnd - m_nPageSize;
}
