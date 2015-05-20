#include "../Mango.h"
#include ".\mangoeditbox.h"


//--------------------------------------------------------------------------------------
// CMANGOEditBox class
//--------------------------------------------------------------------------------------

// When scrolling, EDITBOX_SCROLLEXTENT is reciprocal of the amount to scroll.
// If EDITBOX_SCROLLEXTENT = 4, then we scroll 1/4 of the control each time.
#define EDITBOX_SCROLLEXTENT 4

//--------------------------------------------------------------------------------------
CMANGOEditBox::CMANGOEditBox( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_EDITBOX;
	m_pDialog = pDialog;

	m_nBorder = 5;  // Default border width
	m_nSpacing = 4;  // Default spacing

	m_bCaretOn = true;
	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = MANGOGetGlobalTimer()->GetAbsoluteTime();
	s_bHideCaret = false;
	m_nFirstVisible = 0;
	m_TextColor = D3DCOLOR_ARGB( 255, 16, 16, 16 );
	m_SelTextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelBkColor = D3DCOLOR_ARGB( 255, 40, 50, 92 );
	m_CaretColor = D3DCOLOR_ARGB( 255, 0, 0, 0 );
	m_nCaret = m_nSelStart = 0;
	m_bInsertMode = true;
	m_bPassword = false;

	m_bMouseDrag = false;
}


//--------------------------------------------------------------------------------------
CMANGOEditBox::~CMANGOEditBox()
{
}


//--------------------------------------------------------------------------------------
// PlaceCaret: Set the caret to a character position, and adjust the scrolling if
//             necessary.
//--------------------------------------------------------------------------------------
void CMANGOEditBox::PlaceCaret( int nCP )
{
	assert( nCP >= 0 && nCP <= m_Buffer.GetTextSize() );
	m_nCaret = nCP;

	// Obtain the X offset of the character.
	int nX1st, nX, nX2;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // 1st visible char
	m_Buffer.CPtoX( nCP, FALSE, &nX );  // LEAD
	// If nCP is the NULL terminator, get the leading edge instead of trailing.
	if( nCP == m_Buffer.GetTextSize() )
		nX2 = nX;
	else
		m_Buffer.CPtoX( nCP, TRUE, &nX2 );  // TRAIL

	// If the left edge of the char is smaller than the left edge of the 1st visible char,
	// we need to scroll left until this char is visible.
	if( nX < nX1st )
	{
		// Simply make the first visible character the char at the new caret position.
		m_nFirstVisible = nCP;
	}
	else
		// If the right of the character is bigger than the offset of the control's
		// right edge, we need to scroll right to this character.
		if( nX2 > nX1st + RectWidth( m_rcText ) )
		{
			// Compute the X of the new left-most pixel
			int nXNewLeft = nX2 - RectWidth( m_rcText );

			// Compute the char position of this character
			int nCPNew1st, nNewTrail;
			m_Buffer.XtoCP( nXNewLeft, &nCPNew1st, &nNewTrail );

			// If this coordinate is not on a character border,
			// start from the next character so that the caret
			// position does not fall outside the text rectangle.
			int nXNew1st;
			m_Buffer.CPtoX( nCPNew1st, FALSE, &nXNew1st );
			if( nXNew1st < nXNewLeft )
				++nCPNew1st;

			m_nFirstVisible = nCPNew1st;
		}
}


//--------------------------------------------------------------------------------------
void CMANGOEditBox::ClearText()
{
	m_Buffer.Clear();
	m_nFirstVisible = 0;
	PlaceCaret( 0 );
	m_nSelStart = 0;
}


