#include "stdafx.h"
#include "WaveOut.h"
#include <Windows.h>
#include <stdio.h>


WaveOut::WaveOut()
{
	hwo = nullptr;
	memset(&wfx, 0, sizeof(wfx));
	nOffset = 0;
	m_bIsWaveOutWrite = false;
	m_pCurWaveBuffer = NULL;

	memset(m_arrWaveHdr, 0, sizeof(m_arrWaveHdr));
	m_queWaveHdruffer.push(&m_arrWaveHdr[0]);
	m_queWaveHdruffer.push(&m_arrWaveHdr[1]);
}

WaveOut::~WaveOut()
{
	Stop();

	if (m_pCurWaveBuffer)
	{
		SAFE_DELETE_ARR(m_pCurWaveBuffer->pBuffer);
	}
	SAFE_DELETE(m_pCurWaveBuffer);

	while (m_queWaveBuffer.size() > 0)
	{
		WaveBuffer* pWaveBuffer = m_queWaveBuffer.front();
		m_queWaveBuffer.pop();

		SAFE_DELETE_ARR(pWaveBuffer->pBuffer);
		SAFE_DELETE(pWaveBuffer);
	}

	while (m_queWaveBufferPool.size() > 0)
	{
		WaveBuffer* pWaveBuffer = m_queWaveBufferPool.front();
		m_queWaveBufferPool.pop();

		SAFE_DELETE_ARR(pWaveBuffer->pBuffer);
		SAFE_DELETE(pWaveBuffer);
	}
}

void WaveOut::Play(char* szName)
{
	WAV_INFO wavInfo;
	GetWavInfo(szName, &wavInfo);
	Play(wavInfo);
	SAFE_DELETE_ARR(wavInfo.data);
}

