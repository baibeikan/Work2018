#include "StdAfx.h"
#include ".\BallManager.h"
#define FLOATZERO 0.0f
#define UMG (9.8f * 2)

int CalculationTwoElementEquation(float A, float B, float C, float& x0, float& x1)
{
	float D = B * B - 4 * A * C;//一定大于|B|, A + C -
	if(D < -FloatZero)//没有根
		return 0;

	if(D < FloatZero)//有1个根
	{
		x0 = 0.5f * (-B) / A;
		x1 = x0;

		return 1;
	}

	float sqrtfD =  sqrtf(D);
	x0 = 0.5f * (-B + sqrtfD) / A;
	x1 = 0.5f * (-B - sqrtfD) / A;
	return 2;
}

//默认srcBall是移动球，desBall是静止球
bool OneBallMove(const BALL& srcBall, const BALL& desBall, const float tickTime, float& crashTime)
{
	return false;
}

bool CheckCrash(BALL& srcBall, BALL& desBall, const float tickTime, float& crashTime)
{
	return false;
}

bool BallCrash(BALL* pSrcBall, BALL* pDesBall)
{
	if(pSrcBall == NULL || pDesBall == NULL)
	{
		char szLog[256] = {};
		sprintf_s(szLog, "=====Error:BallCrash::pSrcBall Or pDesBall Is NULL=====\n");
		OutputDebugString(szLog);
		return false;
	}

	//法线方向不是单位向量
	Vector NormalDir = pSrcBall->vPos - pDesBall->vPos;

	float DisSquare = NormalDir.mx * NormalDir.mx + NormalDir.my * NormalDir.my;
	float DiaSquare = BallDia * BallDia;
	if(DisSquare - DiaSquare > 0.5f || DiaSquare - DisSquare < -0.5f)
	{
		char szLog[256] = {};
		sprintf_s(szLog, "=====Error:BallCrash::pSrcBall And pDesBall Dis=%f Is Lagger BillDia=%f=====\n", DisSquare, DiaSquare);
		OutputDebugString(szLog);
		return false;
	}

	//切线方向不是单位向量
	//x = cos@;y=sin@
	//xT=cos(@+90) = cos@cos90 - sin@sin90 = -sin@
	//yT=sin(@+90) = sin@cos90 + cos@sin90 = cos@
	Vector TangentDir = Vector(-NormalDir.my, NormalDir.mx);

	//连线的方向速度为交换，切线的方向速度不变
	Vector SrcSpeed = pSrcBall->dir * pSrcBall->speed;
	Vector DesSpeed = pDesBall->dir * pDesBall->speed;

	float SrcNormal = NormalDir * SrcSpeed;
	float SrcTangent = TangentDir * SrcSpeed;
	float DesNormal = NormalDir * DesSpeed;
	float DesTangent = TangentDir * DesSpeed;
	float DisSquareReciprocal = 1.0f / DisSquare;

	SrcSpeed = (NormalDir * DesNormal + TangentDir * SrcTangent) * DisSquareReciprocal;
	DesSpeed = (NormalDir * SrcNormal + TangentDir * DesTangent) * DisSquareReciprocal;
	pSrcBall->speed = SrcSpeed.VectorMod();
	pSrcBall->dir = SrcSpeed.VectorNormal();

	pDesBall->speed = DesSpeed.VectorMod();
	pDesBall->dir = DesSpeed.VectorNormal();

	return true;
}


