
// WaveEdit.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWaveEditApp:
// �йش����ʵ�֣������ WaveEdit.cpp
//

class CWaveEditApp : public CWinApp
{
public:
	CWaveEditApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWaveEditApp theApp;