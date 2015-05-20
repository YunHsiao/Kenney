
#ifndef MANGO_GUI_BUTTON_H
#define MANGO_GUI_BUTTON_H

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
#pragma once
#include "../core/MangoGUI.h"

class CMANGOButton : public CMANGOLable
{
public:
	CMANGOButton( CMANGOScene *pDialog = NULL );

	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this ); }

	virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

protected:
	bool m_bPressed;
};

#endif