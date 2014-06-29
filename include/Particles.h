/************************************************************************/
/*  Direct3D GameEngine													*/
/*  2DÁ£×ÓÏµÍ³                            ±àÐ´£º Ò¦´ºêÍ                 */
/************************************************************************/
#pragma once
#include <d3dx9.h>
#include "Image2D.h"

struct Particle2D
{
	CImage2D * img;	
	float x, y;
	float vx, vy;
	float accx, accy;
	float wholelife;
	float curlife;
	float decay;
	bool  active;
	D3DXCOLOR color_from;
	D3DXCOLOR color_to; 
	D3DXCOLOR color; 

	D3DXMATRIXA16 transform;
	
	RECT * rect;

	bool move(float time)
	{
		curlife -= time * decay;
		if (curlife <= 0) return true;

		D3DXColorLerp(&color, &color_to, &color_from, curlife / wholelife); 
		x += time * vx;
		y += time * vy;
		vx += time * accx;
		vy += time * accy;
		return false;
	};
};

class CParticleListener
{
public:
	virtual void OnInitParticle(Particle2D* p, int index) {};
	virtual void OnMoveParticle(Particle2D* p, int index) {};
};

class CParticles
{
private:
	Particle2D * m_particles;
	CParticleListener * m_listener; 
	bool m_finished;
public:
	UINT Count;
	bool Looped;
	void Init(int num);
	void ResetAll();
	
	void Render();
	void Move(float time);
	
	bool IsFinished() const { return m_finished; };
	void Destory();

	CParticles(CParticleListener * listener);
	~CParticles(void);
};

