// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// KenneyTaskEditor.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"


std::wstring CU_A2U(const std::string& _str)
{
	//����һ�ε��õõ���С��Դ�ַ����ĳ���
	int	iSize	=	MultiByteToWideChar (CP_ACP, 0, _str.c_str(), -1, NULL, 0);
	//!��һ��Ŀ���Ǳ�������ַ�
	std::wstring	strTemp;
	//!����ͬ����Ŀռ�������ת��������UNICODE�ַ���
	strTemp.resize(iSize);
	MultiByteToWideChar(CP_ACP, 0, _str.c_str(), -1, &strTemp[0], iSize);
	return	strTemp;
}


std::string	CU_U2A(const std::wstring& _str)
{
	std::string		strTemp;
	//����һ�ε��õõ���С��Դ�ַ����ĳ���
	int	iSize	=	WideCharToMultiByte(CP_OEMCP,NULL,_str.c_str(),-1,NULL,0,NULL,FALSE);

	//!��ΪUNICODE��˫�ֽڴ洢����ANSI�ǵ��ֽ��ַ��洢
	//!��ô�ռ�����㹻��ſ�������ת������ַ������Դ�С��ԭ��������
	strTemp.resize(iSize*2);
	WideCharToMultiByte (CP_OEMCP,NULL,_str.c_str(),-1,&strTemp[0],iSize,NULL,FALSE);
	return	strTemp.c_str();
}
