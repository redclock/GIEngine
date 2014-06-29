#include "GameState.h"

CGameState::CGameState(const char *name)
{
    m_prior = NULL;
    m_exited = false;
	m_drawPrior = true;
    strcpy(m_name, name);
}

CGameState::~CGameState(void)
{

}

void CGameState::Start(CGameState * prior)
{
	m_prior = prior;
}

void CGameState::Draw2D(CGame2D * g2d)
{
	if (m_prior && m_drawPrior)
	{
		m_prior->Draw2D(g2d);
	}

}

void CGameState::PreRender()
{
    if (m_prior && m_drawPrior)
    {
        m_prior->PreRender();
    }
}

void CGameState::Draw3D()
{
	if (m_prior && m_drawPrior)
	{
		m_prior->Draw3D();
	}
}

void CGameState::PostRender()
{
    if (m_prior && m_drawPrior)
    {
        m_prior->PostRender();
    }
}

void CGameState::Animate(float elapsedTime)
{
	if (m_prior && m_drawPrior)
	{
		m_prior->Animate(elapsedTime);
	}

}

void CGameState::Update(float elapsedTime)
{

}