void WaveOut::Play(const WAV_INFO& wav_info)
{
	if (wav_info.data == nullptr)
		return;

	if (wav_info.dataSize == 0)
		return;

	while(m_queWaveBuffer.size() > 0)
	{
		m_queWaveBufferPool.push(m_queWaveBuffer.front());
		m_queWaveBuffer.pop();
	}

	if(m_pCurWaveBuffer != NULL)
	{
		m_queWaveBufferPool.push(m_pCurWaveBuffer);
		m_pCurWaveBuffer = NULL;
	}
	nOffset = 0;
	PushStream(wav_info.data, wav_info.dataSize);

	WAVEFORMATEX waveformat;
	memset(&waveformat, 0, sizeof(waveformat));
	waveformat.wFormatTag = wav_info.wFormatTag; // WAVE_FORMAT_PCM;//设置波形声音的格式
	waveformat.nChannels = wav_info.nChannels;//设置音频文件的通道数量
	waveformat.nSamplesPerSec = wav_info.nSamplesPerSec;//设置每一个声道播放和记录时的样本频率
	waveformat.nBlockAlign = wav_info.nBlockAlign;//以字节为单位设置块对齐
	waveformat.wBitsPerSample = wav_info.wBitsPerSample; // 设置每个样本的位深,多为8或16
	waveformat.nAvgBytesPerSec = wav_info.nAvgBytesPerSec;//设置请求的平均传输数据率,单位byte/s。
	waveformat.cbSize = 0;//额外信息的大小

	if (hwo == NULL)
	{
		memcpy(&wfx, &waveformat, sizeof(WAVEFORMATEX));
		MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);//打开一个给定的波形音频输出装置来进行声音播放，方式为回调函数方式。假设是对话框程序，能够将第五个參数改为(DWORD)this。操作跟本Demo程序类似
		switch (mmresult)
		{
		case MMSYSERR_ALLOCATED:
			OutputDebugString(L"MMSYSERR_ALLOCATED");
			break;
		case MMSYSERR_BADDEVICEID:
			OutputDebugString(L"MMSYSERR_BADDEVICEID");
			break;
		case MMSYSERR_NODRIVER:
			OutputDebugString(L"MMSYSERR_NODRIVER");
			break;
		case MMSYSERR_NOMEM:
			OutputDebugString(L"MMSYSERR_NOMEM");
			break;
		case WAVERR_BADFORMAT:
			OutputDebugString(L"WAVERR_BADFORMAT");
			break;
		case  WAVERR_SYNC:
			OutputDebugString(L"WAVERR_SYNC");
			break;
		default:
			break;
		}
	}
	else
	{
		waveOutPause(hwo);
		while(m_queWaveHdruffer.size() > 0)
		{
			m_queWaveHdruffer.pop();
		}
		for (int i = 0; i < 2; ++i)
		{
			WAVEHDR& hdr = m_arrWaveHdr[i];
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//清理数据
			m_queWaveHdruffer.push(&hdr);
		}


		if (memcmp(&waveformat, &wfx, sizeof(WAVEFORMATEX)) == 0)
		{
			//播放格式与以前一样
			OutputDebugString(L"不用重新创建\n");
		}
		else
		{
			OutputDebugString(L"从新创建\n");

			waveOutPause(hwo);
			waveOutClose(hwo);
			hwo = NULL;
			// 重新创建
			memcpy(&wfx, &waveformat, sizeof(WAVEFORMATEX));
			//打开一个给定的波形音频输出装置来进行声音播放，方式为回调函数方式。假设是对话框程序，能够将第五个參数改为(DWORD)this。操作跟本Demo程序类似
			MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
		}
	}

	while(m_queWaveHdruffer.size() > 0)
	{
		LPWAVEHDR lpWaveHdr = m_queWaveHdruffer.front(); 
		memset(lpWaveHdr, 0, sizeof(WAVEHDR));
		if (FillDataToWaveHdr(*lpWaveHdr))
		{
			waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));//准备一个波形数据块用于播放
			waveOutWrite(hwo, lpWaveHdr, sizeof(WAVEHDR));//在音频媒体中播放第二个參数指定的数据，也相当于开启一个播放区的意思
			m_queWaveHdruffer.pop();
		}
		else
		{
			break;
		}
	}
	waveOutRestart(hwo);

	OutputDebugString(L"当前格式\n");
	OutputDebugString(L"wFormatTag=");
	OutputDebugString(_bstr_t(wfx.wFormatTag));
	OutputDebugString(L" nSamplesPerSec=");
	OutputDebugString(_bstr_t(wfx.nSamplesPerSec));
	OutputDebugString(L" nChannels=");
	OutputDebugString(_bstr_t(wfx.nChannels));
	OutputDebugString(L" wBitsPerSample=");
	OutputDebugString(_bstr_t(wfx.wBitsPerSample));
	OutputDebugString(L" nAvgBytesPerSec=");
	OutputDebugString(_bstr_t(wfx.nAvgBytesPerSec));
	OutputDebugString(L" nBlockAlign=");
	OutputDebugString(_bstr_t(wfx.nBlockAlign));
	OutputDebugString(L"\n");
	

}