//--------------------------------------------------------------------------------------
void CMANGOEditBox::SetText( LPCWSTR wszText, bool bSelected )
{
	assert( wszText != NULL );

	m_Buffer.SetText( wszText );
	m_nFirstVisible = 0;
	// Move the caret to the end of the text
	PlaceCaret( m_Buffer.GetTextSize() );
	m_nSelStart = bSelected ? 0 : m_nCaret;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOEditBox::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
	assert( strDest );

	StringCchCopy( strDest, bufferCount, m_Buffer.GetBuffer() );

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CMANGOEditBox::DeleteSelectionText()
{
	int nFirst = __min( m_nCaret, m_nSelStart );
	int nLast = __max( m_nCaret, m_nSelStart );
	// Update caret and selection
	PlaceCaret( nFirst );
	m_nSelStart = m_nCaret;
	// Remove the characters
	for( int i = nFirst; i < nLast; ++i )
		m_Buffer.RemoveChar( nFirst );
}


//--------------------------------------------------------------------------------------
void CMANGOEditBox::UpdateRects()
{
	CMANGOControl::UpdateRects();

	// Update the text rectangle
	m_rcText = m_rcBoundingBox;
	// First inflate by m_nBorder to compute render rects
	InflateRect( &m_rcText, -m_nBorder, -m_nBorder );

	// Update the render rectangles
	m_rcRender[0] = m_rcText;
	SetRect( &m_rcRender[1], m_rcBoundingBox.left, m_rcBoundingBox.top, m_rcText.left, m_rcText.top );
	SetRect( &m_rcRender[2], m_rcText.left, m_rcBoundingBox.top, m_rcText.right, m_rcText.top );
	SetRect( &m_rcRender[3], m_rcText.right, m_rcBoundingBox.top, m_rcBoundingBox.right, m_rcText.top );
	SetRect( &m_rcRender[4], m_rcBoundingBox.left, m_rcText.top, m_rcText.left, m_rcText.bottom );
	SetRect( &m_rcRender[5], m_rcText.right, m_rcText.top, m_rcBoundingBox.right, m_rcText.bottom );
	SetRect( &m_rcRender[6], m_rcBoundingBox.left, m_rcText.bottom, m_rcText.left, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[7], m_rcText.left, m_rcText.bottom, m_rcText.right, m_rcBoundingBox.bottom );
	SetRect( &m_rcRender[8], m_rcText.right, m_rcText.bottom, m_rcBoundingBox.right, m_rcBoundingBox.bottom );

	// Inflate further by m_nSpacing
	InflateRect( &m_rcText, -m_nSpacing, -m_nSpacing );
}


void CMANGOEditBox::CopyToClipboard()
{
	// Copy the selection text to the clipboard
	if( m_nCaret != m_nSelStart && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(WCHAR) * ( m_Buffer.GetTextSize() + 1 ) );
		if( hBlock )
		{
			WCHAR *pwszText = (WCHAR*)GlobalLock( hBlock );
			if( pwszText )
			{
				int nFirst = __min( m_nCaret, m_nSelStart );
				int nLast = __max( m_nCaret, m_nSelStart );
				if( nLast - nFirst > 0 )
					CopyMemory( pwszText, m_Buffer.GetBuffer() + nFirst, (nLast - nFirst) * sizeof(WCHAR) );
				pwszText[nLast - nFirst] = L'\0';  // Terminate it
				GlobalUnlock( hBlock );
			}
			SetClipboardData( CF_UNICODETEXT, hBlock );
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}


void CMANGOEditBox::PasteFromClipboard()
{
	DeleteSelectionText();

	if( OpenClipboard( NULL ) )
	{
		HANDLE handle = GetClipboardData( CF_UNICODETEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR *pwszText = (WCHAR*)GlobalLock( handle );
			if( pwszText )
			{
				// Copy all characters up to null.
				if( m_Buffer.InsertString( m_nCaret, pwszText ) )
					PlaceCaret( m_nCaret + lstrlenW( pwszText ) );
				m_nSelStart = m_nCaret;
				GlobalUnlock( handle );
			}
		}
		CloseClipboard();
	}
}


//--------------------------------------------------------------------------------------
bool CMANGOEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	bool bHandled = false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_TAB:
				// We don't process Tab in case keyboard input is enabled and the user
				// wishes to Tab to other controls.
				break;

			case VK_HOME:
				PlaceCaret( 0 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_END:
				PlaceCaret( m_Buffer.GetTextSize() );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_INSERT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control Insert. Copy to clipboard
					CopyToClipboard();
				} else
					if( GetKeyState( VK_SHIFT ) < 0 )
					{
						// Shift Insert. Paste from clipboard
						PasteFromClipboard();
					} else
					{
						// Toggle caret insert mode
						m_bInsertMode = !m_bInsertMode;
					}
					break;

			case VK_DELETE:
				// Check if there is a text selection.
				if( m_nCaret != m_nSelStart )
				{
					DeleteSelectionText();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
				else
				{
					// Deleting one character
					if( m_Buffer.RemoveChar( m_nCaret ) )
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_LEFT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else
					if( m_nCaret > 0 )
						PlaceCaret( m_nCaret - 1 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_RIGHT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else
					if( m_nCaret < m_Buffer.GetTextSize() )
						PlaceCaret( m_nCaret + 1 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_UP:
			case VK_DOWN:
				// Trap up and down arrows so that the dialog
				// does not switch focus to another control.
				bHandled = true;
				break;

			default:
				bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
			}
		}
	}
	return bHandled;
}


//--------------------------------------------------------------------------------------
bool CMANGOEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( !m_bHasFocus )
				m_pDialog->RequestFocus( this );

			if( !ContainsPoint( pt ) )
				return false;

			m_bMouseDrag = true;
			SetCapture( MANGOGetHWND() );
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if( nTrail && nCP < m_Buffer.GetTextSize() )
					PlaceCaret( nCP + 1 );
				else
					PlaceCaret( nCP );
				m_nSelStart = m_nCaret;
				ResetCaretBlink();
			}
			return true;
		}

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_bMouseDrag = false;
		break;

	case WM_MOUSEMOVE:
		if( m_bMouseDrag )
		{
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if( nTrail && nCP < m_Buffer.GetTextSize() )
					PlaceCaret( nCP + 1 );
				else
					PlaceCaret( nCP );
			}
		}
		break;
	}

	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOEditBox::OnFocusIn()
{
	CMANGOControl::OnFocusIn();

	ResetCaretBlink();
}


