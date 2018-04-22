#ifndef __AudioRecord_H__
#define __AudioRecord_H__

#define WAVE_BUFF_SIZE (1024 * 16)
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <string>
#ifdef UNICODE
typedef std::wstring string;
#else
typedef std::string string;
#endif

class AudioRecord
{
public:
	AudioRecord();
	~AudioRecord();

public:
	void StartRecord(UINT nWaveID);

	void StopRecord();

	// 获得声音大小0.0 - 1.0
	float GetAudioVolume(); 

	static DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

private:
	void PushBuffer(char* _data, UINT _size);

private:
	HWAVEIN phwi;
	short m_bMin;
	short m_bMax;
	string m_WaveName;
};

#endif
