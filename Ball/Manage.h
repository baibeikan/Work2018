#ifndef AFX_MANAGE
#define AFX_MANAGE

class Manage
{
public:
	Manage(HDC pMainDc,RECT ClientRect);
	~Manage(void);
	void Init();
	void Update(float _deltaTime);
	void Render(HDC pMainDc);
private:
	RECT	m_ClientRect;
	HDC		m_pBackDc;
	HBITMAP	m_pBitmap;
	HBRUSH m_pClientBrush;
private:
	BallManager	*m_pBall1;
	BallManager	*m_pBall2;
	SrcBitmap	*m_pSrc;
	float		m_Spend;
	POINT		m_CurPos;
	
	float		m_FrameTime;
	HWND		m_hWnd;
};
#endif
