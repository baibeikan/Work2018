#ifndef AFX_MYBALL
#define AFX_MYBALL
#define BallRadii 15.0f		// 球的半径
#define BallDia 30.f //球的直径

struct Circle
{
	Vector vecCenter;
	float fRadii;
};

struct HalfCircle : public Circle
{
	Vector vecDir;
};


// 线段
struct Linear
{
	Vector vecBeginPos;
	Vector vecEndPos;
};


bool Insert(const Circle& left, const Circle& right)
{
	return true;
}

bool Insert(const Circle& left, const HalfCircle& right)
{
	return true;
}

bool Insert(const Circle& left, const Linear& right)
{
	float x = 0, y = 0;
	//(x  - left.vecCenter.mx) ^ 2 + (y - left.vecCenter.my) ^ 2 = left.fRadii ^ 2;

	float a21 = right.vecBeginPos.my - right.vecEndPos.my;
	float a22 = -(right.vecBeginPos.mx - right.vecEndPos.mx);
	float b2 = a21 * right.vecEndPos.mx + a22 * right.vecEndPos.my;




	return true;
}

bool Insert(const HalfCircle& left, const Circle& right)
{
	return Insert(right, left);
}

bool Insert(const HalfCircle& left, const HalfCircle& right)
{
	return true;
}

bool Insert(const HalfCircle& left, const Linear& right)
{
	return true;
}

bool Insert(const Linear& left, const Circle& right)
{
	return Insert(right, left);
}

bool Insert(const Linear& left, const HalfCircle& right)
{
	return Insert(right, left);
}

bool Insert(const Linear& left, const Linear& right)
{
	float a11 = left.vecBeginPos.my - left.vecEndPos.my;
	float a12 = -(left.vecBeginPos.mx - left.vecEndPos.mx);
	float b1 = a11 * left.vecEndPos.mx + a12 * left.vecEndPos.my;

	float a21 = right.vecBeginPos.my - right.vecEndPos.my;
	float a22 = -(right.vecBeginPos.mx - right.vecEndPos.mx);
	float b2 = a21 * right.vecEndPos.mx + a22 * right.vecEndPos.my;

	float d = a11 * a22 - a12 * a21;
	if(fabs(d) > 0.001f)
	{

		float x = (b1 * a22 - b2 * a12) / d;
		//float y = (a11 * b2 - a21 * b1) / d;
		if(x > min(left.vecBeginPos.mx, left.vecEndPos.mx) && x < max(left.vecBeginPos.mx, left.vecEndPos.mx))
		{
			if(x > min(right.vecBeginPos.mx, right.vecEndPos.mx) && x < max(right.vecBeginPos.mx, right.vecEndPos.mx))
			{
				return true;
			}
		}
	}

	return false;
}

struct BallMoveArea
{
	float fRadii;
	Vector vecBeginPos;
	Vector vecEndPos;

