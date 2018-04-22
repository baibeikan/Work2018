
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"
#include "resource.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#define SAFE_DELETE_ARR(ptr) { if(ptr) { delete[] ptr; ptr = NULL; }}
#define SAFE_DELETE(ptr) { if(ptr) { delete ptr; ptr = NULL; }}

typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;
typedef  __int16 int16_t;
typedef  __int32 int32_t;

typedef struct
{
	char *szName;     // wav�ļ���
	short wFormatTag;				//�������ͱ�ʾPCM(0001)�� ADPCM(0002)	2�ֽ�
	short nChannels;			//������	2�ֽ�
	unsigned long nAvgBytesPerSec;	//ÿ���¼�������ֽ���	4�ֽ�
	short	nBlockAlign;			//������ռ�ֽڴ�С	2�ֽ� wBitsPerSample*nChannels/8
	short  wBitsPerSample;			//����λ��	2�ֽ� 8��ʾ��ͨ�� 16˫ͨ��
	long nSamplesPerSec;    // �ܹ����������� ÿ�������
	uint32_t dataSize;	//"DATA"���С	4�ֽ�
	char *data;		//��Ų�������
}WAV_INFO;