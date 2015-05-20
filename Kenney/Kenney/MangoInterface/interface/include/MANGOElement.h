//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------

#ifndef MANGOELEMENT_H
#define MANGOELEMENT_H
#pragma once


enum MANGO_CONTROL_STATE
{
	MANGO_STATE_NORMAL = 0,
	MANGO_STATE_DISABLED,
	MANGO_STATE_HIDDEN,
	MANGO_STATE_FOCUS,
	MANGO_STATE_MOUSEOVER,
	MANGO_STATE_PRESSED,
};

#define MAX_CONTROL_STATES 6

struct MANGOBlendColor
{
	void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0 );
	void Blend( UINT iState, float fElapsedTime, float fRate = 0.7f );

	D3DCOLOR  States[ MAX_CONTROL_STATES ]; // Modulate colors for all possible control states
	D3DXCOLOR Current;
};

//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------

class CMANGOElement
{
public:
	void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255) );
	void SetFont( UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );

	void Refresh();

	UINT iTexture;          // Index of the texture for this Element 
	UINT iFont;             // Index of the font for this Element
	DWORD dwTextFormat;     // The format argument to DrawText 

	RECT rcTexture;         // Bounding rect of this element on the composite texture

	MANGOBlendColor TextureColor;
	MANGOBlendColor FontColor;
};

#endif