#include "StdAfx.h"
#include ".\manage.h"

Manage::Manage( HDC pMainDc,RECT ClientRect )
{
	m_ClientRect = ClientRect;
	m_pBackDc = CreateCompatibleDC(NULL);
	m_pBitmap = CreateCompatibleBitmap(pMainDc,ClientRect.right,ClientRect.bottom);
	SelectObject(m_pBackDc,m_pBitmap);
	m_pClientBrush = CreateSolidBrush(RGB(0,0,0));
	m_pBall1 = NULL;
	m_pBall2 = NULL;
	m_pSrc = NULL;
	m_Spend = 0.0f;
	m_FrameTime = 0.0f;
}

Manage::~Manage(void)
{
	SAFE_DELETE(m_pSrc);
	SAFE_DELETE(m_pBall2);
	SAFE_DELETE(m_pBall1);
	SAFE_DELOBJ(m_pClientBrush);
	SAFE_DELOBJ(m_pBitmap);
	SAFE_DELDC(m_pBackDc);
}

void Manage::Init()
{
	m_pSrc = new SrcBitmap();
	m_pSrc->LoadBitmap();
	m_pSrc->LoadBkBitmap();

	m_pBall1 = new BallManager(m_pSrc->GetMap(),&m_pBackDc,m_ClientRect.right-m_ClientRect.left,m_ClientRect.bottom-m_ClientRect.top);
	m_pBall1->Init();
	//m_pBall1->Load("2.bmp",50,50);
	//m_pBall2 = new MyBall(&m_pBackDc);
	//m_pBall2->Load("1.bmp",50,50);
}

void Manage::Update()
{
	static bool IsDown = false;
	//IsDown = false;

	m_FrameTime = GetFrameTime();
	if (GetKeyDown(VK_LBUTTON))
	{
		IsDown = true;
		m_pBall1->SetShowCursor(true);
	}

	if (GetKeyUp(VK_LBUTTON))
	{
		IsDown = false;
		m_pBall1->SetShowCursor(false);
	}
	static Vector ClientCursorPos;
	GetClientCursorPos(ClientCursorPos);

	if (IsDown)
	{
		if (GetKeyState(VK_LBUTTON))
		{
			if(m_pBall1->IsStop())
			{
				m_pBall1->SetCursorPos(ClientCursorPos);
			}
		}
	}
	m_pBall1->Update(m_FrameTime);
}

void Manage::Render( HDC pMainDc )
{
	FillRect(m_pBackDc,&m_ClientRect,m_pClientBrush);
	////////////////////////////////////////////////////////////////////////////
	//m_pBall2->Render(0,0);

	BitBlt(m_pBackDc,0,0,m_ClientRect.right,m_ClientRect.bottom,m_pSrc->GetBkhdc(),0,0,SRCCOPY);
	
	m_pBall1->Render(0,0);
	//m_pSrc->Render(m_pBackDc);
	GetCursorPos(&m_CurPos);
	//char Filena[128] = {};

	//sprintf(Filena,"%f",m_FrameTime);
	//TextOut(m_pBackDc,0,0,Filena,int(strlen(Filena)));
	//sprintf(Filena,"ËÙ¶È:%f      ",m_Spend);
	//TextOut(m_pBackDc,0,20,Filena,int(strlen(Filena)));

	//////////////////////////////////////////////////////////////////////////
	BitBlt(pMainDc,0,0,m_ClientRect.right,m_ClientRect.bottom,m_pBackDc,0,0,SRCCOPY);

}