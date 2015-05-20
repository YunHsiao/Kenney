
#ifndef MANGO_GUI_CONTROL_H
#define MANGO_GUI_CONTROL_H
#pragma once

//-----------------------------------------------------------------------------
// Base class for controls
//-----------------------------------------------------------------------------
class CMANGOControl
{
public:
	CMANGOControl( CMANGOScene *pDialog = NULL );
	virtual ~CMANGOControl();

	virtual HRESULT OnInit() { return S_OK; }
	virtual void Refresh();
	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime ) { };

	// Windows message handler
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return false; }

	virtual bool CanHaveFocus() { return false; }
	virtual void OnFocusIn() { m_bHasFocus = true; }
	virtual void OnFocusOut() { m_bHasFocus = false; }
	virtual void OnMouseEnter() { m_bMouseOver = true; }
	virtual void OnMouseLeave() { m_bMouseOver = false; }
	virtual void OnHotkey() {}

	virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }

	virtual void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
	virtual bool GetEnabled() { return m_bEnabled; }
	virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	virtual bool GetVisible() { return m_bVisible; }

	UINT GetType() const { return m_Type; }

	int  GetID() const { return m_ID; }
	void SetID( int ID ) { m_ID = ID; }

	void SetLocation( int x, int y ,float z=1.0f) { m_x = x; m_y = y ,m_z = z; UpdateRects(); }
	void SetSize( int width, int height ) { m_width = width; m_height = height; UpdateRects(); }

	void SetHotkey( UINT nHotkey ) { m_nHotkey = nHotkey; }
	UINT GetHotkey() { return m_nHotkey; }

	void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
	void *GetUserData() const { return m_pUserData; }

	virtual void SetTextColor( D3DCOLOR Color );
	CMANGOElement* GetElement( UINT iElement ) { return m_Elements.GetAt( iElement ); }
	HRESULT SetElement( UINT iElement, CMANGOElement* pElement);
	bool	IsMouseOver(){return m_bMouseOver;}
	bool	IsDefault(){return m_bIsDefault;}
	bool	IsFocus(){return m_bHasFocus;}

	bool m_bVisible;                // Shown/hidden flag
	bool m_bMouseOver;              // Mouse pointer is above control
	bool m_bHasFocus;               // Control has input focus
	bool m_bIsDefault;              // Is the default control

	// Size, scale, and positioning members
	int m_x, m_y ;
	float m_z;
	int m_width, m_height;

	// These members are set by the container
	CMANGOScene* m_pDialog;    // Parent container
	UINT m_Index;              // Index within the control list

	CMangoArray< CMANGOElement* > m_Elements;  // All display elements

protected:
	virtual void UpdateRects();

	int  m_ID;                 // ID number
	MANGO_CONTROL_TYPE m_Type;  // Control type, set once in constructor  
	UINT m_nHotkey;            // Virtual key code for this control's hotkey
	void *m_pUserData;         // Data associated with this control that is set by user.

	bool m_bEnabled;           // Enabled/disabled flag

	RECT m_rcBoundingBox;      // Rectangle defining the active region of the control
};

#endif
