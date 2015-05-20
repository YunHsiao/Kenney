//-----------------------------------------------------------------------------
// All controls must be assigned to a dialog, which handles
// input and rendering for the controls.
//-----------------------------------------------------------------------------

#ifndef MANGO_GUI_SCENE_H
#define MANGO_GUI_SCENE_H

#pragma once
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class CMANGOResourceManager;
class CMANGOControl;
class CMANGOButton;
class CMANGOImage;
class CMANGOLable;
class CMANGOCheckBox;
class CMANGORadioButton;
class CMANGOComboBox;
class CMANGOSlider;
class CMANGOEditBox;
class CMANGOIMEEditBox;
class CMANGOListBox;
class CMANGOScrollBar;
class CMANGOElement;
struct MANGOElementHolder;
struct MANGOTextureNode;
struct MANGOFontNode;
typedef VOID (CALLBACK *PCALLBACKMANGOGUIEVENT) ( UINT nEvent, int nControlID, CMANGOControl* pControl, void* pUserContext );

//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct MANGOElementHolder
{
	UINT nControlType;
	UINT iElement;

	CMANGOElement Element;
};

//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum MANGO_CONTROL_TYPE 
{ 
	MANGO_CONTROL_BUTTON, 
	MANGO_CONTROL_LABLE, 
	MANGO_CONTROL_IMAGE, 
	MANGO_CONTROL_CHECKBOX,
	MANGO_CONTROL_RADIOBUTTON,
	MANGO_CONTROL_COMBOBOX,
	MANGO_CONTROL_SLIDER,
	MANGO_CONTROL_EDITBOX,
	MANGO_CONTROL_IMEEDITBOX,
	MANGO_CONTROL_LISTBOX,
	MANGO_CONTROL_SCROLLBAR,
};

class CMANGOScene
{
	friend class CMANGOResourceManager;

public:
	CMANGOScene();
	~CMANGOScene();

	// Need to call this now
	void Init( CMANGOResourceManager* pManager, bool bRegisterDialog = true, char*  psCodexFilename = ".\\interface\\info\\default.codex" );
	void Init( CMANGOResourceManager* pManager, bool bRegisterDialog, LPCWSTR pszControlTextureFilename, char*  psCodexFilename = ".\\interface\\info\\default.codex");
	void Init( CMANGOResourceManager* pManager, bool bRegisterDialog, LPCWSTR szControlTextureResourceName, HMODULE hControlTextureResourceModule, char*  psCodexFilename = ".\\interface\\info\\default.codex");