//检测碰壁Ax+By+C=0;
bool CheckCrashWall(const BALL& srcBall, const Vector& pos, const Vector& dir, const float tickTime, float& crashTime)
{
	if(srcBall.speed < FloatZero) return false;

	////切线
	//Vector TangentDir = dir;

	//法线
	Vector NormalDir = Vector(-dir.my, dir.mx);
	if(NormalDir * srcBall.dir > FloatZero)
		return false;

	float a, b, c;
	a = dir.my;
	b = -dir.mx;
	c = dir.mx * pos.my - dir.my * pos.mx;

	// 	float x0 = srcBall.pos.mx + srcBall.dir.mx * s;
	// 	float y0 = srcBall.pos.my + srcBall.dir.my * s;
	// 	a * x0 + b * y0 + c = 0;
	// 	a * (srcBall.pos.mx + srcBall.dir.mx * s) + b * (srcBall.pos.my + srcBall.dir.my * s) + c = 0;
	// 	(srcBall.dir.mx * A + srcBall.dir.my * B) * s = -(C + srcBall.pos.mx * A + srcBall.dir.my * B);
	float Divisor = -(c + srcBall.vPos.mx * a + srcBall.vPos.my * b);//除数
	float Dividend = (srcBall.dir.mx * a + srcBall.dir.my * b);//被除数
	if (Dividend < FloatZero  && Dividend > -FloatZero)
	{
		//char szLog[256] = {};
		//sprintf(szLog, "=====CheckCrashWall::Divisor=%f,Dividend=%f=====\n", Divisor, Dividend);
		//OutputDebugString(szLog);
		return false;
	}

	float S = Divisor / Dividend;
	if (S < FloatZero)
	{
		//char szLog[256] = {};
		//sprintf(szLog, "=====CheckCrashWall::Divisor=%f,Dividend=%f,S=%f=====\n", Divisor, Dividend, S);
		//OutputDebugString(szLog);
		return false;
	}

	float moveTime = srcBall.speed / FrictionCoefficient;
	if(moveTime > tickTime)
	{
		float tickS = srcBall.speed * tickTime - 0.5f * FrictionCoefficient * tickTime * tickTime;
		if(tickS < S)
		{
			//char szLog[256] = {};
			//sprintf(szLog, "=====CheckCrashWall::tickS=%f,S=%f=====\n", tickS, S);
			//OutputDebugString(szLog);
			return false;
		}

		if(tickS == S)
		{
			crashTime = tickTime;
			return true;
		}
	}

	//求出来的S就是距离
	//float t = srcBall.speed / FrictionCoefficient;
	//srcBall.speed * t - 0.5f * FrictionCoefficient*t^2=srcBall.speed*srcBall.speed/FrictionCoefficient-0.5f*FrictionCoefficient*srcBall.speed^2;
	//=0.5f*srcBall.speed^2/FrictionCoefficient;
	float endS = srcBall.speed * srcBall.speed * 0.5f / FrictionCoefficient;
	if(endS < S)//到不了焦点处
	{
		//char szLog[256] = {};
		//sprintf(szLog, "=====CheckCrashWall到不了焦点处::endS=%f,S=%f=====\n", endS, S);
		//OutputDebugString(szLog);
		return false;
	}

	//0.5f*FrictionCoefficient*t^2- srcBall.speed*t  + S = 0; 
	a = 0.5f * FrictionCoefficient;
	b = -srcBall.speed;
	c = S;

	//两根之和-b/a,两根之积c/a;[C < 0]一正一负
	float x0, x1;
	switch(CalculationTwoElementEquation(a, b, c, x0, x1))
	{
	case 0:
		return false;
	case 1:
		{
			if(x0 < FloatZero) return false;//这种情况应该不会发生

			crashTime = x0;
		}
	case 2:
		{
			//很明显C>0所以不可能一正一负，-b>0所以两根都是正的
			if(x0 < 0.0f) return false;//这种情况应该不会发生

			crashTime = x0 < x1 ? x0 : x1;//[取最小的]
		}
		break;
	}
	return true;
}

bool CrashWall(BALL* pSrcBall, const Vector& pos, const Vector& dir)
{
	if(pSrcBall->speed <= 0.0f)
		return false;

	float a, b, c;
	a = dir.my;
	b = -dir.mx;
	c = dir.mx * pos.my - dir.my * pos.mx;

	float root = pSrcBall->vPos.mx * a + pSrcBall->vPos.my * b + c;
	if(root > 0.01f || root < -0.01f)
	{
		return false;
	}

	Vector TangentDir = dir;
	Vector NormalDir = Vector(-dir.my, dir.mx);

	Vector SrcSpeed = pSrcBall->dir * pSrcBall->speed;
	float SrcNormal = NormalDir * SrcSpeed;
	float SrcTangent = TangentDir * SrcSpeed;
	float DisSquareReciprocal = 1.0f / (TangentDir * TangentDir);

	SrcSpeed = (TangentDir * SrcTangent - NormalDir * SrcNormal) * DisSquareReciprocal;
	pSrcBall->speed = SrcSpeed.VectorMod();
	pSrcBall->dir = SrcSpeed.VectorNormal();
	//pSrcBall->dir = Vector(-pSrcBall->dir.mx, -pSrcBall->dir.my);
	return true;
}