	bool Insert(const BallMoveArea& other) const
	{
		Vector vecDir(vecEndPos-vecBeginPos);
		Vector vecOtherDir((Vector)other.vecEndPos-other.vecBeginPos);

		// 检测两个圆是否相交 两个三角形
		if(vecDir.VectorMod() > 0.001f)
		{
			//中间的两条线段
			Vector vecNormal(vecDir.my, -vecDir.mx); // 方向的法线
			vecNormal.SelFNormal(); 
			vecNormal *= fRadii;

			Linear arrLinear[2] = {};

			Linear& linear = arrLinear[0];
			linear.vecBeginPos = vecBeginPos + vecNormal;
			linear.vecEndPos = vecEndPos + vecNormal;

			linear = arrLinear[1];
			linear.vecBeginPos = vecBeginPos - vecNormal;
			linear.vecEndPos = vecEndPos - vecNormal;

			// (x - vectBeginPos.x) ^ 2 + (y - vectEndPos.y) ^ 2 = fRadii ^ 2
			HalfCircle arrHalfCircle[2] = {};
			Vector vecDirNormal = vecDir.SelFNormal(); // 方向的单位向量 

			//第一个半圆弧
			HalfCircle& halfCircle = arrHalfCircle[0];
			halfCircle.vecCenter = vecBeginPos;
			halfCircle.fRadii = fRadii;
			halfCircle.vecDir = Vector(-vecDirNormal.mx, -vecDirNormal.my);

			//第二个半圆弧
			halfCircle = arrHalfCircle[1];
			halfCircle.vecCenter = vecEndPos;
			halfCircle.fRadii = fRadii;
			halfCircle.vecDir = vecDirNormal;
			if (vecOtherDir.VectorMod() > 0.001f)
			{
				//中间的两条线段
				Vector vecOtherNormal(vecOtherDir.my, -vecOtherDir.mx);
				vecOtherNormal.SelFNormal();
				vecOtherNormal *= other.fRadii;

				Linear arrLinearOther[2] = {};

				// 左边一条
				Linear& linearOther = arrLinearOther[0];
				linearOther.vecBeginPos = other.vecBeginPos + vecOtherNormal;
				linearOther.vecEndPos = other.vecEndPos + vecOtherNormal;

				// 右边一条
				linearOther = arrLinearOther[1];
				linearOther.vecBeginPos = other.vecBeginPos - vecOtherNormal;
				linearOther.vecEndPos = other.vecEndPos - vecOtherNormal;
				for (int i = 0; i < 2; ++i)
				{
					for (int j = i; j < 2; ++j)
					{
						if (::Insert(arrLinear[i], arrLinearOther[j]))
							return true;
					}
				}

				for (int i = 0; i < 2; ++i)
				{
					for (int j = i; j < 2; ++j)
					{
						if (::Insert(arrHalfCircle[i], arrLinearOther[j]))
							return true;
					}
				}

				HalfCircle arrHalfCircleOther[2] = {};
				Vector vecDirNormalOther = vecOtherDir.VectorNormal(); 

				//第一个半圆弧
				HalfCircle& halfCircleOther = arrHalfCircleOther[0];
				halfCircleOther.vecCenter = other.vecBeginPos;
				halfCircleOther.fRadii = other.fRadii;
				halfCircleOther.vecDir = Vector(-vecDirNormalOther.mx, -vecDirNormalOther.my);

				//第二个半圆弧
				halfCircleOther = arrHalfCircleOther[1];
				halfCircleOther.vecCenter = other.vecEndPos;
				halfCircleOther.fRadii = other.fRadii;
				halfCircleOther.vecDir = vecDirNormalOther;
				for (int i = 0; i < 2; ++i)
				{
					for (int j = i; j < 2; ++j)
					{
						if (::Insert(arrLinear[i], arrHalfCircleOther[j]))
							return true;
					}
				}

				for (int i = 0; i < 2; ++i)
				{
					for (int j = i; j < 2; ++j)
					{
						if (::Insert(arrHalfCircle[i], arrHalfCircleOther[j]))
							return true;
					}
				}
			}
			else
			{
				Circle circleOther;
				circleOther.vecCenter = other.vecBeginPos;
				circleOther.fRadii = other.fRadii;
				for (int i = 0; i < 2; ++i)
				{
					if(::Insert(arrLinear[i], circleOther))
						return true;
				}

				for (int i = 0; i < 2; ++i)
				{
					if(::Insert(arrHalfCircle[i], circleOther))
						return true;
				}
			}
		}
		else
		{
			//就是一个园
			// (x - vectBeginPos.x) ^ 2 + (y - vectEndPos.y) ^ 2 = fRadii ^ 2;
			Circle circle;
			circle.vecCenter = vecBeginPos;
			circle.fRadii = fRadii;
			if (vecOtherDir.VectorMod() > 0.001f)
			{
				//中间的两条线段
				Vector vecOtherNormal(vecOtherDir.my, -vecOtherDir.mx);
				vecOtherNormal.SelFNormal();
				vecOtherNormal *= other.fRadii;

				Linear arrLinearOther[2] = {};

				// 左边一条
				Linear& linearOther = arrLinearOther[0];
				linearOther.vecBeginPos = other.vecBeginPos + vecOtherNormal;
				linearOther.vecEndPos = other.vecEndPos + vecOtherNormal;

				// 右边一条
				linearOther = arrLinearOther[1];
				linearOther.vecBeginPos = other.vecBeginPos - vecOtherNormal;
				linearOther.vecEndPos = other.vecEndPos - vecOtherNormal;
				for (int i = 0; i < 2; ++i)
				{
					if (::Insert(circle, arrLinearOther[i]))
					{
						return true;
					}
				}

				HalfCircle arrHalfCircleOther[2] = {};

				Vector vecDirNormalOther = vecOtherDir.VectorNormal(); 
				HalfCircle& halfCircleOther = arrHalfCircleOther[0];
				//第一个半圆弧
				halfCircleOther.vecCenter = other.vecBeginPos;
				halfCircleOther.fRadii = other.fRadii;
				halfCircleOther.vecDir = Vector(-vecDirNormalOther.mx, -vecDirNormalOther.my);

				//第二个半圆弧
				halfCircleOther = arrHalfCircleOther[1];
				halfCircleOther.vecCenter = other.vecEndPos;
				halfCircleOther.fRadii = other.fRadii;
				halfCircleOther.vecDir = vecDirNormalOther;
				for (int i = 0; i < 2; ++i)
				{
					if (::Insert(circle, arrHalfCircleOther[i]))
						return true;
				}
			}
			else
			{
				Circle circleOther;
				circleOther.vecCenter = other.vecBeginPos;
				circleOther.fRadii = other.fRadii;

				return ::Insert(circle, circleOther);
			}
		}

		return false;
	}

