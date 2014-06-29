#include "FlyController.h"

CFlyController::CFlyController(float flyTime)
{
	FlyTime = flyTime;
	State = STATE_IN;
	timer = 0;
}

void CFlyController::Update(float elapsedTime)
{
	switch (State)
	{
	case STATE_IN:
		if (timer + elapsedTime >= FlyTime)
		{
			timer = FlyTime;
			Move(FlyTime - timer);
			State = STATE_SHOW;
		}else
		{
			timer += elapsedTime;
			Move(elapsedTime);
		}
		break;
	case STATE_SHOW:
		break;
	case STATE_OUT:
		if (timer - elapsedTime <= 0)
		{
			timer = 0;
			Move(timer);
			State = STATE_FINISH;
		}else
		{
			timer -= elapsedTime;
			Move(elapsedTime);
		}
		break;
	}
}

CFlyLineController::CFlyLineController(float sx, float sy, float ex, float ey, float flyTime): CFlyController(flyTime)
{
	SpeedX = (ex - sx) / flyTime;
	SpeedY = (ey - sy) / flyTime;
	X = sx;
	Y = sy;
}

void CFlyLineController::Move(float elapsedTime)
{
	switch (State)
	{
	case STATE_IN:
		X += SpeedX * elapsedTime;
		Y += SpeedY * elapsedTime;
		break;
	case STATE_OUT:
		X -= SpeedX * elapsedTime;
		Y -= SpeedY * elapsedTime;
		break;

	}
}