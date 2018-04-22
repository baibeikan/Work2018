#include "stdafx.h"
#include "WaveIn.h"
#include <Windows.h>
#include <stdio.h>
#include "WaveEditDlg.h"

void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nChannels;
	m_WaveFormat->nSamplesPerSec = nSamplesPerSec;
	m_WaveFormat->wBitsPerSample = wBitsPerSample;
	m_WaveFormat->nBlockAlign = nChannels * wBitsPerSample / 8;
	m_WaveFormat->nAvgBytesPerSec = nSamplesPerSec * m_WaveFormat->nBlockAlign;
	m_WaveFormat->cbSize = 0;
}

WaveIn::WaveIn()
{
	phwi = NULL;
	m_waveData = NULL;

	m_wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_wavFormat.nChannels = 2;
	m_wavFormat.nSamplesPerSec = 44100;
	m_wavFormat.wBitsPerSample = 8;
	m_wavFormat.nAvgBytesPerSec = m_wavFormat.nSamplesPerSec * m_wavFormat.nChannels * m_wavFormat.wBitsPerSample / 8;
	m_wavFormat.nBlockAlign = m_wavFormat.nChannels * m_wavFormat.wBitsPerSample / 8;
	m_wavFormat.cbSize = 0;
	WaveInitFormat(&m_wavFormat, 2, 44100, 16);
}

WaveIn::~WaveIn()
{
	StopRecord();
}


void WaveIn::StartRecord(UINT nWaveID)
{
	UINT count = waveInGetNumDevs();//1
	if (count == 0)
	{
		printf("\n��Ƶ����������%d\n", count);
		return;
	}

	if (count > nWaveID)
	{
		WAVEINCAPS waveIncaps;
		MMRESULT mmResult = waveInGetDevCaps(nWaveID, &waveIncaps, sizeof(WAVEINCAPS));//2
		m_WaveName = waveIncaps.szPname;

		OutputDebugString(waveIncaps.szPname);
		OutputDebugString(_T("\n"));

		if (MMSYSERR_NOERROR == mmResult)
		{
			//m_wavFormat.wFormatTag = WAVE_FORMAT_PCM;
			//m_wavFormat.nChannels = 2;
			//m_wavFormat.nSamplesPerSec = 44100;
			//m_wavFormat.wBitsPerSample = 8;
			//m_wavFormat.nAvgBytesPerSec = m_wavFormat.nSamplesPerSec * m_wavFormat.nChannels * m_wavFormat.wBitsPerSample / 8;
			//m_wavFormat.nBlockAlign = m_wavFormat.nChannels * m_wavFormat.wBitsPerSample / 8;
			//m_wavFormat.cbSize = 0;

			printf("\n�������Ƶ�����豸");
			printf("\n���������������� 8kHz 8bit\n");
			//mmResult = waveInOpen(&phwi, WAVE_MAPPER, &pwfx, (DWORD)(MicCallback), (DWORD_PTR)this, CALLBACK_FUNCTION);//3
			mmResult = waveInOpen(&phwi, nWaveID, &m_wavFormat, (DWORD)(MicCallback), (DWORD_PTR)this, CALLBACK_FUNCTION);//3
			UINT waveInID = 0;
			waveInGetID(phwi, &waveInID);

			if (MMSYSERR_NOERROR == mmResult)
			{
				unsigned int recordSize = m_wavFormat.nAvgBytesPerSec * 0.5;
				wh1.dwBufferLength = recordSize;
				wh1.lpData = new char[wh1.dwBufferLength];
				wh1.dwUser = 1;
				wh1.dwFlags = 0;
				printf("\n׼��������1");
				printf("\n��������1������Ƶ�����豸");
				mmResult = waveInPrepareHeader(phwi, &wh1, sizeof(WAVEHDR));//4
				mmResult = waveInAddBuffer(phwi, &wh1, sizeof(WAVEHDR));//5

				wh2.dwBufferLength = recordSize;
				wh2.lpData = new char[wh2.dwBufferLength];
				wh2.dwUser = 2;
				wh2.dwFlags = 0;
				printf("\n׼��������2\n");
				mmResult = waveInPrepareHeader(phwi, &wh2, sizeof(WAVEHDR));//4
				printf("\n��������2������Ƶ�����豸\n");
				mmResult = waveInAddBuffer(phwi, &wh2, sizeof(WAVEHDR));//5

				if (MMSYSERR_NOERROR == mmResult)
				{

					if (MMSYSERR_NOERROR == mmResult)
					{
						mmResult = waveInStart(phwi);//6
						printf("\n����ʼ¼��\n");
					}
				}
			}

		}
	}
}

void WaveIn::StopRecord()
{
	waveInStop(phwi);
	OutputDebugStringA("waveInStop\n");
	waveInUnprepareHeader(phwi, &wh1, sizeof(WAVEHDR));//��������
	waveInUnprepareHeader(phwi, &wh2, sizeof(WAVEHDR));//��������
	waveInClose(phwi);
	OutputDebugStringA("waveInClose\n");

	SAFE_DELETE_ARR(wh1.lpData);
	SAFE_DELETE_ARR(wh2.lpData);
}

float WaveIn::GetAudioVolume()
{
	return max(abs(m_bMin), m_bMax);
}

DWORD CALLBACK WaveIn::MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	WaveIn* pWaveIn = (WaveIn*)dwInstance;
	if (pWaveIn == nullptr)
		return 0;

	switch (uMsg)
	{
	case WIM_OPEN:
		printf("\n�豸�Ѿ���...\n");
		break;

	case WIM_DATA:
		if (1)
		{
			LPWAVEHDR lpWaveHdr = ((LPWAVEHDR)dwParam1);
			printf("\n������%d����...\n", lpWaveHdr->dwUser);
			pWaveIn->PushBuffer(lpWaveHdr->lpData, lpWaveHdr->dwBytesRecorded);
			waveInPrepareHeader(hwavein, lpWaveHdr, sizeof(WAVEHDR));//4
			waveInAddBuffer(hwavein, lpWaveHdr, sizeof(WAVEHDR));
		}
		break;
	case WIM_CLOSE:
		printf("\n�豸�Ѿ��ر�...\n");
		break;
	default:
		break;
	}
	return 0;
}

void WaveIn::PushBuffer(char* _dataRecorded, UINT _sizeRecorded)
{
	//WAV_INFO waveInfo;
	//waveInfo.data = _dataRecorded;
	//waveInfo.dataSize = _sizeRecorded;
	//waveInfo.nAvgBytesPerSec = m_wavFormat.nAvgBytesPerSec;
	//waveInfo.nBlockAlign = m_wavFormat.nBlockAlign;
	//waveInfo.nChannels = m_wavFormat.nChannels;
	//waveInfo.nSamplesPerSec = m_wavFormat.nSamplesPerSec;
	//waveInfo.wBitsPerSample = m_wavFormat.wBitsPerSample;
	//waveInfo.wFormatTag = m_wavFormat.wFormatTag;

	//m_waveOut.Play(waveInfo);
	CWaveEditDlg* pWaveDlg = (CWaveEditDlg*)m_pOwner;
	pWaveDlg->PushWaveOutStream(_dataRecorded, _sizeRecorded);
	
}