	// Windows message handler
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Control creation
	HRESULT AddImage( int ID, int x, int y, int width, int height ,float z = 1.0f, bool bIsDefault=false, CMANGOImage** ppCreated=NULL);
	HRESULT AddLable( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z = 1.0f, bool bIsDefault=false, CMANGOLable** ppCreated=NULL );
	HRESULT AddButton( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z = 1.0f, UINT nHotkey=0, bool bIsDefault=false, CMANGOButton** ppCreated=NULL );
	HRESULT AddCheckBox( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z = 1.0f, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CMANGOCheckBox** ppCreated=NULL);
	HRESULT AddRadioButton( int ID, UINT nButtonGroup, LPCWSTR strText, int x, int y, int width, int height ,float z = 1.0f, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CMANGORadioButton** ppCreated=NULL );
	HRESULT AddComboBox( int ID, int x, int y, int width, int height ,float z = 1.0f, UINT nHotKey=0, bool bIsDefault=false, CMANGOComboBox** ppCreated=NULL);
	HRESULT AddSlider( int ID, int x, int y, int width, int height ,float z = 1.0f, int min=0, int max=100, int value=50, bool bIsDefault=false, CMANGOSlider** ppCreated=NULL );
	HRESULT AddEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z = 1.0f, bool bIsDefault=false, CMANGOEditBox** ppCreated=NULL);
	HRESULT AddIMEEditBox( int ID, LPCWSTR strText, int x, int y, int width, int height ,float z = 1.0f, bool bIsDefault=false, CMANGOIMEEditBox** ppCreated=NULL );
	HRESULT AddListBox( int ID, int x, int y, int width, int height ,float z = 1.0f, DWORD dwStyle=0, CMANGOListBox** ppCreated=NULL );
	HRESULT AddScrollBar( int ID, int x, int y, int width, int height ,float z = 1.0f, int iTrackStart = 0, int iTrackEnd = 10, int iTrackPos = 0, int iPageSize = 1, CMANGOScrollBar** ppCreated = NULL );
	HRESULT AddControl( CMANGOControl* pControl );
	HRESULT InitControl( CMANGOControl* pControl );

	// Control retrieval
	CMANGOImage*       GetImageByID( int ID ) { return (CMANGOImage*) GetControl( ID, MANGO_CONTROL_IMAGE ); }
	CMANGOLable*       GetLableByID( int ID ) { return (CMANGOLable*) GetControl( ID, MANGO_CONTROL_LABLE ); }
	CMANGOButton*      GetButtonByID( int ID ) { return (CMANGOButton*) GetControl( ID, MANGO_CONTROL_BUTTON ); }
	CMANGOCheckBox*    GetCheckBoxByID( int ID ) { return (CMANGOCheckBox*) GetControl( ID, MANGO_CONTROL_CHECKBOX ); }
	CMANGORadioButton* GetRadioButtonByID( int ID ) { return (CMANGORadioButton*) GetControl( ID, MANGO_CONTROL_RADIOBUTTON ); }
	CMANGOComboBox*    GetComboBoxByID( int ID ) { return (CMANGOComboBox*) GetControl( ID, MANGO_CONTROL_COMBOBOX ); }
	CMANGOSlider*      GetSliderByID( int ID ) { return (CMANGOSlider*) GetControl( ID, MANGO_CONTROL_SLIDER ); }
	CMANGOEditBox*     GetEditBoxByID( int ID ) { return (CMANGOEditBox*) GetControl( ID, MANGO_CONTROL_EDITBOX ); }
	CMANGOIMEEditBox*  GetIMEEditBoxByID( int ID ) { return (CMANGOIMEEditBox*) GetControl( ID, MANGO_CONTROL_IMEEDITBOX ); }
	CMANGOListBox*     GetListBoxByID( int ID ) { return (CMANGOListBox*) GetControl( ID, MANGO_CONTROL_LISTBOX ); }

	CMANGOControl* GetControlByID( int ID );
	CMANGOControl* GetControl( int ID, UINT nControlType );
	CMANGOControl* GetControlAtPoint( POINT pt );

	bool GetControlEnabled( int ID );
	void SetControlEnabled( int ID, bool bEnabled );

	void ClearRadioButtonGroup( UINT nGroup );
	void ClearComboBox( int ID );

	// Access the default display Elements used when adding new controls
	HRESULT        SetDefaultElement( UINT nControlType, UINT iElement, CMANGOElement* pElement );
	CMANGOElement* GetDefaultElement( UINT nControlType, UINT iElement );

	// Methods called by controls
	void SendEvent( UINT nEvent, bool bTriggeredByUser, CMANGOControl* pControl );
	void RequestFocus( CMANGOControl* pControl );

	// Render helpers
	HRESULT DrawRect( RECT* pRect, D3DCOLOR color );
	HRESULT DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color );
	HRESULT DrawSprite( CMANGOElement* pElement, RECT* prcDest );
	HRESULT CalcTextRect( LPCWSTR strText, CMANGOElement* pElement, RECT* prcDest, int nCount = -1 );
	HRESULT DrawText( LPCWSTR strText, CMANGOElement* pElement, RECT* prcDest, bool bShadow = false, int nCount = -1 );

	// Attributes
	bool IsVisible() { return m_bVisible; }
	void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	bool GetMinimized() { return m_bMinimized; }
	void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
	void SetBackgroundColors( D3DCOLOR colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
	void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight );
	void EnableCaption( bool bEnable ) { m_bCaption = bEnable; }
	int  GetCaptionHeight() const { return m_nCaptionHeight; }
	void SetCaptionHeight( int nHeight ) { m_nCaptionHeight = nHeight; }
	void SetCaptionText( const WCHAR *pwszText ) { StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText); }
	void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
	void SetLocation( int x, int y ) { m_x = x; m_y = y;}
	void SetSize( int width, int height ) { m_width = width; m_height = height;  }
	int  GetWidth() { return m_width; }
	int  GetHeight() { return m_height; }

	static void SetRefreshTime( float fTime ){ s_fTimeRefresh = fTime; }

	static CMANGOControl* GetNextControl( CMANGOControl* pControl );
	static CMANGOControl* GetPrevControl( CMANGOControl* pControl );

	void RemoveControl( int ID );
	void RemoveAllControls();

	// Sets the callback used to notify the app of control events
	void SetCallback( PCALLBACKMANGOGUIEVENT pCallback, void* pUserContext = NULL );
	void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
	void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
	void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }
	bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

	// Device state notification
	void	Refresh();
	HRESULT OnRender( float fElapsedTime );

	// Shared resource access. Indexed fonts and textures are shared among
	// all the controls.
	HRESULT       SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight );
	MANGOFontNode* GetFont( UINT index );

	HRESULT           SetTexture( UINT index, LPCWSTR strFilename );
	HRESULT           SetTexture( UINT index, LPCWSTR strResourceName, HMODULE hResourceModule );
	MANGOTextureNode* GetTexture( UINT index );

	CMANGOResourceManager* GetManager() { return m_pManager; }

	static void ClearFocus();
	void FocusDefaultControl();

	bool  m_bNonUserEvents;
	bool  m_bKeyboardInput;
	bool  m_bMouseInput;
	int   m_SceneID;
	std::wstring m_wsSceneName;

private:
	int m_nDefaultControlID;

	static double s_fTimeRefresh;
	double m_fTimeLastRefresh;

	// Initialize default Elements
	void InitDefaultElements(char * strName );

	// Windows message handlers
	void OnMouseMove( POINT pt );
	void OnMouseUp( POINT pt );

	void SetNextDialog( CMANGOScene* pNextDialog );

	// Control events
	bool OnCycleFocus( bool bForward );
	void sort();


	static CMANGOControl* s_pControlFocus;        // The control which has focus
	static CMANGOControl* s_pControlPressed;      // The control currently pressed

	CMANGOControl* m_pControlMouseOver;           // The control which is hovered over

	bool  m_bVisible;
	bool  m_bCaption;
	bool  m_bMinimized;
	bool  m_bDrag;
	WCHAR m_wszCaption[256];

	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_nCaptionHeight;

	D3DCOLOR m_colorTopLeft;
	D3DCOLOR m_colorTopRight;
	D3DCOLOR m_colorBottomLeft;
	D3DCOLOR m_colorBottomRight;

	CMANGOResourceManager* m_pManager;
	PCALLBACKMANGOGUIEVENT m_pCallbackEvent;
	void* m_pCallbackEventUserContext;

	CMangoArray< int > m_Textures;   // Index into m_TextureCache;
	CMangoArray< int > m_Fonts;      // Index into m_FontCache;

	CMangoArray< CMANGOControl* > m_Controls;
	CMangoArray< MANGOElementHolder* > m_DefaultElements;

	CMANGOElement m_CapElement;  // Element for the caption

	CMANGOScene* m_pNextDialog;
	CMANGOScene* m_pPrevDialog;
};

#endif //MANGO_GUI_SCENE_H