void WaveOut::Play(const WAVEFORMATEX& _wavformat)
{
	while(m_queWaveBuffer.size() > 0)
	{
		m_queWaveBufferPool.push(m_queWaveBuffer.front());
		m_queWaveBuffer.pop();
	}

	if(m_pCurWaveBuffer != NULL)
	{
		m_queWaveBufferPool.push(m_pCurWaveBuffer);
		m_pCurWaveBuffer = NULL;
	}
	nOffset = 0;
	
	if (hwo != NULL)
	{
		waveOutPause(hwo);
		for (int i = 0; i < 2; ++i)
		{
			WAVEHDR& hdr = m_arrWaveHdr[i];
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//清理数据
		}
		if (memcmp(&_wavformat, &wfx, sizeof(WAVEFORMATEX)) == 0)
		{
			//播放格式与以前一样
			OutputDebugString(L"不用重新创建\n");
		}
		else
		{
			waveOutClose(hwo);
			hwo = NULL;
			OutputDebugString(L"从新创建\n");
		}
	}

	while(m_queWaveHdruffer.size() > 0)
	{
		m_queWaveHdruffer.pop();
	}
	for (int i = 0; i < 2; ++i)
	{
		m_queWaveHdruffer.push(&m_arrWaveHdr[i]);
	}

	if (hwo == NULL)
	{
		memcpy(&wfx, &_wavformat, sizeof(WAVEFORMATEX));
		MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);//打开一个给定的波形音频输出装置来进行声音播放，方式为回调函数方式。假设是对话框程序，能够将第五个參数改为(DWORD)this。操作跟本Demo程序类似
		switch (mmresult)
		{
		case MMSYSERR_ALLOCATED:
			OutputDebugString(L"MMSYSERR_ALLOCATED");
			break;
		case MMSYSERR_BADDEVICEID:
			OutputDebugString(L"MMSYSERR_BADDEVICEID");
			break;
		case MMSYSERR_NODRIVER:
			OutputDebugString(L"MMSYSERR_NODRIVER");
			break;
		case MMSYSERR_NOMEM:
			OutputDebugString(L"MMSYSERR_NOMEM");
			break;
		case WAVERR_BADFORMAT:
			OutputDebugString(L"WAVERR_BADFORMAT");
			break;
		case  WAVERR_SYNC:
			OutputDebugString(L"WAVERR_SYNC");
			break;
		default:
			break;
		}
		waveOutPause(hwo);
	}
	waveOutRestart(hwo);
}

void WaveOut::Stop()
{
	if (hwo != NULL)
	{
		do 
		{
			waveOutPause(hwo);
			Sleep(1);
		} while (m_bIsWaveOutWrite);
		
		while(m_queWaveHdruffer.size() > 0)
		{
			m_queWaveHdruffer.pop();
		}
		for (int i = 0; i < 2; ++i)
		{
			WAVEHDR& hdr = m_arrWaveHdr[i];
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//清理数据
			m_queWaveHdruffer.push(&hdr);
		}
		waveOutClose(hwo);
		hwo = NULL;
	}
}

void WaveOut::SetPitch(float pitch)
{
	if (hwo != NULL)
	{
		waveOutSetPitch(hwo, DWORD(pitch*0x10000));
	}
}

void WaveOut::SetPlaybackRate(float playbackRate)
{
	if (hwo != NULL)
	{
		DWORD dwPlaybackRate = DWORD(playbackRate * 0x10000);
		waveOutSetPlaybackRate(hwo, dwPlaybackRate);
	}
}

void WaveOut::PushStream(char* _pStream, uint32_t _nStreamSize)
{
	if (_pStream == NULL || _nStreamSize == 0)
		return;

	int nStreamOffset = 0;
	if (m_queWaveBuffer.size() > 0 && nStreamOffset < _nStreamSize)
	{
		WaveBuffer *pWaveBackBuffer = m_queWaveBuffer.back();
		if (pWaveBackBuffer->nDataSize < pWaveBackBuffer->nBufferSize)
		{
			uint32_t nCopySize = min(pWaveBackBuffer->nBufferSize - pWaveBackBuffer->nDataSize, _nStreamSize - nStreamOffset);
			memcpy(pWaveBackBuffer->pBuffer+ pWaveBackBuffer->nDataSize, _pStream+ nStreamOffset, nCopySize);
			nStreamOffset += nCopySize;
			pWaveBackBuffer->nDataSize += nCopySize;
		}
	}

	while (nStreamOffset < _nStreamSize)
	{
		WaveBuffer *pWaveBuffer = NULL;
		if (m_queWaveBufferPool.size() > 0)
		{
			pWaveBuffer = m_queWaveBufferPool.front();
			m_queWaveBufferPool.pop();
			pWaveBuffer->nDataSize = 0;
		}
		else
		{
			pWaveBuffer = new WaveBuffer();
			memset(pWaveBuffer, 0, sizeof(WaveBuffer));
			pWaveBuffer->nBufferSize = WAVE_OUT_SIZE;
			pWaveBuffer->pBuffer = new char[pWaveBuffer->nBufferSize];
			memset(pWaveBuffer->pBuffer, 0, sizeof(char)*pWaveBuffer->nBufferSize);
		}

		uint32_t nCopySize = min(pWaveBuffer->nBufferSize - pWaveBuffer->nDataSize, _nStreamSize - nStreamOffset);
		memcpy(pWaveBuffer->pBuffer + pWaveBuffer->nDataSize, _pStream+nStreamOffset, nCopySize);
		pWaveBuffer->nDataSize += nCopySize;
		nStreamOffset += nCopySize;
		m_queWaveBuffer.push(pWaveBuffer);
	}
}

