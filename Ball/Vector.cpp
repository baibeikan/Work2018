#include "stdafx.h"
#include ".\vector.h"

Vector::Vector(void)
{
	mx = 0.0f;
	my	= 0.0f;
}
Vector::~Vector(void)
{
}

Vector::Vector( Vector const &other )
{
	this->mx = other.mx;
	this->my = other.my;
}

Vector::Vector( float x,float y )
{
	this->mx = x;
	this->my = y;
}

Vector& Vector::operator=( Vector const &other )
{
	this->mx = other.mx;
	this->my = other.my;
	return *this;
}

Vector& Vector::operator()( float x ,float y )
{
	this->mx = x;
	this->my = y;
	return *this;
}

Vector Vector::operator-( Vector const &other ) const
{
	static Vector TempVector;
	TempVector.mx = this->mx - other.mx;
	TempVector.my = this->my - other.my;
	return TempVector;
}

Vector Vector::operator+( Vector const &other ) const
{
	static Vector TempVector;
	TempVector.mx = this->mx + other.mx;
	TempVector.my = this->my + other.my;
	return TempVector;
}

Vector& Vector::operator-=( Vector const &other )
{
	this->mx = this->mx - other.mx;
	this->my = this->my - other.my;
	return *this;
}

Vector& Vector::operator+=( Vector const &other )
{
	this->mx = this->mx + other.mx;
	this->my = this->my + other.my;
	return *this;
}

float Vector::operator*( Vector const &other )
{
	return this->mx*other.mx + this->my * other.my;
}

Vector Vector::operator*( int num )
{
	static Vector TempVector;
	TempVector.mx = this->mx*num;
	TempVector.my = this->my*num;
	return TempVector;
}

Vector Vector::operator*( float num )
{
	static Vector TempVector;
	TempVector.mx = this->mx*num;
	TempVector.my = this->my*num;
	return TempVector;
}

Vector Vector::operator*( double num )
{
	float TempNum = float(num);
	static Vector TempVector;
	TempVector.mx = this->mx*TempNum;
	TempVector.my = this->my*TempNum;
	return TempVector;
}

Vector& Vector::operator*=( int num )
{
	this->mx*=num;
	this->my*=num;
	return *this;
}

Vector& Vector::operator*=( float num )
{
	this->mx*=num;
	this->my*=num;
	return *this;
}

Vector& Vector::operator*=( double num )
{
	float TempNum = float(num);
	this->mx*=TempNum;
	this->my*=TempNum;
	return *this;
}

Vector operator*( Vector const &Src,int num )
{
	static Vector TempVector;
	TempVector.mx = Src.mx*num;
	TempVector.my = Src.my*num;
	return TempVector;
}

Vector operator*( Vector const &Src,double num )
{
	float TempNum = float(num);
	static Vector TempVector;
	TempVector.mx = Src.mx*TempNum;
	TempVector.my = Src.my*TempNum;
	return TempVector;
}

Vector operator*( Vector const &Src,float num )
{
	static Vector TempVector;
	TempVector.mx = Src.mx*num;
	TempVector.my = Src.my*num;
	return TempVector;
}

Vector Vector::operator/( int num )
{
	static Vector TempVector;
	TempVector.mx = this->mx/num;
	TempVector.my = this->my/num;
	return TempVector;
}

Vector Vector::operator/( float num )
{
	static Vector TempVector;
	TempVector.mx = this->mx/num;
	TempVector.my = this->my/num;
	return TempVector;
}

Vector Vector::operator/( double num )
{
	static float TempNum = float(num);
	static Vector TempVector;
	TempVector.mx = this->mx/TempNum;
	TempVector.my = this->my/TempNum;
	return TempVector;
}

Vector& Vector::operator/=( int num )
{
	this->mx /= num;
	this->my /= num;
	return *this;
}

Vector& Vector::operator/=( float num )
{
	this->mx /= num;
	this->my /= num;
	return *this;
}

Vector& Vector::operator/=( double num )
{
	static float TempNum = float(num);
	this->mx /= TempNum;
	this->my /= TempNum;
	return *this;
}

float Vector::CosVectorAngle( Vector const&other )
{
	return (this->mx * other.mx + this->my * other.my)/(this->VectorMod() * other.VectorMod());
}

float Vector::VectorMod()const
{
	return sqrtf(mx * mx + my * my);
}

Vector Vector::VectorNormal() const
{
	static Vector TempVector(0,0);
	float lenght = this->VectorMod();
	if(lenght < FloatZero) return Vector(0, 0); 

	float LenghtReciprocal = 1 / lenght;
	TempVector.mx = this->mx * LenghtReciprocal;
	TempVector.my = this->my * LenghtReciprocal;
	return TempVector;
}

Vector& Vector::SelFNormal()
{
	*this = this->VectorNormal();
	return *this;
}

Vector& Vector::XReflexSelf()
{
	this->my = -this->my;
	return *this;
}

Vector Vector::XReflex()
{
	static Vector TempVector;
	TempVector.mx = this->mx;
	TempVector.my = -this->my;
	return TempVector;
}

Vector& Vector::YReflexSelf()
{
	this->mx = -this->mx;
	return *this;
}

Vector Vector::YReflex()
{
	static Vector TempVector;
	TempVector.mx = -this->mx;
	TempVector.my = this->my;
	return TempVector;
}

Vector& Vector::XYReflexSelf()
{
	this->my = -this->my;
	this->mx = -this->mx;
	return *this;
}	

Vector Vector::XYReflex()
{
	static Vector TempVector;
	TempVector.mx = -this->mx;
	TempVector.my = -this->my;
	return TempVector;
}

Vector& Vector::Abs()
{
	if (this->mx < 0.0f)
	{
		this->mx = -this->mx;
	}
	if (this->my < 0.0f)
	{
		this->my = -this->my;
	}
	return *this;
}

Vector& Vector::ReflexXLine( int line )
{
	this->my = 2*line-this->my;
	return *this;
}

Vector& Vector::ReflexXLine( float line )
{
	this->my = 2*line - this->my;
	return *this;
}
Vector& Vector::ReflexYLine( int line )
{
	this->mx = 2*line-this->mx;
	return *this;
}

Vector& Vector::ReflexYLine( float line )
{
	this->mx = 2*line-this->mx;
	return *this;
}

bool Vector::operator==( Vector const&other)
{
	if (this->CosVectorAngle(other) > 0.995f)
	{
		return true;
	}
	return false;
}

float Vector::VectorSlope()
{
	return (this->my/this->mx);
}

Vector Vector::GetVectorAboutLine(float CutLineSlope,float TwoBallSlope)
{
	static Vector TempVector;
	TempVector.mx = (this->mx - 2*this->my + 2*TwoBallSlope*this->mx)/(TwoBallSlope - CutLineSlope);
	TempVector.my = CutLineSlope*(this->mx + TempVector.mx) - this->my;
	return TempVector;
}