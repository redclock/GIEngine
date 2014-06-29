#include "BoundaryFinder.h"
#include "DeferredShade.h"
#include "GameEngine.h"

CBoundaryFinder::CBoundaryFinder()
{
    m_effect = LoadEffect("shaders/Boundary.fx");
}

CBoundaryFinder::~CBoundaryFinder()
{

}

void CBoundaryFinder::Init(UINT width, UINT height)
{
    m_width = width;
    m_height = height;
    m_targetSwap[0].CreateTexture(GetDevice(), width, height, D3DFMT_A16B16G16R16F, D3DFMT_UNKNOWN, 0);
    m_targetSwap[1].CreateTexture(GetDevice(), width, height, D3DFMT_A16B16G16R16F, D3DFMT_UNKNOWN, 0);
}

void CBoundaryFinder::OnelevelRenderBoundary(int level, const Point3& eyePos, CDeferredShade* defer)
{
    float size[2] = {(float) defer->GetWidth() / (1 << level), 
        (float) defer->GetHeight() / (1 << level)};
    float offset[2] = {1.0f / size[0], 1.0f / size[1]};

    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetFloatArray("g_Offset", offset, 3);
    m_effect->SetInt("g_Level", level);
    
    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));

    m_effect->SetTechnique("RenderBoundary");
    
    RenderBoundary((UINT) size[0], (UINT) size[1]);
}

void CBoundaryFinder::RenderBoundary(UINT width, UINT height)
{
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, width, height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);
}

void CBoundaryFinder::MultilevelRenderBoundary(int startLevel, const Point3& eyePos, CDeferredShade* defer)
{
    UINT width = m_width / (1 << startLevel);
    UINT height = m_height / (1 << startLevel);
    
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));

    for (int level = startLevel; level >= 0; level--)
    {
        float offset[2] = {1.0f / width, 1.0f / height};
        m_effect->SetFloatArray("g_Offset", offset, 2);
        m_effect->SetTexture("g_texLow", m_targetSwap[0].GetTexture());
        m_effect->SetInt("g_Level", level);
        
        m_targetSwap[1].UpdateSurface(level);
        //m_targetSwap[index].Fill(0, 0, 0, 0);
        m_targetSwap[1].BeginRenderTarget(0);
            GetDevice()->BeginScene();
                GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0, 0);
                if (level < startLevel)
                {
                    m_effect->SetTechnique("RenderInterpolation");
                    RenderBoundary(width, height);
                }
                m_effect->SetTechnique("RenderBoundary");
                RenderBoundary(width, height);
            GetDevice()->EndScene();
        m_targetSwap[1].EndRenderTarget();


        m_effect->SetTexture("g_texLow", m_targetSwap[1].GetTexture());
        m_targetSwap[0].UpdateSurface(level);
        m_targetSwap[0].BeginRenderTarget(0);
        GetDevice()->BeginScene();
        GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0, 0);
        m_effect->SetTechnique("RenderBlur");
        RenderBoundary(width, height);
        GetDevice()->EndScene();
        m_targetSwap[0].EndRenderTarget();

        width *= 2;
        height *= 2;
    }

//     m_effect->SetTechnique("RenderTest");
//     RenderBoundary(m_width, m_height);

}