bool WaveOut::FillDataToWaveHdr(WAVEHDR& wavehdr)
{
	if (m_pCurWaveBuffer == NULL)
	{
		if (m_queWaveBuffer.size() > 0)
		{
			m_pCurWaveBuffer = m_queWaveBuffer.front();
			m_queWaveBuffer.pop();
			nOffset = 0;
		}
	}
	else 
	{
		// 如果放完这个缓冲区
		if (nOffset >= m_pCurWaveBuffer->nDataSize)
		{
			m_queWaveBufferPool.push(m_pCurWaveBuffer);
			m_pCurWaveBuffer = NULL;

			if (m_queWaveBuffer.size() > 0)
			{
				m_pCurWaveBuffer = m_queWaveBuffer.front();
				m_queWaveBuffer.pop();
				nOffset = 0;
			}
		}
	}

	if(m_pCurWaveBuffer)
	{
		uint32_t bufferSize = min(m_pCurWaveBuffer->nDataSize - nOffset, uint32_t(wfx.nAvgBytesPerSec*1.5));
		wavehdr.lpData = m_pCurWaveBuffer->pBuffer + nOffset;
		wavehdr.dwBufferLength = bufferSize;
		nOffset += bufferSize;
		return true;
	}
	return false;
}

DWORD CALLBACK WaveOut::WaveCallback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	WaveOut* pWaveOut = (WaveOut*)dwInstance;
	if (pWaveOut == nullptr)
		return 0;
	switch (uMsg)
	{
	case WIM_OPEN:
		break;
	case WOM_DONE:
		if(1)
		{
			pWaveOut->m_bIsWaveOutWrite = true;
			LPWAVEHDR pWaveHeader = (LPWAVEHDR)dwParam1;
			if (pWaveOut->FillDataToWaveHdr(*pWaveHeader))
			{
				waveOutPrepareHeader(pWaveOut->hwo, pWaveHeader, sizeof(WAVEHDR));
				waveOutWrite(pWaveOut->hwo, pWaveHeader, sizeof(WAVEHDR));
			}
			else
			{
				pWaveOut->m_queWaveHdruffer.push(pWaveHeader);
			}
			pWaveOut->m_bIsWaveOutWrite = false;
		}
		break;
	case WOM_CLOSE:
		if (1)
		{
			OutputDebugString(TEXT("设备已经关闭...\n"));
			int a = 0;
		}
		break;
	default:
		break;
	}
	return 0;
}

void WaveOut::Update()
{
	while(m_queWaveHdruffer.size() > 0)
	{
		LPWAVEHDR lpWaveHdr = m_queWaveHdruffer.front(); 
		memset(lpWaveHdr, 0, sizeof(WAVEHDR));
		if (FillDataToWaveHdr(*lpWaveHdr))
		{
			waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));//准备一个波形数据块用于播放
			waveOutWrite(hwo, lpWaveHdr, sizeof(WAVEHDR));//在音频媒体中播放第二个參数指定的数据，也相当于开启一个播放区的意思
			m_queWaveHdruffer.pop();
		}
		else
		{
			break;
		}
	}
}

