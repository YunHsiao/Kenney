
#ifndef MANGO_GUI_LABLE_H
#define MANGO_GUI_LABLE_H

//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
#pragma once

class CMANGOLable : public CMANGOControl
{
public:
	CMANGOLable( CMANGOScene *pDialog = NULL );

	virtual void Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime );
	//virtual BOOL ContainsPoint( POINT pt ) { return false; }

	HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
	LPCWSTR GetText() { return m_strText; }
	HRESULT SetText( LPCWSTR strText );
	void SetTexture(RECT re){m_rectTexture = re;}

protected:
	WCHAR m_strText[MAX_PATH];      // Window text  
	RECT m_rectTexture;
};

#endif