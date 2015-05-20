#include "../Mango.h"

//--------------------------------------------------------------------------------------
CD3DEnumDeviceInfo::~CD3DEnumDeviceInfo( void )
{
	CD3DEnumDeviceSettingsCombo* pDeviceCombo;
	for( int i=0; i<deviceSettingsComboList.GetSize(); i++ )
	{
		pDeviceCombo = deviceSettingsComboList.GetAt(i);
		delete pDeviceCombo;
	}
	deviceSettingsComboList.RemoveAll();
}
