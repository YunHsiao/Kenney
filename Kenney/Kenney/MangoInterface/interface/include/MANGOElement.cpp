//--------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// 2010-3-4 12:30   Cheng()   by  mi            
//--------------------------------------------------------------------------------------
#pragma once
#include "..\Mango.h"
#include "MANGOElement.h "

//--------------------------------------------------------------------------------------
void CMANGOElement::SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor )
{
	this->iTexture = iTexture;

	if( prcTexture )
		rcTexture = *prcTexture;
	else
		SetRectEmpty( &rcTexture );

	TextureColor.Init( defaultTextureColor );
}


//--------------------------------------------------------------------------------------
void CMANGOElement::SetFont( UINT iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat )
{
	this->iFont = iFont;
	this->dwTextFormat = dwTextFormat;

	FontColor.Init( defaultFontColor );
}


//--------------------------------------------------------------------------------------
void CMANGOElement::Refresh()
{
	TextureColor.Current = TextureColor.States[ MANGO_STATE_HIDDEN ];
	FontColor.Current = FontColor.States[ MANGO_STATE_HIDDEN ];
}

//--------------------------------------------------------------------------------------
void MANGOBlendColor::Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor, D3DCOLOR hiddenColor)
{
	for( int i=0; i < MAX_CONTROL_STATES; i++ )
	{
		States[ i ] = defaultColor;
	}

	States[ MANGO_STATE_DISABLED ] = disabledColor;
	States[ MANGO_STATE_HIDDEN ] = hiddenColor;
	Current = hiddenColor;
}


//--------------------------------------------------------------------------------------
void MANGOBlendColor::Blend( UINT iState, float fElapsedTime, float fRate )
{
	D3DXCOLOR destColor = States[ iState ];
	D3DXColorLerp( &Current, &Current, &destColor, 1.0f - powf( fRate, 30 * fElapsedTime ) );
}
