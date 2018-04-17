// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#ifndef AFX_STDAFX
#define AFX_STDAFX


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlimage.h>
#include <Mmsystem.h>
#pragma comment(lib,"Winmm.lib")

#define SAFE_DELETE(p)	 {if(p) { delete p; p = NULL;}}
#define SAFE_DELARR(p)	 {if(p) { delete []p; p = NULL;}}
#define SAFE_DELOBJ(p)	 {if(p) { DeleteObject(p); p = NULL;}}
#define SAFE_DELDC(p)	{if(p) { DeleteDC(p); p = NULL;}}

struct Vector; 
extern HWND hWnd;

unsigned char PreKey[256] = {};
unsigned char CurKey[256] = {};
void KeyInit();
void KeyUpdate();
bool GetKeyDown(unsigned char VK);
bool GetKeyUp(unsigned char VK);

bool GetClientCursorPos(Vector &CurPos);

float g_EndTime = 0.0f;
float g_BeginTime = 0.0f;
float g_FrameTime = 0.0f;
void TimeInit();
void TimeUpdate();
float GetFrameTime();

bool PosInRect(RECT const &SrcRect,Vector const &pos);


#define STOP_POWER 10.0f
#define POWER 1000.0f
#define M_PI 3.14159265358979323846f


// TODO: 在此处引用程序要求的附加头文件
#include "Vector.h"
#include "SrcBitmap.h"
#include "BallData.h"
#include "BallManager.h"
#include "Manage.h"

#endif
