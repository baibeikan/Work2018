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
	waveformat.wFormatTag = wav_info.wFormatTag; // WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	waveformat.nChannels = wav_info.nChannels;//������Ƶ�ļ���ͨ������
	waveformat.nSamplesPerSec = wav_info.nSamplesPerSec;//����ÿһ���������źͼ�¼ʱ������Ƶ��
	waveformat.nBlockAlign = wav_info.nBlockAlign;//���ֽ�Ϊ��λ���ÿ����
	waveformat.wBitsPerSample = wav_info.wBitsPerSample; // ����ÿ��������λ��,��Ϊ8��16
	waveformat.nAvgBytesPerSec = wav_info.nAvgBytesPerSec;//���������ƽ������������,��λbyte/s��
	waveformat.cbSize = 0;//������Ϣ�Ĵ�С

	if (hwo == NULL)
	{
		memcpy(&wfx, &waveformat, sizeof(WAVEFORMATEX));
		MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);//��һ�������Ĳ�����Ƶ���װ���������������ţ���ʽΪ�ص�������ʽ�������ǶԻ�������ܹ��������������Ϊ(DWORD)this����������Demo��������
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
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//��������
			m_queWaveHdruffer.push(&hdr);
		}


		if (memcmp(&waveformat, &wfx, sizeof(WAVEFORMATEX)) == 0)
		{
			//���Ÿ�ʽ����ǰһ��
			OutputDebugString(L"�������´���\n");
		}
		else
		{
			OutputDebugString(L"���´���\n");

			waveOutPause(hwo);
			waveOutClose(hwo);
			hwo = NULL;
			// ���´���
			memcpy(&wfx, &waveformat, sizeof(WAVEFORMATEX));
			//��һ�������Ĳ�����Ƶ���װ���������������ţ���ʽΪ�ص�������ʽ�������ǶԻ�������ܹ��������������Ϊ(DWORD)this����������Demo��������
			MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
		}
	}

	while(m_queWaveHdruffer.size() > 0)
	{
		LPWAVEHDR lpWaveHdr = m_queWaveHdruffer.front(); 
		memset(lpWaveHdr, 0, sizeof(WAVEHDR));
		if (FillDataToWaveHdr(*lpWaveHdr))
		{
			waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
			waveOutWrite(hwo, lpWaveHdr, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
			m_queWaveHdruffer.pop();
		}
		else
		{
			break;
		}
	}
	waveOutRestart(hwo);

	OutputDebugString(L"��ǰ��ʽ\n");
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
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//��������
		}
		if (memcmp(&_wavformat, &wfx, sizeof(WAVEFORMATEX)) == 0)
		{
			//���Ÿ�ʽ����ǰһ��
			OutputDebugString(L"�������´���\n");
		}
		else
		{
			waveOutClose(hwo);
			hwo = NULL;
			OutputDebugString(L"���´���\n");
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
		MMRESULT mmresult = waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);//��һ�������Ĳ�����Ƶ���װ���������������ţ���ʽΪ�ص�������ʽ�������ǶԻ�������ܹ��������������Ϊ(DWORD)this����������Demo��������
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
			waveOutUnprepareHeader(hwo, &hdr, sizeof(WAVEHDR));//��������
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
		// ����������������
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
			OutputDebugString(TEXT("�豸�Ѿ��ر�...\n"));
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
			waveOutPrepareHeader(hwo, lpWaveHdr, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
			waveOutWrite(hwo, lpWaveHdr, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
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

	/*******************���ļ���ȡ*************************/
	fopen_s(&file, filename, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "����Ҫ��ȡ�Ĵ��ļ�\n");
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

	/***************�ж��Ƿ�Ϊ��Ч��wav�ļ�*******************
	*********************************************************/
	bool bIsError = false;
	bool bIsLoop = false;// �Ƿ�ѭ��
	do
	{
		// ƫ�� 
		int nOffset = 0;
		// �ֽ�
		int nSizeBytes = 0;
		// Parse "RIFF" string (chars 0-3)
		nOffset = 0;
		memset(tag, 0, sizeof(tag));
		memcpy(tag, bpData, 4);
		if (strcmp(tag, "RIFF") != 0)
		{
			bIsError = true;
			fprintf(stderr, "����һ��RIFF�ļ�\n");
			break;
		}

		// ����һ����ַ���ļ�β�����ֽ���
		nOffset = 4;
		len = *((int32_t*)(bpData + nOffset));

		//Parse "WAVE" string len (chars 8-11)
		nOffset = 8;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		if (strcmp(tag, "WAVE") != 0)
		{
			bIsError = true;
			fprintf(stderr, "����һ����ȷ��wave�ļ�\n");
			break;
		}

		//Parse "fmt" string  (chars 12-15)
		nOffset = 0x0c;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		if (strcmp(tag, "fmt ") != 0)
		{
			bIsError = true;
			fprintf(stderr, "������wave�ļ����ҵ���ʽ\n");
			break;
		}
		// �����ֽڣ�һ��Ϊ00000010H��
		nOffset = 0x10;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);

		// ��ʽ���ֵࣨΪ1ʱ����ʾ����Ϊ����PCM���룩
		nOffset = 0x14;
		unsigned short nPCM = *(unsigned short*)(bpData + nOffset);
		(*wav_info).wFormatTag = (short)nPCM;

		// ͨ������������Ϊ1��˫����Ϊ2
		nOffset = 0x16;
		unsigned short nChan = *(unsigned short*)(bpData + nOffset);
		(*wav_info).nChannels = nChan;

		// ����Ƶ�� 4�ֽ� ������
		//Parse frequency (chars 24-27)
		nOffset = 0x18;
		uint32_t nSamplesPerSec = *(uint32_t*)(bpData + nOffset);
		wav_info->nSamplesPerSec = nSamplesPerSec;

		// �������ݴ������ʣ�ÿ��ƽ���ֽ�����4�ֽ� ������
		//Parse bytesPerSec (chars 28-31)
		nOffset = 0x1c;
		uint32_t bytesPerSec = *(uint32_t*)(bpData + nOffset);
		(*wav_info).nAvgBytesPerSec = bytesPerSec;

		// DATA���ݿ鳤�ȣ��ֽڡ� 2�ֽ�����
		//(chars 32--33)
		nOffset = 0x20;
		uint16_t blockAlign = *(uint16_t*)(bpData + nOffset);
		(*wav_info).nBlockAlign = blockAlign;

		// PCMλ�� 2�ֽ� ����
		//(chars 34--35)
		nOffset = 0x22;
		uint16_t BitsPerSample = *(uint16_t*)(bpData + nOffset);
		wav_info->wBitsPerSample = BitsPerSample;
		// ��fact��,�ò���һ���ǿ�ѡ���֣��������У�����û��,һ�㵽WAV�ļ���ĳЩ���ת������ʱ�������ⲿ�֡� 4�ֽ� 4�ַ�
		// size, ��ֵΪ4 4�ֽ� ������
		nOffset = 0x24;
		nSizeBytes = 4;
		memcpy(tag, bpData + nOffset, nSizeBytes);
		uint32_t size = *(uint32_t*)(bpData + nOffset);

		nOffset = 0x28;
		uint32_t dataSize = *(uint32_t*)(bpData + nOffset);
		if (dataSize > filelen - 44)
		{
			bIsError = true;
			fprintf(stderr, "������wave�ļ����ҵ���ʽ\n");
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
