//--------------------------------------------------------------------------------------
// File: D3DEnumeration.h  
//
// Helper functions for Direct3D programming.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------

#ifndef MANGO_ENUMERATION_H
#define MANGO_ENUMERATION_H

#pragma once

//--------------------------------------------------------------------------------------
// A depth/stencil buffer format that is incompatible with a
// multisample type.
//--------------------------------------------------------------------------------------
class CD3DEnumAdapterInfo;
class CD3DEnumDeviceInfo;
struct CD3DEnumDeviceSettingsCombo;
struct MANGODeviceSettings;

struct CD3DEnumDSMSConflict
{
	D3DFORMAT DSFormat;
	D3DMULTISAMPLE_TYPE MSType;
};



class CD3DEnumeration
{
public:
	void SetRequirePostPixelShaderBlending( bool bRequire ) { m_bRequirePostPixelShaderBlending = bRequire; }
	void SetResolutionMinMax( UINT nMinWidth, UINT nMinHeight, UINT nMaxWidth, UINT nMaxHeight );  
	void SetRefreshMinMax( UINT nMin, UINT nMax );
	void SetMultisampleQualityMax( UINT nMax );    
	void GetPossibleVertexProcessingList( bool* pbSoftwareVP, bool* pbHardwareVP, bool* pbPureHarewareVP, bool* pbMixedVP );
	void SetPossibleVertexProcessingList( bool bSoftwareVP, bool bHardwareVP, bool bPureHarewareVP, bool bMixedVP );
	CMangoArray<D3DFORMAT>*				GetPossibleDepthStencilFormatList();   
	CMangoArray<D3DMULTISAMPLE_TYPE>*	GetPossibleMultisampleTypeList();   
	CMangoArray<UINT>*					GetPossiblePresentIntervalList();
	void ResetPossibleDepthStencilFormats();
	void ResetPossibleMultisampleTypeList();
	void ResetPossiblePresentIntervalList();

	// Call Enumerate() to enumerate available D3D adapters, devices, modes, etc.
	HRESULT Enumerate( IDirect3D9* pD3D = NULL,
		LPMANGOCALLBACKISDEVICEACCEPTABLE IsDeviceAcceptableFunc = NULL,
		void* pIsDeviceAcceptableFuncUserContext = NULL );

	// These should be called after Enumerate() is called
	CMangoArray<CD3DEnumAdapterInfo*>*		GetAdapterInfoList();  
	CD3DEnumAdapterInfo*                    GetAdapterInfo( UINT AdapterOrdinal );  
	CD3DEnumDeviceInfo*                     GetDeviceInfo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType );    
	CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( MANGODeviceSettings* pDeviceSettings ) { return GetDeviceSettingsCombo( pDeviceSettings->AdapterOrdinal, pDeviceSettings->DeviceType, pDeviceSettings->AdapterFormat, pDeviceSettings->pp.BackBufferFormat, pDeviceSettings->pp.Windowed ); }
	CD3DEnumDeviceSettingsCombo*            GetDeviceSettingsCombo( UINT AdapterOrdinal, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL Windowed );  

	~CD3DEnumeration();

private:
	friend CD3DEnumeration* MANGOGetEnumeration(); 

	// Use MANGOGetEnumeration() to access global instance
	CD3DEnumeration();

	IDirect3D9* m_pD3D;                                    
	LPMANGOCALLBACKISDEVICEACCEPTABLE m_IsDeviceAcceptableFunc;
	void* m_pIsDeviceAcceptableFuncUserContext;
	bool m_bRequirePostPixelShaderBlending;
	CMangoArray<D3DFORMAT> m_DepthStecilPossibleList;
	CMangoArray<D3DMULTISAMPLE_TYPE> m_MultiSampleTypeList;
	CMangoArray<UINT> m_PresentIntervalList;

	bool m_bSoftwareVP;
	bool m_bHardwareVP;
	bool m_bPureHarewareVP;
	bool m_bMixedVP;

	UINT m_nMinWidth;
	UINT m_nMaxWidth;
	UINT m_nMinHeight;
	UINT m_nMaxHeight;
	UINT m_nRefreshMin;
	UINT m_nRefreshMax;
	UINT m_nMultisampleQualityMax;

	// Array of CD3DEnumAdapterInfo* with unique AdapterOrdinals
	CMangoArray<CD3DEnumAdapterInfo*> m_AdapterInfoList;  

	HRESULT EnumerateDevices( CD3DEnumAdapterInfo* pAdapterInfo, CMangoArray<D3DFORMAT>* pAdapterFormatList );
	HRESULT EnumerateDeviceCombos( CD3DEnumAdapterInfo* pAdapterInfo, CD3DEnumDeviceInfo* pDeviceInfo, CMangoArray<D3DFORMAT>* pAdapterFormatList );
	void BuildDepthStencilFormatList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
	void BuildMultiSampleTypeList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
	void BuildDSMSConflictList( CD3DEnumDeviceSettingsCombo* pDeviceCombo );
	void BuildPresentIntervalList( CD3DEnumDeviceInfo* pDeviceInfo, CD3DEnumDeviceSettingsCombo* pDeviceCombo );
	void ClearAdapterInfoList();
};

CD3DEnumeration* MANGOGetEnumeration(); 

//--------------------------------------------------------------------------------------
// A struct describing device settings that contains a unique combination of 
// adapter format, back buffer format, and windowed that is compatible with a 
// particular Direct3D device and the app.
//--------------------------------------------------------------------------------------

struct CD3DEnumDeviceSettingsCombo
{
	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DFORMAT AdapterFormat;
	D3DFORMAT BackBufferFormat;
	BOOL Windowed;

	CMangoArray<D3DFORMAT> depthStencilFormatList; // List of D3DFORMATs
	CMangoArray<D3DMULTISAMPLE_TYPE> multiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
	CMangoArray<DWORD> multiSampleQualityList; // List of number of quality levels for each multisample type
	CMangoArray<UINT> presentIntervalList; // List of D3DPRESENT flags
	CMangoArray<CD3DEnumDSMSConflict> DSMSConflictList; // List of CD3DEnumDSMSConflict

	CD3DEnumAdapterInfo* pAdapterInfo;
	CD3DEnumDeviceInfo* pDeviceInfo;
};

#endif