BallManager::BallManager(map<int,HDC> SrcMap,HDC *pBackDc,int TableW,int TableH)
{
	m_pBackDc = pBackDc;
	m_Height = 0;
	m_Width = 0;
	memset(m_Ball, 0, sizeof(m_Ball));
	m_SrcMap = SrcMap;
	m_FrameLeft=BallRadii;
	m_FrameTop =BallRadii;
	m_FrameRight=TableW-BallRadii;
	m_FrameBottom=TableH-BallRadii;
	SetRect(&m_MapRect,0,0,TableW,TableH);

	m_Fps = 0.01f;

	m_MapDc = CreateCompatibleDC(NULL);
	m_MapBit = CreateCompatibleBitmap(*pBackDc,TableW,TableH);
	SelectObject(m_MapDc,m_MapBit);
	m_MapBrush = CreateSolidBrush(RGB(125,125,125));
	m_IsShowCursor = false;

	m_i = 0;
	m_j = 0;

}

BallManager::~BallManager(void)
{
	SAFE_DELOBJ(m_MapBrush);
	SAFE_DELOBJ(m_MapBit);
	SAFE_DELDC(m_MapDc);
	m_pBackDc = NULL;
}

void BallManager::Init()
{	
	float Cos = cosf(M_PI/6);
	float Sin = sinf(M_PI/6);
	float D = BallDia; 

	// 初始化求的位置
	m_Ball[0].vPos(m_FrameRight/5,m_FrameBottom/2);

	m_Ball[1].vPos(m_FrameRight*3/5,m_FrameBottom/2);

	m_Ball[2].vPos(m_Ball[1].vPos.mx + D*Cos,m_Ball[1].vPos.my + D*Sin);
	m_Ball[3].vPos(m_Ball[2].vPos.mx,m_Ball[2].vPos.my-D);

	m_Ball[4].vPos(m_Ball[1].vPos.mx + 2*D*Cos,m_Ball[1].vPos.my+ 2*D*Sin);
	m_Ball[5].vPos(m_Ball[4].vPos.mx,m_Ball[4].vPos.my - D);
	m_Ball[6].vPos(m_Ball[4].vPos.mx,m_Ball[4].vPos.my - 2*D);

	m_Ball[7].vPos(m_Ball[1].vPos.mx + 3*D*Cos,m_Ball[1].vPos.my+3*D*Sin);
	m_Ball[8].vPos(m_Ball[7].vPos.mx,m_Ball[7].vPos.my-D);
	m_Ball[9].vPos(m_Ball[7].vPos.mx,m_Ball[7].vPos.my-2*D);
	m_Ball[10].vPos(m_Ball[7].vPos.mx,m_Ball[7].vPos.my-3*D);

	m_Ball[11].vPos(m_Ball[1].vPos.mx + 4*D*Cos,m_Ball[1].vPos.my+4*D*Sin);
	m_Ball[12].vPos(m_Ball[11].vPos.mx,m_Ball[11].vPos.my-D);
	m_Ball[13].vPos(m_Ball[11].vPos.mx,m_Ball[11].vPos.my-2*D);
	m_Ball[14].vPos(m_Ball[11].vPos.mx,m_Ball[11].vPos.my-3*D);
	m_Ball[15].vPos(m_Ball[11].vPos.mx,m_Ball[11].vPos.my-4*D);

	for(int i = 0; i < BALL_NUM; ++i)
	{
		m_Ball[i].bIsActive = true;
	}
}

