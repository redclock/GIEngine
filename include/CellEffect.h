#pragma once
#include "Particle3D.h"
class CCellEffect
{
protected:
	bool m_stopped;
	float m_timer;
public:
	CCellEffect() { m_stopped = false; m_timer = 0;};
	virtual ~CCellEffect() {};
	bool IsStopped() const { return m_stopped; };

	virtual void Render() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Reset() = 0;
};

class CParticle3DEffect: public CCellEffect, public CParticle3DListener
{
protected:
	CParticle3DSystem* m_p;
public:
	CParticle3DEffect();
	virtual ~CParticle3DEffect();

	virtual void OnMoveParticle(CParticle3D* p, int index) {};
	virtual void Render();
	virtual void Update(float elapsedTime);
	virtual void Reset() = 0;
};

class CFireEffect: public CParticle3DEffect
{
public:
	float size;
	int count;
	CFireEffect(float size, int count);
	~CFireEffect();

	void OnInitParticle(CParticle3D* p, int index);
	void Reset();
};

class CSparkEffect: public CParticle3DEffect
{
public:
	D3DXCOLOR Color;
	float Size;
	CImage2D * Img;
	int Num;

	CSparkEffect(CImage2D * img);
	~CSparkEffect();

	void OnInitParticle(CParticle3D* p, int index);
	void Reset();
};
