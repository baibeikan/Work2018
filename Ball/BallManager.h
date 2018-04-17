#ifndef AFX_BALL
#define AFX_BALL



class BallManager
{
public:
	BallManager(map<int,HDC> SrcMap,HDC *pBackDc,int TableW,int TableH);
	void Init();
	void Update(float FrameTime);
	void Render(int x,int y);
	~BallManager(void);
public:
	void SetOriginGlobePos(Vector CursorPos);
	void SetOriginGlobeSpend(float Speed);
	void SetCursorPos(Vector const&CursorPos);
	void SetShowCursor(bool IsShow = false);

	bool IsStop();

private:
	void _CrashFrame(int i,float speed);

	void _CheckCrash(float time);

	void _Crash(float time);

private:
	HDC		*m_pBackDc;
	CImage m_Image;
	int m_Height;
	int m_Width;
	BALL m_Ball[16];
	map<int,HDC> m_SrcMap; 
	map<int,HDC>::iterator m_SrcPt;

	float	m_FrameLeft;
	float	m_FrameTop;
	float	m_FrameRight;
	float	m_FrameBottom;
	RECT  m_MapRect;

	float	m_Fps;

	HDC		m_MapDc;
	HBITMAP	m_MapBit;
	HBRUSH	m_MapBrush;
	Vector	m_CusorPos;
	bool	m_IsShowCursor;
	
	int		m_i;
	int		m_j;
};

#endif
