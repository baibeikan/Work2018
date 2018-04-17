// stdafx.cpp : 只包括标准包含文件的源文件
// Ball.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
//引用任何所需的附加头文件，而不是在此文件中引用


void KeyInit()
{
	GetKeyboardState(CurKey);
}
void KeyUpdate()
{
	memcpy(PreKey,CurKey,256);
	GetKeyboardState(CurKey);
}

bool GetKeyDown( unsigned char VK )
{
	if (!(PreKey[VK]&0x80))
	{
		if (CurKey[VK]&0x80)
		{
			return true;
		}
	}
	return false;
}

bool GetKeyUp( unsigned char VK )
{
	if (PreKey[VK]&0x80)
	{
		if (!(CurKey[VK]&0x80))
		{
			return true;
		}
	}
	return false;
}

void TimeUpdate()
{
	g_EndTime = timeGetTime()/1000.f;
	g_FrameTime = g_EndTime - g_BeginTime;
	g_BeginTime = g_EndTime;
}

float GetFrameTime()
{
	return g_FrameTime;
}

void TimeInit()
{
	g_EndTime = 0.0f;
	g_FrameTime = 0.0f;
	g_BeginTime = timeGetTime()/1000.f;

}

bool GetClientCursorPos(Vector &CurPos )
{
	static POINT Pt;
	GetCursorPos(&Pt);
	if (hWnd == NULL)
	{
		return false;
	}
	ScreenToClient(hWnd,&Pt);
	CurPos.mx = float(Pt.x);
	CurPos.my = float(Pt.y);
	return true;
}

bool PosInRect( RECT const &SrcRect,Vector const &pos )
{
	static POINT Pt;
	Pt.x = int(pos.mx);
	Pt.y = int(pos.my);
	if (PtInRect(&SrcRect,Pt))
	{
		return true;
	}
	return false;
}