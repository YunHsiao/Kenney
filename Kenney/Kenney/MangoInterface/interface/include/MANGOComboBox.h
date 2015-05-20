
#pragma once

//-----------------------------------------------------------------------------
// ComboBox control
//-----------------------------------------------------------------------------
struct MANGOComboBoxItem
{
	WCHAR strText[256];
	void*  pData;

	RECT  rcActive;
	bool  bVisible;
};


struct MANGOFontNode
{
	WCHAR strFace[MAX_PATH];
	ID3DXFont* pFont;
	LONG  nHeight;
	LONG  nWeight;
};

class CMANGOComboBox : public CMANGOButton
{
public:
	CMANGOComboBox( CMANGOScene *pDialog = NULL );
	virtual ~CMANGOComboBox();

	virtual void SetTextColor( D3DCOLOR Color );
	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey();

	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual void OnFocusOut();
	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

	virtual void UpdateRects(); 

	HRESULT AddItem( const WCHAR* strText, void* pData );
	void    RemoveAllItems();
	void    RemoveItem( UINT index );
	bool    ContainsItem( const WCHAR* strText, UINT iStart=0 );
	int     FindItem( const WCHAR* strText, UINT iStart=0 );
	void*   GetItemData( const WCHAR* strText );
	void*   GetItemData( int nIndex );
	void    SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; UpdateRects(); }
	int     GetScrollBarWidth() const { return m_nSBWidth; }
	void    SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }

	void*   GetSelectedData();
	MANGOComboBoxItem* GetSelectedItem();

	UINT    GetNumItems() { return m_Items.GetSize(); }
	MANGOComboBoxItem* GetItem( UINT index ) { return m_Items.GetAt( index ); }

	HRESULT SetSelectedByIndex( UINT index );
	HRESULT SetSelectedByText( const WCHAR* strText );
	HRESULT SetSelectedByData( void* pData );  

protected:
	int     m_iSelected;
	int     m_iFocused;
	int     m_nDropHeight;
	CMANGOScrollBar m_ScrollBar;
	int     m_nSBWidth;

	bool    m_bOpened;

	RECT m_rcText;
	RECT m_rcButton;
	RECT m_rcDropdown;
	RECT m_rcDropdownText;


	CMangoArray< MANGOComboBoxItem* > m_Items;
};

