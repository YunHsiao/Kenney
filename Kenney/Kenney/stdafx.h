// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#define _CRT_RAND_S
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// STL
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <fstream>

#include <tinyxml2.h>
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Resource.h"
#include "Mango.h"

#define CLIENT_WIDTH 1280
#define CLIENT_HEIGHT 720

#define BLOCKLENGTH 64
#define BG_HEIGHT 1024
#define MAX_ROW BG_HEIGHT/BLOCKLENGTH

extern bool g_bDebug;