void BallManager::Update( float deltaTime )
{
	BallMoveArea arrBallMoveArea[BALL_NUM] = {};
	for (int i = 0; i < BALL_NUM; ++i)
	{
		BALL& srcBall = m_Ball[i];
		if(srcBall.bIsActive && srcBall.speed > 0.001f)
		{
			float moveTime = srcBall.speed / (UMG);
			if(moveTime > deltaTime)
				moveTime = deltaTime;

			BallMoveArea& ballMoveArea = arrBallMoveArea[i];
			ballMoveArea.fRadii = BallRadii;
			ballMoveArea.vecBeginPos = srcBall.vPos;
			ballMoveArea.vecEndPos = srcBall.vPos + srcBall.dir * (srcBall.speed + 0.5f * UMG * moveTime) * moveTime;

			srcBall.vPos = ballMoveArea.vecEndPos;

			srcBall.speed -= UMG*moveTime;
		}
	}

	for (int i = 0; i < BALL_NUM; ++i)
	{
		BALL& srcBall = m_Ball[i];
		if(srcBall.bIsActive && srcBall.speed > 0.001f)
		{
			if(CrashWall(&srcBall, srcBall.vPos, srcBall.dir))
			{
			}
		}
	}

	//std::vector<int> setIndex;
	for (int i = 0; i < BALL_NUM; ++i)
	{
		BALL& srcBall = m_Ball[i];
		if(srcBall.bIsActive && srcBall.speed > 0.001f)
		{
			for (int j = i + 1; j < BALL_NUM; ++j)
			{
				BALL& desBall = m_Ball[j];
				if(desBall.bIsActive && desBall.speed > 0.001f)
				{
					if(arrBallMoveArea[i].Insert(arrBallMoveArea[j]))
					{
						/*setIndex.insert(i);
						setIndex.insert(j);*/
					}
				}
			}
		}
	}


}

//bool detectionCollide(time_interval, least_time, ball_pairs) {
//
//	res = false;
//	least_time = time_interval;
//
//	foreach ball in billiards:
//	foreach otherBall in billiards:
//	// 求出两球的距离
//	S = distance(ball, otherBall)
//		// 以某一球作为参考坐标系, 则令一球速度向量变为 U’=U-V
//		// 在圆心的直线作为x轴
//		Ux(relative) = Ux(other ball) - Vx(ball)
//		// 	若该方向使得两球远离, 则直接忽略
//		if Ux(relative) < 0：
//			continue
//			// 	某该方向使得两球接近, 则可求其碰撞的预期时间点
//			A' = 2 * A;	// 加速度为原来的两倍
//
//			// 取两者最小的时间点
//			delta_time = min(time_interval, Ux(relative) / Ax’)
//			// 预期距离 小于 两球距离，则在time_interval中不会发生碰撞
//			if 1/2 * Ax’ * delta_time ^ 2 + Ux(relative) * delta_time < S - 2*r:
//	continue
//
//		// 解一元二次方程, 使用二分搜索逼近求解
//		res_time <= slove(1/2 * Ax’ * x ^ 2 + Ux(relative) * x = S - 2 * r)
//
//		if res_time < least_time: 
//	ball_pairs <= (ball, otherBall)
//		least_time = res_time
//		res = true
//
//		foreach wall in billiards:
//	S = distance(ball, wall)
//		// 设垂直于平面的方向为x轴
//		if Vx < 0:
//	continue
//
//		// 取两者最小的时间点
//		delta_time = min(time_interval, Vx / Ax)
//		// 预期距离 小于 两球距离，则在time_interval中不会发生碰撞
//		if 1/2 * Ax * delta_time ^ 2 + Vx * delta_time < S - r:
//	continue
//
//		// 解一元二次方程, 使用二分搜索逼近求解	
//		res_time <= slove(1/2 * A * x ^ 2 + Vx * x = S - r)
//
//		if res_time < least_time:
//	ball_pairs <= (ball, walll)
//		least_time = res_time
//		res = true
//
//		return res
//
//}

