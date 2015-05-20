#pragma once

struct CD3DEnumDeviceSettingsCombo;

class CD3DEnumDeviceInfo
{
public:
	~CD3DEnumDeviceInfo();

	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DCAPS9 Caps;

	// List of CD3DEnumDeviceSettingsCombo* with a unique set 
	// of AdapterFormat, BackBufferFormat, and Windowed
	CMangoArray<CD3DEnumDeviceSettingsCombo*> deviceSettingsComboList; 
};
