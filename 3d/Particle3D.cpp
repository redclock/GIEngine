#include "Particle3D.h"
#include "GameCommon.h"
#include "MyLib.h"
#include "GameEngine.h"

CParticle3DSystem::CParticle3DSystem(CParticle3DListener * listener)
{
	m_pVB = NULL;
	m_listener = listener;
	m_particles = NULL;
	psize = 0.8f;
}

CParticle3DSystem::~CParticle3DSystem()
{
	SAFE_RELEASE(m_pVB);
	SAFE_DELETE_ARRAY(m_particles);
}

void CParticle3DSystem::Init(int num)
{
	Count = num;
	SAFE_DELETE_ARRAY(m_particles);
	Destory();
	m_particles = new CParticle3D[Count];
	ZeroMemory(m_particles, Count * sizeof(CParticle3D));

	for (UINT i = 0; i < Count; i++)
	{
		m_listener->OnInitParticle(&m_particles[i], i);
	}

	Reset();
}

void CParticle3DSystem::Reset()
{
	if (m_pVB) return;
	HRESULT	hr;

	if(FAILED(hr = GetDevice()->CreateVertexBuffer( Count * sizeof(PARTICLE_VERTEX), 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
		PARTICLE_VERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
		MessageBox(NULL, "Particle 3d createVB error", "ERROR", 0);
		return;
	}

	return;
}

void CParticle3DSystem::Destory()
{
	SAFE_RELEASE(m_pVB);
}

void CParticle3DSystem::Render()
{
	int NumToRender = 0;
	HRESULT hr;
	
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	// Set the render states for using point sprites设置点精灵的渲染状态

	pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	pDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(psize) );
	pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
	pDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
	pDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
	pDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );
	
	PARTICLE_VERTEX* pVertices;
	if( FAILED( hr = m_pVB->Lock( 0, Count * sizeof(PARTICLE_VERTEX),
					(void**) &pVertices, D3DLOCK_DISCARD ) ) )
	{
		return;
	}

	for (UINT i = 0; i < Count; i++)
	{
		if (m_particles[i].active)
		{
			pVertices->x = m_particles[i].pos.x;
			pVertices->y = m_particles[i].pos.y;
			pVertices->z = m_particles[i].pos.z;
			pVertices->color = m_particles[i].color;
			pVertices ++;
			NumToRender ++;
		}
	}
	m_pVB->Unlock();

	// Set up the vertex buffer to be rendered建立渲染用顶点缓冲区
	pDevice->SetStreamSource( 0, m_pVB, 0, sizeof(PARTICLE_VERTEX) );
	pDevice->SetFVF( PARTICLE_VERTEX::FVF );
	TextureModulateDiffuse(img->GetTexture());
	if( NumToRender )
	{
		if(FAILED(hr = pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, NumToRender)))
			return;
	}

	// Reset render states重新设置渲染状态
	pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
}

void CParticle3DSystem::Update(float elapsedTime)
{
	if (m_particles == NULL) return;
	CParticle3D * p = m_particles;
	m_finished = true;
	for (UINT i = 0; i < Count; i++, p++)
	{
		if (p->active)
		{
			m_finished = false;
			if (p->Update(elapsedTime))
			{
				if (Looped && m_listener)	m_listener->OnInitParticle(p, i);
				else p->active = false;
			}else if (m_listener){
				m_listener->OnMoveParticle(p, i);
			}
		}
	}	
}