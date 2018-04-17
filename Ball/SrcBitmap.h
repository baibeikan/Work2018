#ifndef AFX_SRCBITMAP
#define AFX_SRCBITMAP
#define BALL_NUM 16
#define FrictionCoefficient (9.8f * 5.0f) //Ä¦²ÁÏµÊý
#define FloatZero (0.00001f)

#include <map>
using namespace std;
class SrcBitmap
{
public:
	SrcBitmap();
	void LoadBitmap();
	void LoadBkBitmap();
	~SrcBitmap(void);
public:
	const map<int,HDC> GetMap();
	HDC GetBkhdc() const;
private:
	map<int,HDC> m_SrcBitmap;
	map<int,HDC>::iterator m_SrcPt;

	HDC m_BkDc;
	HBITMAP	m_BkBitmap;
};
#endif
