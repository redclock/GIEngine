#include "CellEffect.h"
#include "GameEngine.h"

CParticle3DEffect::CParticle3DEffect()
{
	m_p = NULL;
}

CParticle3DEffect::~CParticle3DEffect()
{
	if (m_p) delete m_p;
}


void CParticle3DEffect::Render()
{
	if (m_p) m_p->Render();
}

void CParticle3DEffect::Update(float elapsedTime)
{
	m_timer += elapsedTime;
	if (m_p) m_p->Update(elapsedTime);
	if (m_p->IsFinished()) m_stopped = true;
}

CFireEffect::CFireEffect(float size, int count): size(size), count(count)
{

	Reset();
}

CFireEffect::~CFireEffect()
{

}

void CFireEffect::Reset()
{
	if (m_p) delete m_p;
	m_p = new CParticle3DSystem(this);
	m_p->Init(count);
	m_p->Looped = false;
	m_p->img =LoadGameImage("flare3.jpg");
	m_p->psize = size;
	m_stopped = false;
	m_timer = 0;
}

void CFireEffect::OnInitParticle(CParticle3D* p, int index)
{
	p->pos = D3DXVECTOR3(RandomRange(-0.1f, 0.1f), RandomRange(-0.2f, 0.1f), RandomRange(-0.1f, 0.1f));
	float a = RandomRange(0, 2 * D3DX_PI);
	float b = RandomRange(0, D3DX_PI / 2);
	p->vel = D3DXVECTOR3(RandomRange(-0.05f, 0.05f), RandomRange(0.5f, 2.0f), RandomRange(-0.05f, 0.05f));
	p->acc = D3DXVECTOR3(0, 0.05f, 0);
	p->color_from = D3DXCOLOR(1.1f, 0.5f, 0.1f, 1);
	p->color_to = D3DXCOLOR(0, 0, 0, 0);
	p->wholelife = RandomRange(0.1f, 1.2f);
	p->curlife = p->wholelife;
	p->decay = 1;
	p->active = true;
}

CSparkEffect::CSparkEffect(CImage2D * img)
{
	Img = img;
	Num = 20;
	Size = 2;
	Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

CSparkEffect::~CSparkEffect()
{

}

void CSparkEffect::Reset()
{
	if (m_p) delete m_p;
	m_p = new CParticle3DSystem(this);
	m_p->Init(Num);
	m_p->Looped = false;
	m_p->img = Img;
	m_p->psize = Size;
	m_stopped = false;
	m_timer = 0;
}

void CSparkEffect::OnInitParticle(CParticle3D* p, int index)
{

	float a = 2 * D3DX_PI * index / m_p->Count;  //RandomRange(0, 2 * D3DX_PI);
	float b = RandomRange(0, D3DX_PI / 2);
	float r = 2;
	p->pos = D3DXVECTOR3(0, 0.2f, 0);
	p->vel = D3DXVECTOR3(cos(a) * r, 0, sin(a) * r);
	p->acc = D3DXVECTOR3(0, -0.0, 0);
	p->color_from = Color;
	p->color_to = D3DXCOLOR(0, 0, 0, 0);
	p->wholelife = 0.5f;
	p->curlife = p->wholelife;
	p->decay = 1;
	p->active = true;
}