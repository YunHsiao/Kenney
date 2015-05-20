
#ifndef MANGO_GUI_CHECKBOX_H
#define MANGO_GUI_CHECKBOX_H

#pragma once
//-----------------------------------------------------------------------------
// CheckBox control
//-----------------------------------------------------------------------------
class CMANGOCheckBox : public CMANGOButton
{
public:
	CMANGOCheckBox( CMANGOScene *pDialog = NULL );

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); SetCheckedInternal( !m_bChecked, true ); }

	virtual BOOL ContainsPoint( POINT pt ); 
	virtual void UpdateRects(); 

	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

	bool GetChecked() { return m_bChecked; }
	void SetChecked( bool bChecked ) { SetCheckedInternal( bChecked, false ); }

protected:
	virtual void SetCheckedInternal( bool bChecked, bool bFromInput );

	bool m_bChecked;
	RECT m_rcButton;
	RECT m_rcText;
};

#endif