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
	Vector& XReflexSelf();	//��X��Գ�
	Vector XReflex();	//��X��Գ�
	Vector& YReflexSelf();	//��Y��Գ�
	Vector YReflex();	//��Y��Գ�
	Vector& XYReflexSelf();	//��XY��Գ�
	Vector XYReflex();	//��XY��Գ�
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
