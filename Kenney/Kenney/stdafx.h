// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
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
// TODO: 在此处引用程序需要的其他头文件
#include "Resource.h"
#include "Mango.h"

#define CLIENT_WIDTH 1280
#define CLIENT_HEIGHT 720

#define BLOCKLENGTH 64
#define BG_HEIGHT 1024
#define MAX_ROW BG_HEIGHT/BLOCKLENGTH

extern bool g_bDebug;