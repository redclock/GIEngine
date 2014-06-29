/************************************************************************/
/* Á£×ÓÏµÍ³                                                             */
/************************************************************************/

#include ".\particles.h"
#include "GameCommon.h"
#include "GameEngine.h"

CParticles::CParticles(CParticleListener * listener)
{
	m_listener = listener;
	m_particles = NULL;
	Count = 0;
	Looped = false;
	m_finished = false;
}

CParticles::~CParticles(void)
{
	SAFE_DELETE_ARRAY(m_particles);
}

void CParticles::Move(float time)
{
	if (m_particles == NULL) return;
	Particle2D * p = m_particles;
	m_finished = true;
	for (UINT i = 0; i < Count; i++, p++)
	{
		if (p->active)
		{
			m_finished = false;
			if (p->move(time))
			{
				if (Looped && m_listener)	m_listener->OnInitParticle(p, i);
				else p->active = false;
			}else if (m_listener){
				m_listener->OnMoveParticle(p, i);
			}
		}
	}	
}

void CParticles::Render()
{
	if (m_particles == NULL) return;
	Particle2D * p = m_particles;
	D3DXMATRIXA16 mat;
	for (UINT i = 0; i < Count; i++, p++)
	{
		if (p->active && p->img)
		{
			D3DXMatrixTranslation(&mat, p->x, p->y, 0.0f);
			mat = p->transform * mat;
			Get2D()->DrawImage(p->img, 0, 0, p->rect, &mat, p->color);
		}
	}	
}

void CParticles::Init(int num)
{
	Count = num;
	SAFE_DELETE_ARRAY(m_particles);
	m_particles = new Particle2D[Count];
	ZeroMemory(m_particles, Count * sizeof(Particle2D));
}

void CParticles::ResetAll()
{
	if (m_particles == NULL) return;
	if (m_listener == NULL) return;
	for (UINT i = 0; i < Count; i++)
	{
		m_listener->OnInitParticle(&m_particles[i], i);
	}
}

void CParticles::Destory()
{
	SAFE_DELETE_ARRAY(m_particles);
}
