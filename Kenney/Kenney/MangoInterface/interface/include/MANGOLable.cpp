
#include "..\Mango.h"
#include ".\mangolable.h"

//--------------------------------------------------------------------------------------
// CMANGOLable class
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
CMANGOLable::CMANGOLable( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_LABLE;
	m_pDialog = pDialog;

	ZeroMemory( &m_strText, sizeof(m_strText) );  

	for( int i=0; i < m_Elements.GetSize(); i++ )
	{
		CMANGOElement* pElement = m_Elements.GetAt( i );
		SAFE_DELETE( pElement );
	}

	m_Elements.RemoveAll();
}


//--------------------------------------------------------------------------------------
void CMANGOLable::Render( IDirect3DDevice9* pd3dDevice, float fElapsedTime )
{    
	if( m_bVisible == false )
		return;

	MANGO_CONTROL_STATE iState = MANGO_STATE_NORMAL;

	if( m_bEnabled == false )
		iState = MANGO_STATE_DISABLED;

	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	pElement->FontColor.Blend( iState, fElapsedTime );

	m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox, true );
}

//--------------------------------------------------------------------------------------
HRESULT CMANGOLable::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
	// Validate incoming parameters
	if( strDest == NULL || bufferCount == 0 )
	{
		return E_INVALIDARG;
	}

	// Copy the window text
	StringCchCopy( strDest, bufferCount, m_strText );

	return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOLable::SetText( LPCWSTR strText )
{
	if( strText == NULL )
	{
		m_strText[0] = 0;
		return S_OK;
	}

	StringCchCopy( m_strText, MAX_PATH, strText); 
	return S_OK;
}
