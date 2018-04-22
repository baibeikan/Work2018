#ifndef __WAVEOUT_H__
#define __WAVEOUT_H__

#define WAVE_OUT_SIZE (256 * 1024)
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <string>
#include <queue>
#ifdef UNICODE
typedef std::wstring string;
#else
typedef std::string string;
#endif


void GetWavInfo(char * filename, WAV_INFO * wav_info);

struct WaveBuffer
{
	char* pBuffer;
	uint32_t nBufferSize;
	uint32_t nDataSize;
};

class WaveOut
{
public:
	WaveOut();
	~WaveOut();

public:
	void Play(char* szName);

	void Play(const WAV_INFO& wav_info);

	void Play(const WAVEFORMATEX& _wavformat);

	void Stop();

	void SetPitch(float pitch);

	void SetPlaybackRate(float playbackRate);

	void PushStream(char* _stream, uint32_t _streamSize);

	static DWORD CALLBACK WaveCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

	void Update();

private:
	bool FillDataToWaveHdr(WAVEHDR& wavehdr);

private:
	// wave 格式
	WAVEFORMATEX	wfx;
	HWAVEOUT		hwo;
	// 缓冲区
	WAVEHDR			m_arrWaveHdr[2];

	std::queue<WAVEHDR*> m_queWaveHdruffer;
	// 是不是向缓冲区里面写数据
	bool			m_bIsWaveOutWrite;

	WaveBuffer* m_pCurWaveBuffer;
	int				nOffset;
	std::queue<WaveBuffer*> m_queWaveBuffer;
	std::queue<WaveBuffer*> m_queWaveBufferPool;
};

#endif
