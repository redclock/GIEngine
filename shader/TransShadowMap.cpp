#include "TransShadowMap.h"
#include "GameEngine.h"
#include "GameLight.h"

CTransShadowMap::CTransShadowMap()
{
    m_effect = LoadEffect("shaders/Translucent.fx");
}

CTransShadowMap::~CTransShadowMap()
{

}

void CTransShadowMap::Init(UINT width, UINT height, 
                           D3DFORMAT format, D3DFORMAT depthFormat, UINT levels, DWORD usage)
{
    m_width = width;
    m_height = height;
    m_target.CreateTexture(GetDevice(), m_width, 
        m_height, format, depthFormat, levels, usage);

}

bool CTransShadowMap::BeginRender(CGameLight* light)
{
    m_lightMat = light->GetShadowMatrix();

    float fOffsetX = 0.5f + (0.5f / m_width);
    float fOffsetY = 0.5f + (0.5f / m_height);
    D3DXMATRIX texScaleBiasMat(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        fOffsetX, fOffsetY, 0.0f, 1.0f );
    m_textureMat = m_lightMat * texScaleBiasMat;

    m_target.BeginRenderTarget(0);
    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
        0x00000000, 1.0f, 0); 

    m_effect->SetMatrix("g_mProjectToLight", &light->GetShadowMatrix());

    m_effect->SetTechnique("Shadowmap");

    if (FAILED(GetDevice()->BeginScene()))
        return false;
    UINT cPasses;
    if (FAILED(m_effect->Begin(&cPasses, 0)))
    {
        GetDevice()->EndScene();
        return false;
    }
    m_effect->BeginPass(0);
    return true;

}

bool CTransShadowMap::EndRender()
{
    m_effect->EndPass();
    m_effect->End();
    GetDevice()->EndScene();
    m_target.EndRenderTarget();
    return true;
}