#ifndef MANGO_GUI_EDITBOX_H
#define MANGO_GUI_EDITBOX_H

#pragma once


//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------
class CMANGOEditBox : public CMANGOControl
{
public:
	CMANGOEditBox( CMANGOScene *pDialog = NULL );
	virtual ~CMANGOEditBox();

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual void OnFocusIn();

	void SetText( LPCWSTR wszText, bool bSelected = false );
	LPCWSTR GetText() { return m_Buffer.GetBuffer(); }
	int GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding NULL.
	HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
	void ClearText();
	virtual void SetTextColor( D3DCOLOR Color ) { m_TextColor = Color; }  // Text color
	void SetSelectedTextColor( D3DCOLOR Color ) { m_SelTextColor = Color; }  // Selected text color
	void SetSelectedBackColor( D3DCOLOR Color ) { m_SelBkColor = Color; }  // Selected background color
	void SetCaretColor( D3DCOLOR Color ) { m_CaretColor = Color; }  // Caret color
	void SetBorderWidth( int nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // Border of the window
	void SetSpacing( int nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
	void ParseFloatArray( float *pNumbers, int nCount );
	void SetTextFloatArray( const float *pNumbers, int nCount );
	void SetPassWord(bool bPassword = true) {m_bPassword = bPassword;}
	bool IsPassWord(){return m_bPassword;}

protected:
	void PlaceCaret( int nCP );
	void DeleteSelectionText();
	void ResetCaretBlink();
	void CopyToClipboard();
	void PasteFromClipboard();

	CUniBuffer m_Buffer;     // Buffer to hold text
	int      m_nBorder;      // Border of the window
	int      m_nSpacing;     // Spacing between the text and the edge of border
	RECT     m_rcText;       // Bounding rectangle for the text
	RECT     m_rcRender[9];  // Convenient rectangles for rendering elements
	double   m_dfBlink;      // Caret blink time in milliseconds
	double   m_dfLastBlink;  // Last timestamp of caret blink
	bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
	int      m_nCaret;       // Caret position, in characters
	bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
	int      m_nSelStart;    // Starting position of the selection. The caret marks the end.
	int      m_nFirstVisible;// First visible character in the edit control
	D3DCOLOR m_TextColor;    // Text color
	D3DCOLOR m_SelTextColor; // Selected text color
	D3DCOLOR m_SelBkColor;   // Selected background color
	D3DCOLOR m_CaretColor;   // Caret color

	// Mouse-specific
	bool m_bMouseDrag;       // True to indicate drag in progress

	//PassWord EditBox
	bool m_bPassword;
	// Static
	static bool s_bHideCaret;   // If true, we don't render the caret.
};

#endif