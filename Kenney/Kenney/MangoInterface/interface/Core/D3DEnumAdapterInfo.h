//--------------------------------------------------------------------------------------
// File: Utility.h  
//
// Helper functions for Direct3D programming.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once

class CD3DEnumAdapterInfo
{
public:
	~CD3DEnumAdapterInfo();

	UINT AdapterOrdinal;
	D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
	WCHAR szUniqueDescription[256];

	CMangoArray<D3DDISPLAYMODE> displayModeList; // Array of supported D3DDISPLAYMODEs
	CMangoArray<CD3DEnumDeviceInfo*> deviceInfoList; // Array of CD3DEnumDeviceInfo* with unique supported DeviceTypes
};