	//bool Insert(const Linear& _linear)
	//{
	//	Vector vecDir(vecEndPos-vecBeginPos);
	//	if(vecDir.VectorMod() > 0.001f)
	//	{
	//		//中间的两条线段
	//		Vector vecNormal(vecDir.my, -vecDir.mx); // 方向的法线
	//		vecNormal.SelFNormal();
	//		vecNormal *= fRadii;

	//		Linear arrLinear[2] = {};

	//		Linear& linear = arrLinear[0];
	//		linear.vecBeginPos = vecBeginPos + vecNormal;
	//		linear.vecEndPos = vecEndPos + vecNormal;

	//		linear = arrLinear[1];
	//		linear.vecBeginPos = vecBeginPos - vecNormal;
	//		linear.vecEndPos = vecEndPos - vecNormal;

	//		for (int i = 0; i < 2; ++i)
	//		{
	//			if(::Insert(arrLinear[i], _linear))
	//				return true;
	//		}

	//		// (x - vectBeginPos.x) ^ 2 + (y - vectEndPos.y) ^ 2 = fRadii ^ 2
	//		HalfCircle arrHalfCircle[2] = {};
	//		Vector vecDirNormal = vecDir.SelFNormal(); // 方向的单位向量 

	//		//第一个半圆弧
	//		HalfCircle& halfCircle = arrHalfCircle[0];
	//		halfCircle.vecCenter = vecBeginPos;
	//		halfCircle.fRadii = fRadii;
	//		halfCircle.vecDir = Vector(-vecDirNormal.mx, -vecDirNormal.my);

	//		//第二个半圆弧
	//		halfCircle = arrHalfCircle[1];
	//		halfCircle.vecCenter = vecEndPos;
	//		halfCircle.fRadii = fRadii;
	//		halfCircle.vecDir = vecDirNormal;
	//		for (int i = 0; i < 2; ++i)
	//		{
	//			if(::Insert(arrHalfCircle[i], _linear))
	//				return true;
	//		}
	//	}
	//	else
	//	{
	//		Circle circle;
	//		circle.vecCenter = vecBeginPos;
	//		circle.fRadii = fRadii;

	//		return ::Insert(circle, _linear);
	//	}
	//	return false;
	//}
};


struct BALL
{
public:
	Vector vPos;
	Vector dir;
	float speed;
	bool bIsMoving;
	bool bIsActive;
public:
	BALL();
	BALL(BALL const &other);
	BALL& operator=(BALL const &other);
	~BALL();

	void Move(float time);
};

#endif