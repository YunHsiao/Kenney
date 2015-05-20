#ifndef MANGO_GUI_UNIBUFFER_H
#define MANGO_GUI_UNIBUFFER_H

#pragma once
#include <usp10.h>
#include <dimm.h>

//-----------------------------------------------------------------------------
// CUniBuffer class for the edit control
//-----------------------------------------------------------------------------
class CUniBuffer
{
public:
	CUniBuffer( int nInitialSize = 1 );
	~CUniBuffer();

	static void Initialize();
	static void Uninitialize();

	int  GetBufferSize() { return m_nBufferSize; }
	bool SetBufferSize( int nSize );
	int  GetTextSize()  { return lstrlenW( m_pwszBuffer ); }
	const WCHAR* GetBuffer() { return m_pwszBuffer; }
	const WCHAR& operator[]( int n ) const { return m_pwszBuffer[n]; }
	WCHAR& operator[]( int n );
	MANGOFontNode* GetFontNode() { return m_pFontNode; }
	void SetFontNode( MANGOFontNode *pFontNode ) { m_pFontNode = pFontNode; }
	void Clear();

	bool InsertChar( int nIndex, WCHAR wChar ); // Inserts the char at specified index. If nIndex == -1, insert to the end.
	bool RemoveChar( int nIndex );  // Removes the char at specified index. If nIndex == -1, remove the last char.
	bool InsertString( int nIndex, const WCHAR *pStr, int nCount = -1 );  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
	bool SetText( LPCWSTR wszText );

	// Uniscribe
	HRESULT CPtoX( int nCP, BOOL bTrail, int *pX );
	HRESULT XtoCP( int nX, int *pCP, int *pnTrail );
	void GetPriorItemPos( int nCP, int *pPrior );
	void GetNextItemPos( int nCP, int *pPrior );

private:
	HRESULT Analyse();      // Uniscribe -- Analyse() analyses the string in the buffer

	WCHAR* m_pwszBuffer;    // Buffer to hold text
	int    m_nBufferSize;   // Size of the buffer allocated, in characters

	// Uniscribe-specific
	MANGOFontNode* m_pFontNode;          // Font node for the font that this buffer uses
	bool m_bAnalyseRequired;            // True if the string has changed since last analysis.
	SCRIPT_STRING_ANALYSIS m_Analysis;  // Analysis for the current string

private:
	// Empty implementation of the Uniscribe API
	static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, int, BOOL, int* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, int, int*, int* ) { return E_NOTIMPL; }
	static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
	static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
	static const int* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

	// Function pointers
	static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
	static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
	static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* );
	static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* );
	static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
	static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
	static const int* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

	static HINSTANCE s_hDll;  // Uniscribe DLL handle

};

#endif
