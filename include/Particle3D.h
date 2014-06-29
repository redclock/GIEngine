/************************************************************************/
/*  Direct3D GameEngine													*/
/*  3DÁ£×ÓÏµÍ³                            ±àÐ´£º Ò¦´ºêÍ                 */
/************************************************************************/

#pragma once

#include <d3dx9.h>
#include <vector>

#include "MyLib.h"
#include "Image2D.h"

typedef VERTEX_XYZC PARTICLE_VERTEX;

class CParticle3D
{
public:

	D3DXVECTOR3 pos;
	D3DXVECTOR3 vel;
	D3DXVECTOR3 acc;

	float wholelife;
	float curlife;
	float decay;

	bool  active;
	D3DXCOLOR color_from;
	D3DXCOLOR color_to; 
	D3DXCOLOR color; 

	bool Update(float elapsedTime)
	{
		curlife -= elapsedTime * decay;
		if (curlife <= 0) return true;

		D3DXColorLerp(&color, &color_to, &color_from, curlife / wholelife); 
		pos += vel * elapsedTime;
		vel += acc * elapsedTime;
		return false;
	};
};

class CParticle3DListener
{
public:
	virtual void OnInitParticle(CParticle3D* p, int index) {};
	virtual void OnMoveParticle(CParticle3D* p, int index) {};
};

class CParticle3DSystem
{
private:
	CParticle3D * m_particles;
	bool m_finished;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
	CParticle3DListener * m_listener;
public:
	CImage2D * img;	
	UINT Count;
	float psize;
	bool Looped;
	void Init(int num);

	void Render();
	void Update(float elapsedTime);

	bool IsFinished() const { return m_finished; };
	void Reset();
	void Destory();

	CParticle3DSystem(CParticle3DListener * listener);
	~CParticle3DSystem(void);
};