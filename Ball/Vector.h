#pragma once
#include <math.h>
struct Vector
{
	Vector(void);
	~Vector(void);
	Vector(Vector const &other);
	Vector(float x,float y);
	Vector& operator = (Vector const &other);
	Vector& operator()(float x ,float y);
	Vector operator - (Vector const &other) const;
	Vector operator + (Vector const &other) const;
	Vector& operator -=(Vector const &other);
	Vector& operator +=(Vector const &other);
	float operator*(Vector const &other);
	Vector operator*(int num);
	Vector operator*(float num);
	Vector operator*(double num);
	Vector& operator*=(int num);
	Vector& operator*=(float num);
	Vector& operator*=(double num);
	Vector operator/(int num);
	Vector operator/(float num);
	Vector operator/(double num);
	Vector& operator/=(int num);
	Vector& operator/=(float num);
	Vector& operator/=(double num);
	friend Vector operator*( Vector const &Src,int num);
	friend Vector operator*( Vector const &Src,float num);
	friend Vector operator*( Vector const &Src,double num);
	float CosVectorAngle(Vector const&other);
	float VectorMod() const;
	Vector VectorNormal() const;
	Vector& SelFNormal();
	Vector& XReflexSelf();	//以X轴对称
	Vector XReflex();	//以X轴对称
	Vector& YReflexSelf();	//以Y轴对称
	Vector YReflex();	//以Y轴对称
	Vector& XYReflexSelf();	//以XY轴对称
	Vector XYReflex();	//以XY轴对称
	Vector& Abs();
	Vector& ReflexXLine(int line);
	Vector& ReflexXLine(float line);
	Vector& ReflexYLine(int line);
	Vector& ReflexYLine(float line);
	bool operator==(Vector const&other);
	float VectorSlope();
	Vector GetVectorAboutLine(float CutLineSlope,float TwoBallSlope);
public:
	float mx;
	float my;
};
