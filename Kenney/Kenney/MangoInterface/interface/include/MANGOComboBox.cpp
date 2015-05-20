#include "..\Mango.h"
#include ".\mangocombobox.h"


//--------------------------------------------------------------------------------------
// CMANGOComboBox class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CMANGOComboBox::CMANGOComboBox( CMANGOScene *pDialog ) :
m_ScrollBar( pDialog )
{
	m_Type = MANGO_CONTROL_COMBOBOX;
	m_pDialog = pDialog;

	m_nDropHeight = 100;

	m_nSBWidth = 16;
	m_bOpened = false;
	m_iSelected = -1;
	m_iFocused = -1;
}


//--------------------------------------------------------------------------------------
CMANGOComboBox::~CMANGOComboBox()
{
	RemoveAllItems();
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::SetTextColor( D3DCOLOR Color )
{
	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	if( pElement )
		pElement->FontColor.States[MANGO_STATE_NORMAL] = Color;

	pElement = m_Elements.GetAt( 2 );

	if( pElement )
		pElement->FontColor.States[MANGO_STATE_NORMAL] = Color;
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::UpdateRects()
{

	CMANGOButton::UpdateRects();

	m_rcButton = m_rcBoundingBox;
	m_rcButton.left = m_rcButton.right - RectHeight( m_rcButton );

	m_rcText = m_rcBoundingBox;
	m_rcText.right = m_rcButton.left;

	m_rcDropdown = m_rcText;
	OffsetRect( &m_rcDropdown, 0, (int) (0.90f * RectHeight( m_rcText )) );
	m_rcDropdown.bottom += m_nDropHeight;
	m_rcDropdown.right += m_nSBWidth -15;		//Drop的宽度

	m_rcDropdownText = m_rcDropdown;
	m_rcDropdownText.left += (int) (0.1f * RectWidth( m_rcDropdown ));
	m_rcDropdownText.right -= (int) (0.1f * RectWidth( m_rcDropdown ));
	m_rcDropdownText.top += (int) (0.1f * RectHeight( m_rcDropdown ));
	m_rcDropdownText.bottom -= (int) (0.1f * RectHeight( m_rcDropdown ));

	// Update the scrollbar's rects
	m_ScrollBar.SetLocation( m_rcDropdown.right, m_rcDropdown.top+2 ,m_z);
	m_ScrollBar.SetSize( m_nSBWidth, RectHeight( m_rcDropdown )-2 );
	MANGOFontNode* pFontNode = m_pDialog->GetManager()->GetFontNode( m_Elements.GetAt( 2 )->iFont );
	if( pFontNode && pFontNode->nHeight )
	{
		m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / pFontNode->nHeight );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_iSelected );
	}
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::OnFocusOut()
{
	CMANGOButton::OnFocusOut();

	m_bOpened = false;
}


//--------------------------------------------------------------------------------------
bool CMANGOComboBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	const DWORD REPEAT_MASK = (0x40000000);

	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
		return true;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_RETURN:
				if( m_bOpened )
				{
					if( m_iSelected != m_iFocused )
					{
						m_iSelected = m_iFocused;
						m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
					}
					m_bOpened = false;

					if( !m_pDialog->m_bKeyboardInput )
						m_pDialog->ClearFocus();

					return true;
				}
				break;

			case VK_F4:
				// Filter out auto-repeats
				if( lParam & REPEAT_MASK )
					return true;

				m_bOpened = !m_bOpened;

				if( !m_bOpened )
				{
					m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );

					if( !m_pDialog->m_bKeyboardInput )
						m_pDialog->ClearFocus();
				}

				return true;

			case VK_LEFT:
			case VK_UP:
				if( m_iFocused > 0 )
				{
					m_iFocused--;
					m_iSelected = m_iFocused;

					if( !m_bOpened )
						m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
				}

				return true;

			case VK_RIGHT:
			case VK_DOWN:
				if( m_iFocused+1 < (int)GetNumItems() )
				{
					m_iFocused++;
					m_iSelected = m_iFocused;

					if( !m_bOpened )
						m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
				}

				return true;
			}
			break;
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------
bool CMANGOComboBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
		return true;

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
			{
				// Determine which item has been selected
				for( int i=0; i < m_Items.GetSize(); i++ )
				{
					MANGOComboBoxItem* pItem = m_Items.GetAt( i );
					if( pItem -> bVisible &&
						PtInRect( &pItem->rcActive, pt ) )
					{
						m_iFocused = i;
					}
				}
				return true;
			}
			break;
		}

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

				// Toggle dropdown
				if( m_bHasFocus )
				{
					m_bOpened = !m_bOpened;

					if( !m_bOpened )
					{
						if( !m_pDialog->m_bKeyboardInput )
							m_pDialog->ClearFocus();
					}
				}

				return true;
			}

			// Perhaps this click is within the dropdown
			if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
			{
				// Determine which item has been selected
				for( int i=m_ScrollBar.GetTrackPos(); i < m_Items.GetSize(); i++ )
				{
					MANGOComboBoxItem* pItem = m_Items.GetAt( i );
					if( pItem -> bVisible &&
						PtInRect( &pItem->rcActive, pt ) )
					{
						m_iFocused = m_iSelected = i;
						m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
						m_bOpened = false;

						if( !m_pDialog->m_bKeyboardInput )
							m_pDialog->ClearFocus();

						break;
					}
				}

				return true;
			}

			// Mouse click not on main control or in dropdown, fire an event if needed
			if( m_bOpened )
			{
				m_iFocused = m_iSelected;

				m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
				m_bOpened = false;
			}

			// Make sure the control is no longer in a pressed state
			m_bPressed = false;

			// Release focus if appropriate
			if( !m_pDialog->m_bKeyboardInput )
			{
				m_pDialog->ClearFocus();
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			if( m_bPressed && ContainsPoint( pt ) )
			{
				// Button click
				m_bPressed = false;
				ReleaseCapture();
				return true;
			}

			break;
		}

	case WM_MOUSEWHEEL:
		{
			int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
			if( m_bOpened )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				m_ScrollBar.Scroll( -zDelta * uLines );
			} else
			{
				if( zDelta > 0 )
				{
					if( m_iFocused > 0 )
					{
						m_iFocused--;
						m_iSelected = m_iFocused;     

						if( !m_bOpened )
							m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
					}          
				}
				else
				{
					if( m_iFocused+1 < (int)GetNumItems() )
					{
						m_iFocused++;
						m_iSelected = m_iFocused;   

						if( !m_bOpened )
							m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
					}
				}
			}
			return true;
		}
	};

	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::OnHotkey()
{
	if( m_bOpened )
		return;

	if( m_iSelected == -1 )
		return;

	if( m_pDialog->IsKeyboardInputEnabled() )
		m_pDialog->RequestFocus( this );

	m_iSelected++;

	if( m_iSelected >= (int) m_Items.GetSize() )
		m_iSelected = 0;

	m_iFocused = m_iSelected;
	m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	MANGO_CONTROL_STATE iState = MANGO_STATE_NORMAL;

	if( !m_bOpened )
		iState = MANGO_STATE_HIDDEN;

	// Dropdown box
	CMANGOElement* pElement = m_Elements.GetAt( 2 );

	// If we have not initialized the scroll bar page size,
	// do that now.
	static bool bSBInit;
	if( !bSBInit )
	{
		// Update the page size of the scroll bar
		if( m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight )
			m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) / m_pDialog->GetManager()->GetFontNode( pElement->iFont )->nHeight );
		else
			m_ScrollBar.SetPageSize( RectHeight( m_rcDropdownText ) );
		bSBInit = true;
	}

	// Scroll bar
	if( m_bOpened )
		m_ScrollBar.Render( pd3dDevice, fElapsedTime );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime );
	pElement->FontColor.Blend( iState, fElapsedTime );

	m_pDialog->DrawSprite( pElement, &m_rcDropdown );

	// Selection outline
	CMANGOElement* pSelectionElement = m_Elements.GetAt( 3 );
	pSelectionElement->TextureColor.Current = pElement->TextureColor.Current;
	pSelectionElement->FontColor.Current = pSelectionElement->FontColor.States[ MANGO_STATE_NORMAL ];

	MANGOFontNode* pFont = m_pDialog->GetFont( pElement->iFont );
	if( pFont )
	{
		int curY = m_rcDropdownText.top;
		int nRemainingHeight = RectHeight( m_rcDropdownText );
		//WCHAR strDropdown[4096] = {0};

		for( int i = m_ScrollBar.GetTrackPos(); i < m_Items.GetSize(); i++ )
		{
			MANGOComboBoxItem* pItem = m_Items.GetAt( i );

			// Make sure there's room left in the dropdown
			nRemainingHeight -= pFont->nHeight;
			if( nRemainingHeight < 0 )
			{
				pItem->bVisible = false;
				continue;
			}

			SetRect( &pItem->rcActive, m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + pFont->nHeight );
			curY += pFont->nHeight;

			//debug
			//int blue = 50 * i;
			//m_pDialog->DrawRect( &pItem->rcActive, 0xFFFF0000 | blue );

			pItem->bVisible = true;

			if( m_bOpened )
			{
				if( (int)i == m_iFocused )
				{
					RECT rc;
					SetRect( &rc, m_rcDropdown.left, pItem->rcActive.top-2, m_rcDropdown.right, pItem->rcActive.bottom+2 );
					m_pDialog->DrawSprite( pSelectionElement, &rc );
					m_pDialog->DrawText( pItem->strText, pSelectionElement, &pItem->rcActive );
				}
				else
				{
					m_pDialog->DrawText( pItem->strText, pElement, &pItem->rcActive );
				}
			}
		}
	}

	int nOffsetX = 0;
	int nOffsetY = 0;

	iState = MANGO_STATE_NORMAL;

	if( m_bVisible == false )
		iState = MANGO_STATE_HIDDEN;
	else if( m_bEnabled == false )
		iState = MANGO_STATE_DISABLED;
	else if( m_bPressed )
	{
		iState = MANGO_STATE_PRESSED;
		//点下
		nOffsetX = 0;
		nOffsetY = 0;
	}
	else if( m_bMouseOver )
	{
		iState = MANGO_STATE_MOUSEOVER;
		//鼠标悬浮
		nOffsetX = 0;
		nOffsetY = 0;
	}
	else if( m_bHasFocus )
		iState = MANGO_STATE_FOCUS;

	float fBlendRate = ( iState == MANGO_STATE_PRESSED ) ? 0.0f : 0.8f;

	// Button
	pElement = m_Elements.GetAt( 1 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );

	RECT rcWindow = m_rcButton;
	OffsetRect( &rcWindow, nOffsetX, nOffsetY );
	m_pDialog->DrawSprite( pElement, &rcWindow );

	if( m_bOpened )
		iState = MANGO_STATE_PRESSED;

	// Main text box
	//TODO: remove magic numbers
	pElement = m_Elements.GetAt( 0 );

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

	m_pDialog->DrawSprite( pElement, &m_rcText);

	if( m_iSelected >= 0 && m_iSelected < (int) m_Items.GetSize() )
	{
		MANGOComboBoxItem* pItem = m_Items.GetAt( m_iSelected );
		if( pItem != NULL )
		{
			m_pDialog->DrawText( pItem->strText, pElement, &m_rcText );

		}
	}
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOComboBox::AddItem( const WCHAR* strText, void* pData )
{
	// Validate parameters
	if( strText== NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new item and set the data
	MANGOComboBoxItem* pItem = new MANGOComboBoxItem;
	if( pItem == NULL )
	{
		return DXTRACE_ERR_MSGBOX( L"new", E_OUTOFMEMORY );
	}

	ZeroMemory( pItem, sizeof(MANGOComboBoxItem) );
	StringCchCopy( pItem->strText, 256, strText );
	pItem->pData = pData;

	m_Items.Add( pItem );

	// Update the scroll bar with new range
	m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );

	// If this is the only item in the list, it's selected
	if( GetNumItems() == 1 )
	{
		m_iSelected = 0;
		m_iFocused = 0;
		m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::RemoveItem( UINT index )
{
	MANGOComboBoxItem* pItem = m_Items.GetAt( index );
	SAFE_DELETE( pItem );
	m_Items.Remove( index );
	m_ScrollBar.SetTrackRange( 0, m_Items.GetSize() );
	if( m_iSelected >= m_Items.GetSize() )
		m_iSelected = m_Items.GetSize() - 1;
}


//--------------------------------------------------------------------------------------
void CMANGOComboBox::RemoveAllItems()
{
	for( int i=0; i < m_Items.GetSize(); i++ )
	{
		MANGOComboBoxItem* pItem = m_Items.GetAt( i );
		SAFE_DELETE( pItem );
	}

	m_Items.RemoveAll();
	m_ScrollBar.SetTrackRange( 0, 1 );
	m_iFocused = m_iSelected = -1;
}



//--------------------------------------------------------------------------------------
bool CMANGOComboBox::ContainsItem( const WCHAR* strText, UINT iStart )
{
	return ( -1 != FindItem( strText, iStart ) );
}


//--------------------------------------------------------------------------------------
int CMANGOComboBox::FindItem( const WCHAR* strText, UINT iStart )
{
	if( strText == NULL )
		return -1;

	for( int i = iStart; i < m_Items.GetSize(); i++ )
	{
		MANGOComboBoxItem* pItem = m_Items.GetAt(i);

		if( 0 == wcscmp( pItem->strText, strText ) )
		{
			return i;
		}
	}

	return -1;
}


//--------------------------------------------------------------------------------------
void* CMANGOComboBox::GetSelectedData()
{
	if( m_iSelected < 0 )
		return NULL;

	MANGOComboBoxItem* pItem = m_Items.GetAt( m_iSelected );
	return pItem->pData;
}



//--------------------------------------------------------------------------------------
MANGOComboBoxItem* CMANGOComboBox::GetSelectedItem()
{
	if( m_iSelected < 0 )
		return NULL;

	return m_Items.GetAt( m_iSelected );
}


//--------------------------------------------------------------------------------------
void* CMANGOComboBox::GetItemData( const WCHAR* strText )
{
	int index = FindItem( strText );
	if( index == -1 )
	{
		return NULL;
	}

	MANGOComboBoxItem* pItem = m_Items.GetAt(index);
	if( pItem == NULL )
	{
		DXTRACE_ERR( L"CGrowableArray::GetAt", E_FAIL );
		return NULL;
	}

	return pItem->pData;
}


//--------------------------------------------------------------------------------------
void* CMANGOComboBox::GetItemData( int nIndex )
{
	if( nIndex < 0 || nIndex >= m_Items.GetSize() )
		return NULL;

	return m_Items.GetAt( nIndex )->pData;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOComboBox::SetSelectedByIndex( UINT index )
{
	if( index >= GetNumItems() )
		return E_INVALIDARG;

	m_iFocused = m_iSelected = index;
	m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOComboBox::SetSelectedByText( const WCHAR* strText )
{
	if( strText == NULL )
		return E_INVALIDARG;

	int index = FindItem( strText );
	if( index == -1 )
		return E_FAIL;

	m_iFocused = m_iSelected = index;
	m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );

	return S_OK;
}



//--------------------------------------------------------------------------------------
HRESULT CMANGOComboBox::SetSelectedByData( void* pData )
{
	for( int i=0; i < m_Items.GetSize(); i++ )
	{
		MANGOComboBoxItem* pItem = m_Items.GetAt(i);

		if( pItem->pData == pData )
		{
			m_iFocused = m_iSelected = i;
			m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, false, this );
			return S_OK;
		}
	}

	return E_FAIL;
}

