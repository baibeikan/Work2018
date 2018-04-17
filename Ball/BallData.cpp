#include "stdafx.h"
#include "BallData.h"

#define COS45 (sqrt(2.0f)/2.0f)
#define COS0  0.995f
#define COS90 0.000f



BALL::BALL()
{
	vPos(0.0f, 0.0f);
	dir(0.0f,0.0f);
	bIsMoving = false;
	bIsActive = true;
}

BALL::BALL( BALL const &other )
{
	vPos = other.vPos;
	dir = other.dir;
	bIsMoving = other.bIsMoving;
	bIsActive = other.bIsActive;
}

BALL& BALL::operator=( BALL const &other )
{
	vPos = other.vPos;
	dir = other.dir;
	return *this;
}

BALL::~BALL()
{

}

void BALL::Move(float time )
{
	if(time <= 0.0f) return;
	float speedSquare = speed * speed;
	if(speedSquare < FloatZero) { return;}

	float speed = sqrtf(speedSquare);

	float moveTime = speed / FrictionCoefficient;
	if(moveTime > time)
		moveTime = time;

	Vector vAverageSpend = dir * FrictionCoefficient * moveTime;
	Vector vDisplacement = (dir * speed - vAverageSpend * 0.5f) * moveTime;
	vPos += vDisplacement;
	speed -= vAverageSpend.VectorMod();
}