//--------------------------------------------------------------------------------------
bool CMANGOEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
		// Make sure that while editing, the keyup and keydown messages associated with 
		// WM_CHAR messages don't go to any non-focused controls or cameras
	case WM_KEYUP:
	case WM_KEYDOWN:
		return true;

	case WM_CHAR:
		{
			switch( (WCHAR)wParam )
			{
				// Backspace
			case VK_BACK:
				{
					// If there's a selection, treat this
					// like a delete key.
					if( m_nCaret != m_nSelStart )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}
					else
						if( m_nCaret > 0 )
						{
							// Move the caret, then delete the char.
							PlaceCaret( m_nCaret - 1 );
							m_nSelStart = m_nCaret;
							m_Buffer.RemoveChar( m_nCaret );
							m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
						}
						ResetCaretBlink();
						break;
				}

			case 24:        // Ctrl-X Cut
			case VK_CANCEL: // Ctrl-C Copy
				{
					CopyToClipboard();

					// If the key is Ctrl-X, delete the selection too.
					if( (WCHAR)wParam == 24 )
					{
						DeleteSelectionText();
						m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					}

					break;
				}

				// Ctrl-V Paste
			case 22:
				{
					PasteFromClipboard();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
					break;
				}

				// Ctrl-A Select All
			case 1:
				if( m_nSelStart == m_nCaret )
				{
					m_nSelStart = 0;
					PlaceCaret( m_Buffer.GetTextSize() );
				}
				break;

			case VK_RETURN:
				// Invoke the callback when the user presses Enter.
				m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
				break;

				// Junk characters we don't want in the string
			case 26:  // Ctrl Z
			case 2:   // Ctrl B
			case 14:  // Ctrl N
			case 19:  // Ctrl S
			case 4:   // Ctrl D
			case 6:   // Ctrl F
			case 7:   // Ctrl G
			case 10:  // Ctrl J
			case 11:  // Ctrl K
			case 12:  // Ctrl L
			case 17:  // Ctrl Q
			case 23:  // Ctrl W
			case 5:   // Ctrl E
			case 18:  // Ctrl R
			case 20:  // Ctrl T
			case 25:  // Ctrl Y
			case 21:  // Ctrl U
			case 9:   // Ctrl I
			case 15:  // Ctrl O
			case 16:  // Ctrl P
			case 27:  // Ctrl [
			case 29:  // Ctrl ]
			case 28:  // Ctrl \ 
				break;

			default:
				{
					// If there's a selection and the user
					// starts to type, the selection should
					// be deleted.
					if( m_nCaret != m_nSelStart )
						DeleteSelectionText();

					// If we are in overwrite mode and there is already
					// a char at the caret's position, simply replace it.
					// Otherwise, we insert the char as normal.
					if( !m_bInsertMode && m_nCaret < m_Buffer.GetTextSize() )
					{
						m_Buffer[m_nCaret] = (WCHAR)wParam;
						PlaceCaret( m_nCaret + 1 );
						m_nSelStart = m_nCaret;
					} else
					{
						// Insert the char
						if( m_Buffer.InsertChar( m_nCaret, (WCHAR)wParam ) )
						{
							PlaceCaret( m_nCaret + 1 );
							m_nSelStart = m_nCaret;
						}
					}
					ResetCaretBlink();
					m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
				}
			}
			return true;
		}
	}
	return false;
}


