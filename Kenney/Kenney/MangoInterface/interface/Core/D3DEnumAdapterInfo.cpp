#include "../Mango.h"

//--------------------------------------------------------------------------------------
CD3DEnumAdapterInfo::~CD3DEnumAdapterInfo( void )
{
	CD3DEnumDeviceInfo* pDeviceInfo;
	for( int i=0; i<deviceInfoList.GetSize(); i++ )
	{
		pDeviceInfo = deviceInfoList.GetAt(i);
		delete pDeviceInfo;
	}
	deviceInfoList.RemoveAll();
}



