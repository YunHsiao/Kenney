#ifndef MANGO_GUI_RADIOBUTTON_H
#define MANGO_GUI_RADIOBUTTON_H

#pragma once

//-----------------------------------------------------------------------------
// RadioButton control
//-----------------------------------------------------------------------------
class CMANGORadioButton : public CMANGOCheckBox
{
public:
	CMANGORadioButton( CMANGOScene *pDialog = NULL );

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); SetCheckedInternal( true, true, true ); }

	void SetChecked( bool bChecked, bool bClearGroup=true ) { SetCheckedInternal( bChecked, bClearGroup, false ); }
	void SetButtonGroup( UINT nButtonGroup ) { m_nButtonGroup = nButtonGroup; }
	UINT GetButtonGroup() { return m_nButtonGroup; }

protected:
	virtual void SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput );
	UINT m_nButtonGroup;
};

#endif
