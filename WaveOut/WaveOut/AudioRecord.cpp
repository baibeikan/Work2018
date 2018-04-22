#include "stdafx.h"
#include "AudioRecord.h"
#include <Windows.h>
#include <stdio.h>


AudioRecord::AudioRecord()
{
	m_bMin = 0;
	m_bMax = 0;
}

AudioRecord::~AudioRecord()
{
}

void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample)
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nCh;
	m_WaveFormat->nSamplesPerSec = nSampleRate;
	m_WaveFormat->nAvgBytesPerSec = nSampleRate * nCh * BitsPerSample / 8;
	m_WaveFormat->nBlockAlign = m_WaveFormat->nChannels * BitsPerSample / 8;
	m_WaveFormat->wBitsPerSample = BitsPerSample;
	m_WaveFormat->cbSize = 0;
}

void AudioRecord::StartRecord(UINT nWaveID)
{
	UINT count = waveInGetNumDevs();//1
	if (count == 0)
	{
		printf("\n音频输入数量：%d\n", count);
		return;
	}

	if (count > nWaveID)
	{
		WAVEINCAPS waveIncaps;
		MMRESULT mmResult = waveInGetDevCaps(nWaveID, &waveIncaps, sizeof(WAVEINCAPS));//2
		printf("\n音频输入设备：%s\n", waveIncaps.szPname);
		m_WaveName = waveIncaps.szPname;

		OutputDebugString(waveIncaps.szPname);
		OutputDebugString(_T("\n"));

		if (MMSYSERR_NOERROR == mmResult)
		{
			WAVEFORMATEX pwfx;
			WaveInitFormat(&pwfx, 1, 44100, 16);
			printf("\n请求打开音频输入设备");
			printf("\n采样参数：单声道 8kHz 8bit\n");
			//mmResult = waveInOpen(&phwi, WAVE_MAPPER, &pwfx, (DWORD)(MicCallback), (DWORD_PTR)this, CALLBACK_FUNCTION);//3
			mmResult = waveInOpen(&phwi, nWaveID, &pwfx, (DWORD)(MicCallback), (DWORD_PTR)this, CALLBACK_FUNCTION);//3
			UINT waveInID = 0;
			waveInGetID(phwi, &waveInID);

			if (MMSYSERR_NOERROR == mmResult)
			{
				WAVEHDR pwh1;

				char buffer1[WAVE_BUFF_SIZE];
				pwh1.lpData = buffer1;
				pwh1.dwBufferLength = WAVE_BUFF_SIZE;
				pwh1.dwUser = 1;
				pwh1.dwFlags = 0;
				mmResult = waveInPrepareHeader(phwi, &pwh1, sizeof(WAVEHDR));//4
				printf("\n准备缓冲区1");

				WAVEHDR pwh2;
				char buffer2[WAVE_BUFF_SIZE];
				pwh2.lpData = buffer2;
				pwh2.dwBufferLength = WAVE_BUFF_SIZE;
				pwh2.dwUser = 2;
				pwh2.dwFlags = 0;
				mmResult = waveInPrepareHeader(phwi, &pwh2, sizeof(WAVEHDR));//4
				printf("\n准备缓冲区2\n");

				if (MMSYSERR_NOERROR == mmResult)
				{
					mmResult = waveInAddBuffer(phwi, &pwh1, sizeof(WAVEHDR));//5
					printf("\n将缓冲区1加入音频输入设备");
					mmResult = waveInAddBuffer(phwi, &pwh2, sizeof(WAVEHDR));//5
					printf("\n将缓冲区2加入音频输入设备\n");

					if (MMSYSERR_NOERROR == mmResult)
					{
						mmResult = waveInStart(phwi);//6
						printf("\n请求开始录音\n");
					}
				}
			}

		}
	}
}

void AudioRecord::StopRecord()
{
	//waveInReset(phwi);
	//OutputDebugStringA("waveInReset\n");
	waveInStop(phwi);
	OutputDebugStringA("waveInStop\n");
	waveInClose(phwi);
	OutputDebugStringA("waveInClose\n");
}

float AudioRecord::GetAudioVolume()
{
	return max(abs(m_bMin), m_bMax);
}

DWORD CALLBACK AudioRecord::MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	AudioRecord* pAudioRecord = (AudioRecord*)dwInstance;
	if (pAudioRecord == nullptr)
		return 0;

	switch (uMsg)
	{
	case WIM_OPEN:
		printf("\n设备已经打开...\n");
		break;

	case WIM_DATA:
		if (1)
		{
			LPWAVEHDR lpWaveHdr = ((LPWAVEHDR)dwParam1);
			printf("\n缓冲区%d存满...\n", lpWaveHdr->dwUser);
			pAudioRecord->PushBuffer(lpWaveHdr->lpData, lpWaveHdr->dwBytesRecorded);
			waveInAddBuffer(hwavein, lpWaveHdr, sizeof(WAVEHDR));
		}
		break;
	case WIM_CLOSE:
		printf("\n设备已经关闭...\n");
		break;
	default:
		break;
	}
	return 0;
}

void AudioRecord::PushBuffer(char* _dataRecorded, UINT _sizeRecorded)
{
	byte buffer1[WAVE_BUFF_SIZE];
	if (_sizeRecorded > WAVE_BUFF_SIZE)
		return;

	memcpy(buffer1, _dataRecorded, _sizeRecorded);

	short* pBuffer = (short*)buffer1;

	UINT sizeRecorded = _sizeRecorded / 2;

	m_bMin = 0;
	m_bMax = 0;
	if (_sizeRecorded > 0)
	{
		m_bMin = pBuffer[0], m_bMax = m_bMin;
		for (UINT i = 1; i < sizeRecorded; ++i)
		{
			short code = buffer1[i];
			if (code < m_bMin)
			{
				m_bMin = code;
			}

			if (code > m_bMax)
			{
				m_bMax = code;
			}
		}

		TCHAR szBuffer[512] = {};
		_stprintf_s(szBuffer, _T("This=%s Min=%d Max=%d\n"), m_WaveName.c_str(), m_bMin, m_bMax);
		OutputDebugString(szBuffer);

		printf("min=%d, max=%d\n", m_bMin, m_bMax);
	}
}