void BallManager::Render(int x,int y)
{
	int static RenderX;
	int static RenderY;
	FillRect(m_MapDc,&m_MapRect,m_MapBrush);

	for (int i = 0;i < BALL_NUM; ++i)
	{
		RenderX = int(m_Ball[i].vPos.mx - BallRadii + x + 0.5f);
		RenderY = int(m_Ball[i].vPos.my - BallRadii + y + 0.5f);
		TransparentBlt(*m_pBackDc,RenderX,RenderY,BallDia,BallDia,m_SrcMap[i],0,0,50,50,RGB(125,61,99));
	}
}

void BallManager::_CheckCrash( float tickTime )
{
	if(tickTime < FloatZero) return;

	Vector frameDir[4];
	Vector framePos[4];
	frameDir[0] = Vector(1, 0);
	framePos[0] = Vector(m_FrameLeft,  m_FrameTop);

	frameDir[1] = Vector(0, 1);
	framePos[1] = Vector(m_FrameRight,  m_FrameTop);

	frameDir[2] = Vector(-1, 0);
	framePos[2] = Vector(m_FrameRight, m_FrameBottom);

	frameDir[3] = Vector(0, -1);
	framePos[3] = Vector(m_FrameLeft, m_FrameBottom);

	int srcIndex = 0;
	int desIndex = 0;
	float fMinCrashTime = 0;
	int crashType = 0;
	for(int i = 0; i < BALL_NUM; ++i)
	{
		BALL& srcBall = m_Ball[i];
		float crashTime = 0.0f;
		for (int j = i+1; j < BALL_NUM; ++j)
		{
			BALL& desBall = m_Ball[j];
			if(CheckCrash(srcBall, desBall, tickTime, crashTime))
			{
				if(crashTime < FloatZero)
					continue;

				if(crashType == 0)
				{
					fMinCrashTime = crashTime;
					srcIndex = i;
					desIndex = j;
					crashType = 1;
				}
				else
				{
					if(fMinCrashTime > crashTime)
					{
						fMinCrashTime = crashTime;

						srcIndex = i;
						desIndex = j;
						crashType = 1;
					}
				}
			}
		}

		//计算破壁的效果
		//dy*(x-x0)+dx*(y-y0)=0;a=dy,b=dx,c=-dy*x0-dx*y0
		for (int k = 0; k < 4; ++k)
		{
			//int k = 1;
			if(CheckCrashWall(srcBall, framePos[k], frameDir[k], tickTime, crashTime))
			{
				if(crashTime < FloatZero)
					continue;

				if(crashType == 0)
				{
					fMinCrashTime = crashTime;
					srcIndex = i;
					desIndex = k;
					crashType = 2;
				}
				else
				{
					if(fMinCrashTime > crashTime)
					{
						fMinCrashTime = crashTime;

						srcIndex = i;
						desIndex = k;
						crashType = 2;
					}
				}
			}
			
		}


		if(crashType == 0)
		{
			_Crash(tickTime);
		}
		else
		{
			char szLog[256] = {};
			sprintf_s(szLog, "=====_CheckCrash::nType=%d, crashTime=%f,time=%f=====\n",crashType,  fMinCrashTime, tickTime);
			OutputDebugString(szLog);

			if(fMinCrashTime > tickTime)
			{
				_Crash(tickTime);

				OutputDebugString("fMinCrashTime > tickTime\n");
			}
			else
			{
				if(crashType == 1)
				{
					//进行运动并且
					_Crash(fMinCrashTime);
					//进行碰撞检测

					if(BallCrash(&m_Ball[srcIndex], &m_Ball[desIndex]))
					{
						//剩余的时间在Update
						_CheckCrash(tickTime - fMinCrashTime);
						//pSrcBall->speed = 0.0f;
						//pDesBall->speed = 0.0f;
						//OutputDebugString("------\n");
					}
					else
					{
						OutputDebugString("error");
					}
				}
				else
				{
					//进行运动
					_Crash(fMinCrashTime);
					//进行碰撞检测

					if(CrashWall(&m_Ball[srcIndex],  framePos[desIndex], frameDir[desIndex]))
					{
						//剩余的时间在Update
						_CheckCrash(tickTime - fMinCrashTime);
						//pSrcBall->speed = 0.0f;
						//pDesBall->speed = 0.0f;
						//OutputDebugString("------\n");
					}
					else
					{
						OutputDebugString("error");
					}
				}
			}
		}
	}


	
}

