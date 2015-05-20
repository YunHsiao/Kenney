
#ifndef MANGO_GUI_IMAGE_H
#define MANGO_GUI_IMAGE_H

//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
#pragma once

class CMANGOImage : public CMANGOControl
{
public:
	CMANGOImage( CMANGOScene *pDialog = NULL );
	void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	void SetTexture(RECT re){m_rectTexture = re;}

	virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
private:
	RECT m_rectTexture;
	bool m_bPressed;

};
#endif	//MANGO_GUI_IMAGE_H