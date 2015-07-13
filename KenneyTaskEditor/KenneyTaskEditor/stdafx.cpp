// stdafx.cpp : 只包括标准包含文件的源文件
// KenneyTaskEditor.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


std::wstring CU_A2U(const std::string& _str)
{
	//！第一次调用得到大小，源字符串的长度
	int	iSize	=	MultiByteToWideChar (CP_ACP, 0, _str.c_str(), -1, NULL, 0);
	//!加一的目的是保存结束字符
	std::wstring	strTemp;
	//!申请同样大的空间来保存转化出来的UNICODE字符串
	strTemp.resize(iSize);
	MultiByteToWideChar(CP_ACP, 0, _str.c_str(), -1, &strTemp[0], iSize);
	return	strTemp;
}


std::string	CU_U2A(const std::wstring& _str)
{
	std::string		strTemp;
	//！第一次调用得到大小，源字符串的长度
	int	iSize	=	WideCharToMultiByte(CP_OEMCP,NULL,_str.c_str(),-1,NULL,0,NULL,FALSE);

	//!因为UNICODE是双字节存储，而ANSI是单字节字符存储
	//!那么空间必须足够大才可以容纳转化完的字符，所以大小是原来的两倍
	strTemp.resize(iSize*2);
	WideCharToMultiByte (CP_OEMCP,NULL,_str.c_str(),-1,&strTemp[0],iSize,NULL,FALSE);
	return	strTemp.c_str();
}
