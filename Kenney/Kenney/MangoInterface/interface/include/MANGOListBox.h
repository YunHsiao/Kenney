#ifndef MANGO_GUI_LISTBOX_H
#define MANGO_GUI_LISTBOX_H

#pragma once

//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------
struct MANGOListBoxItem
{
	WCHAR strText[256];
	void*  pData;

	RECT  rcActive;
	bool  bSelected;
};

class CMANGOListBox : public CMANGOControl
{
public:
	CMANGOListBox( CMANGOScene *pDialog = NULL );
	virtual ~CMANGOListBox();

	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
	virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual bool    MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void    Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	virtual void    UpdateRects();

	DWORD GetStyle() const { return m_dwStyle; }
	int GetSize() const { return m_Items.GetSize(); }
	void SetStyle( DWORD dwStyle ) { m_dwStyle = dwStyle; }
	int  GetScrollBarWidth() const { return m_nSBWidth; }
	void SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }
	void SetBorder( int nBorder, int nMargin ) { m_nBorder = nBorder; m_nMargin = nMargin; }
	HRESULT AddItem( const WCHAR *wszText, void *pData );
	HRESULT InsertItem( int nIndex, const WCHAR *wszText, void *pData );
	void RemoveItem( int nIndex );
	void RemoveItemByText( WCHAR *wszText );
	void RemoveItemByData( void *pData );
	void RemoveAllItems();

	MANGOListBoxItem *GetItem( int nIndex );
	int GetSelectedIndex( int nPreviousSelected = -1 );
	MANGOListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
	void SelectItem( int nNewIndex );

	enum STYLE { MULTISELECTION = 1 };

protected:
	RECT m_rcText;      // Text rendering bound
	RECT m_rcSelection; // Selection box bound
	CMANGOScrollBar m_ScrollBar;
	int m_nSBWidth;
	int m_nBorder;
	int m_nMargin;
	int m_nTextHeight;  // Height of a single line of text
	DWORD m_dwStyle;    // List box style
	int m_nSelected;    // Index of the selected item for single selection list box
	int m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
	bool m_bDrag;       // Whether the user is dragging the mouse to select

	CMangoArray< MANGOListBoxItem* > m_Items;
};

#endif