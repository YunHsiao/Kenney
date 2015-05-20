#include "..\Mango.h"
#include ".\mangolistbox.h"
//--------------------------------------------------------------------------------------
// CMANGOListBox class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CMANGOListBox::CMANGOListBox( CMANGOScene *pDialog ) :
m_ScrollBar( pDialog )
{
	m_Type = MANGO_CONTROL_LISTBOX;
	m_pDialog = pDialog;

	m_dwStyle = 0;
	m_nSBWidth = 16;
	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
}


//--------------------------------------------------------------------------------------
CMANGOListBox::~CMANGOListBox()
{
	RemoveAllItems();
}


//--------------------------------------------------------------------------------------
void CMANGOListBox::UpdateRects()
{
	CMANGOControl::UpdateRects();

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.right -= m_nSBWidth;
	InflateRect( &m_rcSelection, -m_nBorder, -m_nBorder );
	m_rcText = m_rcSelection;
	InflateRect( &m_rcText, -m_nMargin, 0 );

	// Update the scrollbar's rects
	m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top );
	m_ScrollBar.SetSize( m_nSBWidth, m_height );
	MANGOFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( m_Elements.GetAt( 0 )->iFont );
	if( pFontNode && pFontNode->nHeight )
	{
		m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / pFontNode->nHeight );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_nSelected );
	}
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOListBox::AddItem( const WCHAR *wszText, void *pData )
{
	MANGOListBoxItem *pNewItem = new MANGOListBoxItem;
	if( !pNewItem )
		return E_OUTOFMEMORY;

	StringCchCopy( pNewItem->strText, 256, wszText );
	pNewItem->pData = pData;
	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->bSelected = false;

	HRESULT hr = m_Items.Add( pNewItem );
	if( FAILED(hr) )
	{
		SAFE_DELETE( pNewItem );
	}
	else
	{
		m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
	}

	return hr;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOListBox::InsertItem( int nIndex, const WCHAR *wszText, void *pData )
{
	MANGOListBoxItem *pNewItem = new MANGOListBoxItem;
	if( !pNewItem )
		return E_OUTOFMEMORY;

	StringCchCopy( pNewItem->strText, 256, wszText );
	pNewItem->pData = pData;
	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->bSelected = false;

	HRESULT hr = m_Items.Insert( nIndex, pNewItem );
	if( SUCCEEDED( hr ) )
		m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
	else
		SAFE_DELETE( pNewItem );

	return hr;
}


//--------------------------------------------------------------------------------------
void CMANGOListBox::RemoveItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
		return;

	MANGOListBoxItem *pItem = m_Items.GetAt( nIndex );

	delete pItem;
	m_Items.Remove( nIndex );
	m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
	if( m_nSelected >= (int)m_Items.GetSize() )
		m_nSelected = m_Items.GetSize() - 1;

	m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}


//--------------------------------------------------------------------------------------
void CMANGOListBox::RemoveItemByText( WCHAR *wszText )
{
}


//--------------------------------------------------------------------------------------
void CMANGOListBox::RemoveItemByData( void *pData )
{
}


//--------------------------------------------------------------------------------------
void CMANGOListBox::RemoveAllItems()
{
	for( int i = 0; i < m_Items.GetSize(); ++i )
	{
		MANGOListBoxItem *pItem = m_Items.GetAt( i );
		delete pItem;
	}

	m_Items.RemoveAll();
	m_ScrollBar.SetTrackRange( 0, 1 );
}


//--------------------------------------------------------------------------------------
MANGOListBoxItem *CMANGOListBox::GetItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_Items.GetSize() )
		return NULL;

	return m_Items[nIndex];
}