void BallManager::_Crash( float time )
{
	if (time <= 0.0f)
		return;

	for (int i = 0; i < BALL_NUM; ++i)
	{
		m_Ball[i].Move(time);
	}
}

void BallManager::SetOriginGlobePos( Vector CursorPos )
{
	m_Ball[0].dir = CursorPos - m_Ball[0].vPos;
	m_Ball[0].dir.SelFNormal();
}

void BallManager::SetOriginGlobeSpend( float Speed )
{
	Speed = 2.0f;
	m_Ball[0].speed = Speed;
}

void BallManager::_CrashFrame(int i,float speed)
{
	//m_Ball[i].vPos+=m_Ball[i].dir;
	//if (m_Ball[i].vPos.mx<=m_FrameLeft)
	//{
	//	m_Ball[i].vPos.ReflexYLine(m_FrameLeft);
	//	m_Ball[i].dir.mx=-m_Ball[i].dir.mx;
	//	if (m_Ball[i].vPos.my<=m_FrameTop)
	//	{
	//		m_Ball[i].vPos.ReflexXLine(m_FrameTop);
	//		m_Ball[i].dir.my=-m_Ball[i].dir.my;
	//	}
	//	else
	//	{
	//		if (m_Ball[i].vPos.my>=m_FrameBottom)
	//		{
	//			m_Ball[i].vPos.ReflexXLine(m_FrameBottom);
	//			m_Ball[i].dir.my=-m_Ball[i].dir.my;
	//		}
	//	}
	//}
	//else
	//{
	//	if (m_Ball[i].vPos.mx>=m_FrameRight)
	//	{
	//		m_Ball[i].vPos.ReflexYLine(m_FrameRight);
	//		m_Ball[i].dir.mx=-m_Ball[i].dir.mx;
	//		if (m_Ball[i].vPos.my<=m_FrameTop)
	//		{
	//			m_Ball[i].vPos.ReflexXLine(m_FrameTop);
	//			m_Ball[i].dir.my=-m_Ball[i].dir.my;
	//		}
	//		else
	//		{
	//			if (m_Ball[i].vPos.my>=m_FrameBottom)
	//			{
	//				m_Ball[i].vPos.ReflexXLine(m_FrameBottom);
	//				m_Ball[i].dir.my=-m_Ball[i].dir.my;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if (m_Ball[i].vPos.my<=m_FrameTop)
	//		{
	//			m_Ball[i].vPos.ReflexXLine(m_FrameTop);
	//			m_Ball[i].dir.my=-m_Ball[i].dir.my;
	//		}
	//		else
	//		{
	//			if (m_Ball[i].vPos.my>=m_FrameBottom)
	//			{
	//				m_Ball[i].vPos.ReflexXLine(m_FrameBottom);
	//				m_Ball[i].dir.my=-m_Ball[i].dir.my;
	//			}
	//		}
	//	}
	//}
}

void BallManager::SetCursorPos( Vector const&CursorPos )
{
	m_CusorPos = CursorPos;

	if(CursorPos.mx < m_FrameLeft || CursorPos.mx > m_FrameRight )
		return;

	if(CursorPos.my < m_FrameTop || CursorPos.my > m_FrameBottom)
		return;

	Vector dir = m_CusorPos-m_Ball[0].vPos;
	float speed = dir.VectorMod();
	if(speed <= 0.00001f)
		return;

	speed = 100.0f;
	m_Ball[0].dir = dir.VectorNormal();
	m_Ball[0].speed = speed * 2.0f;
}

void BallManager::SetShowCursor( bool IsShow /*= false*/ )
{
	m_IsShowCursor = IsShow;
}

bool BallManager::IsStop()
{
	for (int i = 0; i < BALL_NUM; ++i)
	{
		if(m_Ball[i].speed > 0.000001f)
			return false;
	}

	return true;
}


