// WaveOut.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <comdef.h>
#pragma comment(lib, "winmm.lib")

#define DATASIZE (1024*512) //分次截取数据大小
FILE*			pcmfile;  //音频文件
HWAVEOUT        hwo;

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;


typedef struct
{
	char *szName;     // wav文件名
	short wFormatTag;				//编码类型标示PCM(0001)或 ADPCM(0002)	2字节
	short nChannels;			//声道数	2字节
	unsigned long nAvgBytesPerSec;	//每秒记录数据量字节数	4字节
	short	nBlockAlign;			//采样点占字节大小	2字节 wBitsPerSample*nChannels/8
	short  wBitsPerSample;			//采样位数	2字节 8表示单通道 16双通道
	long nSamplesPerSec;    // 总共的样本点数 每秒采样率
	uint32_t dataSize;	//"DATA"块大小	4字节
	byte *data;		//存放采样数据；

}WAV_INFO;


void GET_WAV_INFO(char * filename, WAV_INFO * wav_info)
{
	memset(wav_info, 0, sizeof(WAV_INFO));
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

	byte *bpData = new byte[44];
	memset(bpData, 0, sizeof(byte) * 44);
	rewind(file);
	fread(bpData, 1, 44, file);
	byte *bpWaveData = new byte[filelen - 44];
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

void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD dwInstance, DWORD dw1, DWORD dw2)//回调函数
{
	switch (uMsg)
	{
		case WOM_DONE://上次缓存播放完毕,触发该事件
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
				waveOutPrepareHeader(hwo, pWaveHeader, sizeof(WAVEHDR));//准备一个波形数据块用于播放
				waveOutWrite(hwo, pWaveHeader, sizeof(WAVEHDR));//在音频媒体中播放第二个參数指定的数据，也相当于开启一个播放区的意思
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
	printf("波形声音%d\n", wavInfo.wFormatTag);
	printf("波形声音%d\n", wavInfo.nChannels);
	printf("样本频率%d\n", wavInfo.nSamplesPerSec);
	printf("平均传输数据率%d\n", wavInfo.nAvgBytesPerSec);
	printf("数%d\n", wavInfo.wBitsPerSample);
	printf("DATASIZE=%d\n", DATASIZE);

	char* data = (char*)wavInfo.data;
	uint32_t dataSize = wavInfo.dataSize;

	float time = (float)wavInfo.dataSize / wavInfo.nAvgBytesPerSec;
	printf("时间%f\n", time);
//	return;

	//fopen_s(&pcmfile, "paomo.pcm", "rb");//打开文件

	wfx.wFormatTag = wavInfo.wFormatTag; // WAVE_FORMAT_PCM;//设置波形声音的格式
	wfx.nChannels = wavInfo.nChannels;//设置音频文件的通道数量
	wfx.nSamplesPerSec = wavInfo.nSamplesPerSec;//设置每一个声道播放和记录时的样本频率
	//这个值对于创建缓冲大小是非常实用的
	wfx.nAvgBytesPerSec = wavInfo.nAvgBytesPerSec;//设置请求的平均传输数据率,单位byte/s。
	wfx.nBlockAlign = wavInfo.nBlockAlign;//以字节为单位设置块对齐
	wfx.wBitsPerSample = wavInfo.wBitsPerSample; // 设置每个样本的位深,多为8或16
	wfx.cbSize = 0;//额外信息的大小
	waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD)WaveCallback, 0L, CALLBACK_FUNCTION);//打开一个给定的波形音频输出装置来进行声音播放，方式为回调函数方式。假设是对话框程序，能够将第五个參数改为(DWORD)this。操作跟本Demo程序类似

	wh1.dwLoops = 0L;//播放区一
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
		waveOutPrepareHeader(hwo, &wh1, sizeof(WAVEHDR));//准备一个波形数据块用于播放
		waveOutWrite(hwo, &wh1, sizeof(WAVEHDR));//在音频媒体中播放第二个參数指定的数据，也相当于开启一个播放区的意思
	}


	wh2.dwLoops = 0L;//播放区二，基本同上
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
		waveOutPrepareHeader(hwo, &wh2, sizeof(WAVEHDR));//准备一个波形数据块用于播放
		waveOutWrite(hwo, &wh2, sizeof(WAVEHDR));//在音频媒体中播放第二个參数指定的数据，也相当于开启一个播放区的意思
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
		printf("剩余时间%f\n", time);

		if (wavInfo.dataSize / 10< nOffset)
			break;

		Sleep(1000);
	}

	waveOutPause(hwo);
	waveOutUnprepareHeader(hwo, &wh1, sizeof(WAVEHDR));//清理数据
	waveOutUnprepareHeader(hwo, &wh2, sizeof(WAVEHDR));
	waveOutClose(hwo);

	delete[] wavInfo.data;
	getchar();
	return;
}
