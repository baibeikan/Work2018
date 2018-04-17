#include "StdAfx.h"
#include ".\srcbitmap.h"

SrcBitmap::SrcBitmap(void)
{
	m_SrcBitmap.clear();
	m_BkDc = CreateCompatibleDC(NULL);
	m_BkBitmap = NULL;
}

SrcBitmap::~SrcBitmap(void)
{
	SAFE_DELOBJ(m_BkBitmap);
	SAFE_DELDC(m_BkDc);
	for (m_SrcPt = m_SrcBitmap.begin();
		m_SrcPt != m_SrcBitmap.end();
		++m_SrcPt)
	{
		HDC hdc = ((*m_SrcPt).second);
		SAFE_DELDC(hdc);
	}
	m_SrcBitmap.clear();

}

void SrcBitmap::LoadBitmap()
{
	char FileName[128];
	HDC hdc = NULL;
	HBITMAP SrcBitmap = NULL;
	for (int i = 0; i < BALL_NUM; ++i)
	{
		sprintf(FileName,"Src\\%d.bmp",i);
		hdc = CreateCompatibleDC(NULL);
		SrcBitmap = HBITMAP(LoadImage(NULL,FileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE));
		SelectObject(hdc,SrcBitmap);
		m_SrcBitmap[i] = hdc;
		SAFE_DELOBJ(SrcBitmap);
	}
}

const map<int,HDC> SrcBitmap::GetMap()
{
	return m_SrcBitmap;
}

void SrcBitmap::LoadBkBitmap()
{
	SAFE_DELOBJ(m_BkBitmap);
	m_BkBitmap = HBITMAP(LoadImage(NULL,"Src\\TableBk.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE));
	SelectObject(m_BkDc,m_BkBitmap);
}

HDC SrcBitmap::GetBkhdc() const
{
	return m_BkDc;
}