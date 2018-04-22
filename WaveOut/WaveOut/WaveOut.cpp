// WaveOut.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <comdef.h>
#pragma comment(lib, "winmm.lib")

#define DATASIZE (1024*512) //�ִν�ȡ���ݴ�С
FILE*			pcmfile;  //��Ƶ�ļ�
HWAVEOUT        hwo;

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;


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
	byte *data;		//��Ų������ݣ�

}WAV_INFO;


void GET_WAV_INFO(char * filename, WAV_INFO * wav_info)
{
	memset(wav_info, 0, sizeof(WAV_INFO));
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

	byte *bpData = new byte[44];
	memset(bpData, 0, sizeof(byte) * 44);
	rewind(file);
	fread(bpData, 1, 44, file);
	byte *bpWaveData = new byte[filelen - 44];
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
		//memcpy(wav_info->data, bpData + nOffset, wav_info->dataSize);
		int a = 0;
	} while (bIsLoop);

	if (bIsError)
	{
		delete[] bpWaveData;
	}
	delete[] bpData;
	bpData = NULL;
}

WAV_INFO wavInfo;
int nOffset = 0;

void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD dwInstance, DWORD dw1, DWORD dw2)//�ص�����
{
	switch (uMsg)
	{
		case WOM_DONE://�ϴλ��沥�����,�������¼�
		{
			LPWAVEHDR pWaveHeader = (LPWAVEHDR)dw1;
			printf("LPWAVEHDR dwFlags=0x%x WHDR_DONE=%d WHDR_PREPARED=%d WHDR_BEGINLOOP=%d WHDR_ENDLOOP=%d WHDR_INQUEUE=%d\n",
				pWaveHeader->dwFlags, bool(pWaveHeader->dwFlags&WHDR_DONE), bool(pWaveHeader->dwFlags&WHDR_PREPARED), bool(pWaveHeader->dwFlags&WHDR_BEGINLOOP),
				bool(pWaveHeader->dwFlags&WHDR_ENDLOOP), bool(pWaveHeader->dwFlags&WHDR_INQUEUE));

			char* data = (char*)wavInfo.data;
			uint32_t dataSize = wavInfo.dataSize;
			int cpySize = min(dataSize - nOffset, wavInfo.nAvgBytesPerSec);
			if (cpySize > 0)
			{
				pWaveHeader->dwBufferLength = cpySize;
				pWaveHeader->lpData = data + nOffset;
				nOffset += cpySize;
				waveOutPrepareHeader(hwo, pWaveHeader, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
				waveOutWrite(hwo, pWaveHeader, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
			}

			printf("LPWAVEHDR dwFlags=0x%x WHDR_DONE=%d WHDR_PREPARED=%d WHDR_BEGINLOOP=%d WHDR_ENDLOOP=%d WHDR_INQUEUE=%d\n",
				pWaveHeader->dwFlags, bool(pWaveHeader->dwFlags&WHDR_DONE), bool(pWaveHeader->dwFlags&WHDR_PREPARED), bool(pWaveHeader->dwFlags&WHDR_BEGINLOOP),
				bool(pWaveHeader->dwFlags&WHDR_ENDLOOP), bool(pWaveHeader->dwFlags&WHDR_INQUEUE));
		}
		break;
	}
}

void main()
{
	WAVEHDR         wh1;
	WAVEHDR         wh2;
	WAVEFORMATEX    wfx;

	GET_WAV_INFO("22.wav", &wavInfo);
	printf("��������%d\n", wavInfo.wFormatTag);
	printf("��������%d\n", wavInfo.nChannels);
	printf("����Ƶ��%d\n", wavInfo.nSamplesPerSec);
	printf("ƽ������������%d\n", wavInfo.nAvgBytesPerSec);
	printf("��%d\n", wavInfo.wBitsPerSample);
	printf("DATASIZE=%d\n", DATASIZE);

	char* data = (char*)wavInfo.data;
	uint32_t dataSize = wavInfo.dataSize;

	float time = (float)wavInfo.dataSize / wavInfo.nAvgBytesPerSec;
	printf("ʱ��%f\n", time);
//	return;

	//fopen_s(&pcmfile, "paomo.pcm", "rb");//���ļ�

	wfx.wFormatTag = wavInfo.wFormatTag; // WAVE_FORMAT_PCM;//���ò��������ĸ�ʽ
	wfx.nChannels = wavInfo.nChannels;//������Ƶ�ļ���ͨ������
	wfx.nSamplesPerSec = wavInfo.nSamplesPerSec;//����ÿһ���������źͼ�¼ʱ������Ƶ��
	//���ֵ���ڴ��������С�Ƿǳ�ʵ�õ�
	wfx.nAvgBytesPerSec = wavInfo.nAvgBytesPerSec;//���������ƽ������������,��λbyte/s��
	wfx.nBlockAlign = wavInfo.nBlockAlign;//���ֽ�Ϊ��λ���ÿ����
	wfx.wBitsPerSample = wavInfo.wBitsPerSample; // ����ÿ��������λ��,��Ϊ8��16
	wfx.cbSize = 0;//������Ϣ�Ĵ�С
	waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, 0L, CALLBACK_FUNCTION);//��һ�������Ĳ�����Ƶ���װ���������������ţ���ʽΪ�ص�������ʽ�������ǶԻ�������ܹ��������������Ϊ(DWORD)this����������Demo��������

	wh1.dwLoops = 0L;//������һ
	wh1.dwUser = 0;
	wh1.dwBufferLength = DATASIZE;
	wh1.lpData = NULL;
	wh1.dwFlags = 0L;
	int cpySize = min(dataSize - nOffset, wavInfo.nAvgBytesPerSec);
	if (cpySize > 0)
	{
		wh1.dwBufferLength = cpySize;
		wh1.lpData = data+nOffset;
		nOffset += cpySize;
		waveOutPrepareHeader(hwo, &wh1, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
		waveOutWrite(hwo, &wh1, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
	}


	wh2.dwLoops = 0L;//��������������ͬ��
	wh2.lpData = NULL;
	wh2.dwBufferLength = DATASIZE;
	wh2.dwUser = 1;
	wh2.dwFlags = 0L;
	cpySize = min(dataSize - nOffset, wavInfo.nAvgBytesPerSec);
	if (cpySize > 0)
	{
		wh2.dwBufferLength = cpySize;
		wh2.lpData = data + nOffset;
		nOffset += cpySize;
		waveOutPrepareHeader(hwo, &wh2, sizeof(WAVEHDR));//׼��һ���������ݿ����ڲ���
		waveOutWrite(hwo, &wh2, sizeof(WAVEHDR));//����Ƶý���в��ŵڶ�������ָ�������ݣ�Ҳ�൱�ڿ���һ������������˼
	}

	TCHAR szTempBuffer[512] = {};
	while ((wh1.dwFlags&WHDR_DONE) != WHDR_DONE || (wh2.dwFlags&WHDR_DONE) != WHDR_DONE)
	{
		memset(szTempBuffer, 0, sizeof(szTempBuffer));
		printf("dwFlags=0x%x WHDR_DONE=%d WHDR_PREPARED=%d WHDR_BEGINLOOP=%d WHDR_ENDLOOP=%d WHDR_INQUEUE=%d\n",
			wh1.dwFlags, bool(wh1.dwFlags&WHDR_DONE), bool(wh1.dwFlags&WHDR_PREPARED), bool(wh1.dwFlags&WHDR_BEGINLOOP),
			bool(wh1.dwFlags&WHDR_ENDLOOP), bool(wh1.dwFlags&WHDR_INQUEUE));

		printf("dwFlags=0x%x WHDR_DONE=%d WHDR_PREPARED=%d WHDR_BEGINLOOP=%d WHDR_ENDLOOP=%d WHDR_INQUEUE=%d\n",
			wh2.dwFlags, bool(wh2.dwFlags&WHDR_DONE), bool(wh2.dwFlags&WHDR_PREPARED), bool(wh2.dwFlags&WHDR_BEGINLOOP),
			bool(wh2.dwFlags&WHDR_ENDLOOP), bool(wh2.dwFlags&WHDR_INQUEUE));

		float time = (float)(wavInfo.dataSize-nOffset) / wavInfo.nAvgBytesPerSec;
		printf("ʣ��ʱ��%f\n", time);

		if (wavInfo.dataSize / 10< nOffset)
			break;

		Sleep(1000);
	}

	waveOutPause(hwo);
	waveOutUnprepareHeader(hwo, &wh1, sizeof(WAVEHDR));//��������
	waveOutUnprepareHeader(hwo, &wh2, sizeof(WAVEHDR));
	waveOutClose(hwo);

	delete[] wavInfo.data;
	getchar();
	return;
}
