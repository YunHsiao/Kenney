
#include "../Mango.h"
#include ".\mangocontrol.h"


//--------------------------------------------------------------------------------------
// CMANGOControl class
//--------------------------------------------------------------------------------------
CMANGOControl::CMANGOControl( CMANGOScene *pDialog )
{
	m_Type = MANGO_CONTROL_BUTTON;
	m_pDialog = pDialog;
	m_ID = 0;
	m_Index = 0;
	m_pUserData = NULL;

	m_bEnabled = true;
	m_bVisible = true;
	m_bMouseOver = false;
	m_bHasFocus = false;
	m_bIsDefault = false;

	m_pDialog = NULL;

	m_x = 0;
	m_y = 0;
	m_z = 1;
	m_width = 0;
	m_height = 0;

	ZeroMemory( &m_rcBoundingBox, sizeof( m_rcBoundingBox ) );
}


CMANGOControl::~CMANGOControl()
{
	for( int i = 0; i < m_Elements.GetSize(); ++i )
	{
		delete m_Elements[i];
	}
	m_Elements.RemoveAll();
}


//--------------------------------------------------------------------------------------
void CMANGOControl::SetTextColor( D3DCOLOR Color )
{
	CMANGOElement* pElement = m_Elements.GetAt( 0 );

	if( pElement )
		pElement->FontColor.States[MANGO_STATE_NORMAL] = Color;
}


//--------------------------------------------------------------------------------------
HRESULT CMANGOControl::SetElement( UINT iElement, CMANGOElement* pElement )
{
	HRESULT hr = S_OK;

	if( pElement == NULL )
		return E_INVALIDARG;

	// Make certain the array is this large
	for( UINT i=m_Elements.GetSize(); i <= iElement; i++ )
	{
		CMANGOElement* pNewElement = new CMANGOElement();
		if( pNewElement == NULL )
			return E_OUTOFMEMORY;

		hr = m_Elements.Add( pNewElement );
		if( FAILED(hr) )
		{
			SAFE_DELETE( pNewElement );
			return hr;
		}
	}

	// Update the data
	CMANGOElement* pCurElement = m_Elements.GetAt( iElement );
	*pCurElement = *pElement;

	return S_OK;
}


//--------------------------------------------------------------------------------------
void CMANGOControl::Refresh()
{
	m_bMouseOver = false;
	m_bHasFocus = false;

	for( int i=0; i < m_Elements.GetSize(); i++ )
	{
		CMANGOElement* pElement = m_Elements.GetAt( i );
		pElement->Refresh();
	}
}


//--------------------------------------------------------------------------------------
void CMANGOControl::UpdateRects()
{
	SetRect( &m_rcBoundingBox, m_x, m_y, m_x + m_width, m_y + m_height );
}
