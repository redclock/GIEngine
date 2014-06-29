#pragma once


class CFlyController
{
protected:
	float timer;
	virtual void Move(float elapsedTime) = 0;
public:
	enum
	{
		STATE_IN, STATE_SHOW, STATE_OUT, STATE_FINISH
	} State;
	float X, Y;
	float FlyTime;
	void Update(float elapsedTime);
	CFlyController(float flyTime);
};

class CFlyLineController: public CFlyController
{
protected:
	void Move(float elapsedTime);
public:
	float  SpeedX;
	float  SpeedY;
	CFlyLineController(float sx, float sy, float ex, float ey, float flyTime);
};