//--------------------------------------------------------------------------------------
void CMANGOEditBox::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{
	if( m_bVisible == false )
		return;

	HRESULT hr;
	int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

	CMANGOElement* pElement = GetElement( 0 );
	if( pElement )
	{
		m_Buffer.SetFontNode( m_pDialog->GetFont( pElement->iFont ) );
		PlaceCaret( m_nCaret );  // Call PlaceCaret now that we have the font info (node),
		// so that scrolling can be handled.
	}

	// Render the control graphics
	for( int e = 0; e < 9; ++e )
	{
		pElement = m_Elements.GetAt( e );
		pElement->TextureColor.Blend( MANGO_STATE_NORMAL, fElapsedTime );

		m_pDialog->DrawSprite( pElement, &m_rcRender[e] );
	}

	//
	// Compute the X coordinates of the first visible character.
	//
	int nXFirst;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

	//
	// Compute the X coordinates of the selection rectangle
	//
	hr = m_Buffer.CPtoX( m_nCaret, FALSE, &nCaretX );
	if( m_nCaret != m_nSelStart )
		hr = m_Buffer.CPtoX( m_nSelStart, FALSE, &nSelStartX );
	else
		nSelStartX = nCaretX;

	//
	// Render the selection rectangle
	//
	RECT rcSelection;  // Make this available for rendering selected text
	if( m_nCaret != m_nSelStart )
	{
		int nSelLeftX = nCaretX, nSelRightX = nSelStartX;
		// Swap if left is bigger than right
		if( nSelLeftX > nSelRightX )
		{ int nTemp = nSelLeftX; nSelLeftX = nSelRightX; nSelRightX = nTemp; }

		SetRect( &rcSelection, nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
		OffsetRect( &rcSelection, m_rcText.left - nXFirst, 0 );
		IntersectRect( &rcSelection, &m_rcText, &rcSelection );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pDialog->DrawRect( &rcSelection, m_SelBkColor );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	}

	//
	// Render the text
	//
	// Element 0 for text
	
	m_Elements.GetAt( 0 )->FontColor.Current = m_TextColor;

	if(m_bPassword)
	{
		WCHAR temp[256];
		int i =0;
		for(;i<m_Buffer.GetTextSize()&& i<256;i++)
			temp[i] = '*';
		temp[i]=0;
		m_pDialog->DrawText( temp + m_nFirstVisible, m_Elements.GetAt( 0 ), &m_rcText );
	}else
	{
		m_pDialog->DrawText( m_Buffer.GetBuffer() + m_nFirstVisible, m_Elements.GetAt( 0 ), &m_rcText );
	}
	// Render the selected text
	if( m_nCaret != m_nSelStart )
	{
		int nFirstToRender = __max( m_nFirstVisible, __min( m_nSelStart, m_nCaret ) );
		int nNumChatToRender = __max( m_nSelStart, m_nCaret ) - nFirstToRender;
		m_Elements.GetAt( 0 )->FontColor.Current = m_SelTextColor;

		if(m_bPassword)
		{
			WCHAR temp[256];
			int i =0;
			for(;i<m_Buffer.GetTextSize()&& i<256;i++)
				temp[i] = '*';
			temp[i]=0;
			m_pDialog->DrawText( temp + nFirstToRender,
				m_Elements.GetAt( 0 ), &rcSelection, false, nNumChatToRender );
		}else
		{	
			m_pDialog->DrawText( m_Buffer.GetBuffer() + nFirstToRender,
			m_Elements.GetAt( 0 ), &rcSelection, false, nNumChatToRender );
		}
	
	}

	//
	// Blink the caret
	//
	if( MANGOGetGlobalTimer()->GetAbsoluteTime() - m_dfLastBlink >= m_dfBlink )
	{
		m_bCaretOn = !m_bCaretOn;
		m_dfLastBlink = MANGOGetGlobalTimer()->GetAbsoluteTime();
	}

	//
	// Render the caret if this control has the focus
	//
	if( m_bHasFocus && m_bCaretOn && !s_bHideCaret )
	{
		// Start the rectangle with insert mode caret
		RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
			m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if( !m_bInsertMode )
		{
			// Obtain the right edge X coord of the current character
			int nRightEdgeX;
			m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
			rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
		}

		m_pDialog->DrawRect( &rcCaret, m_CaretColor );
	}
}


#define IN_FLOAT_CHARSET( c ) \
	( (c) == L'-' || (c) == L'.' || ( (c) >= L'0' && (c) <= L'9' ) )

void CMANGOEditBox::ParseFloatArray( float *pNumbers, int nCount )
{
	int nWritten = 0;  // Number of floats written
	const WCHAR *pToken, *pEnd;
	WCHAR wszToken[60];

	pToken = m_Buffer.GetBuffer();
	while( nWritten < nCount && *pToken != L'\0' )
	{
		// Skip leading spaces
		while( *pToken == L' ' )
			++pToken;

		if( *pToken == L'\0' )
			break;

		// Locate the end of number
		pEnd = pToken;
		while( IN_FLOAT_CHARSET( *pEnd ) )
			++pEnd;

		// Copy the token to our buffer
		int nTokenLen = __min( sizeof(wszToken) / sizeof(wszToken[0]) - 1, int(pEnd - pToken) );
		StringCchCopy( wszToken, nTokenLen, pToken );
		*pNumbers = (float)wcstod( wszToken, NULL );
		++nWritten;
		++pNumbers;
		pToken = pEnd;
	}
}


void CMANGOEditBox::SetTextFloatArray( const float *pNumbers, int nCount )
{
	WCHAR wszBuffer[512] = {0};
	WCHAR wszTmp[64];

	if( pNumbers == NULL )
		return;

	for( int i = 0; i < nCount; ++i )
	{
		StringCchPrintf( wszTmp, 64, L"%.4f ", pNumbers[i] );
		StringCchCat( wszBuffer, 512, wszTmp );
	}

	// Don't want the last space
	if( nCount > 0 && wcslen(wszBuffer) > 0 )
		wszBuffer[wcslen(wszBuffer)-1] = 0;

	SetText( wszBuffer );
}