//--------------------------------------------------------------------------------------
// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
int CMANGOListBox::GetSelectedIndex( int nPreviousSelected )
{
	if( nPreviousSelected < -1 )
		return -1;

	if( m_dwStyle & MULTISELECTION )
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for( int i = nPreviousSelected + 1; i < (int)m_Items.GetSize(); ++i )
		{
			MANGOListBoxItem *pItem = m_Items.GetAt( i );

			if( pItem->bSelected )
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}


//--------------------------------------------------------------------------------------
void CMANGOListBox::SelectItem( int nNewIndex )
{
	// If no item exists, do nothing.
	if( m_Items.GetSize() == 0 )
		return;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if( m_nSelected < 0 )
		m_nSelected = 0;
	if( m_nSelected >= (int)m_Items.GetSize() )
		m_nSelected = m_Items.GetSize() - 1;

	if( nOldSelected != m_nSelected )
	{
		if( m_dwStyle & MULTISELECTION )
		{
			m_Items[m_nSelected]->bSelected = true;
		}

		// Update selection start
		m_nSelStart = m_nSelected;

		// Adjust scroll bar
		m_ScrollBar.ShowItem( m_nSelected );
	}

	m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
}


//--------------------------------------------------------------------------------------
bool CMANGOListBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
		return true;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_HOME:
		case VK_END:
			{
				// If no item exists, do nothing.
				if( m_Items.GetSize() == 0 )
					return true;

				int nOldSelected = m_nSelected;

				// Adjust m_nSelected
				switch( wParam )
				{
				case VK_UP: --m_nSelected; break;
				case VK_DOWN: ++m_nSelected; break;
				case VK_NEXT: m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
				case VK_PRIOR: m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
				case VK_HOME: m_nSelected = 0; break;
				case VK_END: m_nSelected = m_Items.GetSize() - 1; break;
				}

				// Perform capping
				if( m_nSelected < 0 )
					m_nSelected = 0;
				if( m_nSelected >= (int)m_Items.GetSize() )
					m_nSelected = m_Items.GetSize() - 1;

				if( nOldSelected != m_nSelected )
				{
					if( m_dwStyle & MULTISELECTION )
					{
						// Multiple selection

						// Clear all selection
						for( int i = 0; i < (int)m_Items.GetSize(); ++i )
						{
							MANGOListBoxItem *pItem = m_Items[i];
							pItem->bSelected = false;
						}

						if( GetKeyState( VK_SHIFT ) < 0 )
						{
							// Select all items from m_nSelStart to
							// m_nSelected
							int nEnd = __max( m_nSelStart, m_nSelected );

							for( int n = __min( m_nSelStart, m_nSelected ); n <= nEnd; ++n )
								m_Items[n]->bSelected = true;
						}
						else
						{
							m_Items[m_nSelected]->bSelected = true;

							// Update selection start
							m_nSelStart = m_nSelected;
						}
					} else
						m_nSelStart = m_nSelected;

					// Adjust scroll bar

					m_ScrollBar.ShowItem( m_nSelected );

					// Send notification

					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
				}
				return true;
			}

			// Space is the hotkey for double-clicking an item.
			//
		case VK_SPACE:
			m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
			return true;
		}
		break;
	}

	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg )
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
		return true;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		// Check for clicks in the text area
		if( m_Items.GetSize() > 0 && PtInRect( &m_rcSelection, pt ) )
		{
			// Compute the index of the clicked item

			int nClicked;
			if( m_nTextHeight )
				nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
			else
				nClicked = -1;

			// Only proceed if the click falls on top of an item.

			if( nClicked >= m_ScrollBar.GetTrackPos() &&
				nClicked < (int)m_Items.GetSize() &&
				nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
			{
				SetCapture( MANGOGetHWND() );
				m_bDrag = true;

				// If this is a double click, fire off an event and exit
				// since the first click would have taken care of the selection
				// updating.
				if( uMsg == WM_LBUTTONDBLCLK )
				{
					m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
					return true;
				}

				m_nSelected = nClicked;
				if( !( wParam & MK_SHIFT ) )
					m_nSelStart = m_nSelected;

				// If this is a multi-selection listbox, update per-item
				// selection data.

				if( m_dwStyle & MULTISELECTION )
				{
					// Determine behavior based on the state of Shift and Ctrl

					MANGOListBoxItem *pSelItem = m_Items.GetAt( m_nSelected );
					if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
					{
						// Control click. Reverse the selection of this item.

						pSelItem->bSelected = !pSelItem->bSelected;
					} else
						if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
						{
							// Shift click. Set the selection for all items
							// from last selected item to the current item.
							// Clear everything else.

							int nBegin = __min( m_nSelStart, m_nSelected );
							int nEnd = __max( m_nSelStart, m_nSelected );

							for( int i = 0; i < nBegin; ++i )
							{
								MANGOListBoxItem *pItem = m_Items.GetAt( i );
								pItem->bSelected = false;
							}

							for( int i = nEnd + 1; i < (int)m_Items.GetSize(); ++i )
							{
								MANGOListBoxItem *pItem = m_Items.GetAt( i );
								pItem->bSelected = false;
							}

							for( int i = nBegin; i <= nEnd; ++i )
							{
								MANGOListBoxItem *pItem = m_Items.GetAt( i );
								pItem->bSelected = true;
							}
						} else
							if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
							{
								// Control-Shift-click.

								// The behavior is:
								//   Set all items from m_nSelStart to m_nSelected to
								//     the same state as m_nSelStart, not including m_nSelected.
								//   Set m_nSelected to selected.

								int nBegin = __min( m_nSelStart, m_nSelected );
								int nEnd = __max( m_nSelStart, m_nSelected );

								// The two ends do not need to be set here.

								bool bLastSelected = m_Items.GetAt( m_nSelStart )->bSelected;
								for( int i = nBegin + 1; i < nEnd; ++i )
								{
									MANGOListBoxItem *pItem = m_Items.GetAt( i );
									pItem->bSelected = bLastSelected;
								}

								pSelItem->bSelected = true;

								// Restore m_nSelected to the previous value
								// This matches the Windows behavior

								m_nSelected = m_nSelStart;
							} else
							{
								// Simple click.  Clear all items and select the clicked
								// item.


								for( int i = 0; i < (int)m_Items.GetSize(); ++i )
								{
									MANGOListBoxItem *pItem = m_Items.GetAt( i );
									pItem->bSelected = false;
								}

								pSelItem->bSelected = true;
							}
				}  // End of multi-selection case

				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			}

			return true;
		}
		break;

	case WM_LBUTTONUP:
		{
			ReleaseCapture();
			m_bDrag = false;

			if( m_nSelected != -1 )
			{
				// Set all items between m_nSelStart and m_nSelected to
				// the same state as m_nSelStart
				int nEnd = __max( m_nSelStart, m_nSelected );

				for( int n = __min( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
					m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
				m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;

				// If m_nSelStart and m_nSelected are not the same,
				// the user has dragged the mouse to make a selection.
				// Notify the application of this.
				if( m_nSelStart != m_nSelected )
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );

				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION_END, true, this );
			}
			return false;
		}

	case WM_MOUSEMOVE:
		if( m_bDrag )
		{
			// Compute the index of the item below cursor

			int nItem;
			if( m_nTextHeight )
				nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
			else
				nItem = -1;

			// Only proceed if the cursor is on top of an item.

			if( nItem >= (int)m_ScrollBar.GetTrackPos() &&
				nItem < (int)m_Items.GetSize() &&
				nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
			{
				m_nSelected = nItem;
				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			} else
				if( nItem < (int)m_ScrollBar.GetTrackPos() )
				{
					// User drags the mouse above window top
					m_ScrollBar.Scroll( -1 );
					m_nSelected = m_ScrollBar.GetTrackPos();
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
				} else
					if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
					{
						// User drags the mouse below window bottom
						m_ScrollBar.Scroll( 1 );
						m_nSelected = __min( (int)m_Items.GetSize(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() ) - 1;
						m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
					}
		}
		break;

	case WM_MOUSEWHEEL:
		{
			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
			m_ScrollBar.Scroll( -nScrollAmount );
			return true;
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOListBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
void CMANGOListBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	if( m_bVisible == false )
		return;

	CMANGOElement* pElement = m_Elements.GetAt( 0 );
	pElement->TextureColor.Blend( MANGO_STATE_NORMAL, fElapsedTime );
	pElement->FontColor.Blend( MANGO_STATE_NORMAL, fElapsedTime );

	CMANGOElement* pSelElement = m_Elements.GetAt( 1 );
	pSelElement->TextureColor.Blend( MANGO_STATE_NORMAL, fElapsedTime );
	pSelElement->FontColor.Blend( MANGO_STATE_NORMAL, fElapsedTime );

	m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );

	// Render the text
	if( m_Items.GetSize() > 0 )
	{
		// Find out the height of a single line of text
		RECT rc = m_rcText;
		RECT rcSel = m_rcSelection;
		rc.bottom = rc.top + m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight;

		// Update the line height formation
		m_nTextHeight = rc.bottom - rc.top;

		static bool bSBInit;
		if( !bSBInit )
		{
			// Update the page size of the scroll bar
			if( m_nTextHeight )
				m_ScrollBar.SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
			else
				m_ScrollBar.SetPageSize( RectHeight( m_rcText ) );
			bSBInit = true;
		}

		rc.right = m_rcText.right;
		for( int i = m_ScrollBar.GetTrackPos(); i < (int)m_Items.GetSize(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;

			MANGOListBoxItem *pItem = m_Items.GetAt( i );

			// Determine if we need to render this item with the
			// selected element.
			bool bSelectedStyle = false;

			if( !( m_dwStyle & MULTISELECTION ) && i == m_nSelected )
				bSelectedStyle = true;
			else
				if( m_dwStyle & MULTISELECTION )
				{
					if( m_bDrag &&
						( ( i >= m_nSelected && i < m_nSelStart ) ||
						( i <= m_nSelected && i > m_nSelStart ) ) )
						bSelectedStyle = m_Items[m_nSelStart]->bSelected;
					else
						if( pItem->bSelected )
							bSelectedStyle = true;
				}

				if( bSelectedStyle )
				{
					rcSel.top = rc.top; rcSel.bottom = rc.bottom;
					m_pDialog->DrawSprite( pSelElement, &rcSel );
					m_pDialog->DrawText( pItem->strText, pSelElement, &rc );
				}
				else
					m_pDialog->DrawText( pItem->strText, pElement, &rc );

				OffsetRect( &rc, 0, m_nTextHeight );
		}
	}

	// Render the scroll bar

	m_ScrollBar.Render( pd3dDevice, fElapsedTime );
}