void GetWavInfo(char * filename, WAV_INFO * wav_info)
{
	FILE* file;
	int16_t blockAlign = 0, bitsPerSample = 0;
	int32_t bytesPerSec = 0, len = 0, data32 = 0;
	char tag[5];
	(*wav_info).szName = filename;

	/*******************打开文件读取*************************/
	fopen_s(&file, filename, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "不能要读取的打开文件\n");
	}

	fseek(file, 0, SEEK_END);
	uint32_t filelen = ftell(file);
	if (filelen <= 44)
	{
		fclose(file);
		file = nullptr;
		return;
	}

	char *bpData = new char[44];
	memset(bpData, 0, sizeof(char)*44);
	rewind(file);
	fread(bpData, 1, 44, file);
	char *bpWaveData = new char[filelen - 44];
	fread(bpWaveData, 1, filelen - 44, file);
	fclose(file);
	file = nullptr;

	/***************判断是否为有效的wav文件*******************
	*********************************************************/
	bool bIsError = false;
	bool bIsLoop = false;// 是否循环
	do
	{
		// 偏移 
		int nOffset = 0;
		// 字节
		int nSizeBytes = 0;
		// Parse "RIFF" string (chars 0-3)
		nOffset = 0;
		memset(tag, 0, sizeof(tag));
		memcpy(tag, bpData, 4);
		if (strcmp(tag, "RIFF") != 0)
		{
			bIsError = true;
			fprintf(stderr, "不是一个RIFF文件\n");
			break;
		}

		// 从下一个地址大文件尾的总字节数
		nOffset = 4;
		len = *((int32_t*)(bpData + nOffset));

		//Parse "WAVE" string len (chars 8-11)
		nOffset = 8;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		if (strcmp(tag, "WAVE") != 0)
		{
			bIsError = true;
			fprintf(stderr, "不是一个正确的wave文件\n");
			break;
		}

		//Parse "fmt" string  (chars 12-15)
		nOffset = 0x0c;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		if (strcmp(tag, "fmt ") != 0)
		{
			bIsError = true;
			fprintf(stderr, "不能在wave文件中找到格式\n");
			break;
		}
		// 过滤字节（一般为00000010H）
		nOffset = 0x10;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);

		// 格式种类（值为1时，表示数据为线性PCM编码）
		nOffset = 0x14;
		unsigned short nPCM = *(unsigned short*)(bpData + nOffset);
		(*wav_info).wFormatTag = (short)nPCM;

		// 通道数，单声道为1，双声道为2
		nOffset = 0x16;
		unsigned short nChan = *(unsigned short*)(bpData + nOffset);
		(*wav_info).nChannels = nChan;

		// 采样频率 4字节 长整型
		//Parse frequency (chars 24-27)
		nOffset = 0x18;
		uint32_t nSamplesPerSec = *(uint32_t*)(bpData + nOffset);
		wav_info->nSamplesPerSec = nSamplesPerSec;

		// 波形数据传输速率（每秒平均字节数）4字节 长整型
		//Parse bytesPerSec (chars 28-31)
		nOffset = 0x1c;
		uint32_t bytesPerSec = *(uint32_t*)(bpData + nOffset);
		(*wav_info).nAvgBytesPerSec = bytesPerSec;

		// DATA数据块长度，字节。 2字节整数
		//(chars 32--33)
		nOffset = 0x20;
		uint16_t blockAlign = *(uint16_t*)(bpData + nOffset);
		(*wav_info).nBlockAlign = blockAlign;

		// PCM位宽 2字节 整型
		//(chars 34--35)
		nOffset = 0x22;
		uint16_t BitsPerSample = *(uint16_t*)(bpData + nOffset);
		wav_info->wBitsPerSample = BitsPerSample;
		// “fact”,该部分一下是可选部分，即可能有，可能没有,一般到WAV文件由某些软件转换而成时，包含这部分。 4字节 4字符
		// size, 数值为4 4字节 长整型
		nOffset = 0x24;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		uint32_t size = *(uint32_t*)(bpData + nOffset);

		nOffset = 0x28;
		uint32_t dataSize = *(uint32_t*)(bpData + nOffset);
		if (dataSize > filelen - 44)
		{
			bIsError = true;
			fprintf(stderr, "不能在wave文件中找到格式\n");
			break;
		}
		wav_info->dataSize = dataSize;
		wav_info->data = bpWaveData;

		nOffset = 0x2c;
		int a = 0;
	} while (bIsLoop);

	if (bIsError)
	{
		delete[] bpWaveData;
	}
	delete[] bpData;
	bpData = NULL;
}
