#ifndef MANGO_GUI_SLIDER_H
#define MANGO_GUI_SLIDER_H

//-----------------------------------------------------------------------------
// Slider control
//-----------------------------------------------------------------------------

#pragma once

class CMANGOSlider : public CMANGOControl
{
public:
	CMANGOSlider( CMANGOScene *pDialog = NULL );

	virtual BOOL ContainsPoint( POINT pt ); 
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void UpdateRects(); 

	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );

	void SetValue( int nValue ) { SetValueInternal( nValue, false ); }
	int  GetValue() const { return m_nValue; };

	void GetRange( int &nMin, int &nMax ) const { nMin = m_nMin; nMax = m_nMax; }
	void SetRange( int nMin, int nMax );

protected:
	void SetValueInternal( int nValue, bool bFromInput );
	int  ValueFromPos( int x ); 

	int m_nValue;

	int m_nMin;
	int m_nMax;

	int m_nDragX;      // Mouse position at start of drag
	int m_nDragOffset; // Drag offset from the center of the button
	int m_nButtonX;

	bool m_bPressed;
	RECT m_rcButton;
};

#endif

