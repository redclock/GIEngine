#include "GameEngine.h"
#include "ShadowMap.h"
#include "RenderTarget.h"
#include "GameLight.h"

#include <assert.h>

CShadowMap::CShadowMap()
{
    m_target = NULL;

    m_effect = LoadEffect("shaders/ShadowMap.fx");
}

CShadowMap::~CShadowMap()
{
    ReleaseTarget();
}

void CShadowMap::SetSize( UINT width, UINT height )
{
    m_width = width;
    m_height = height;
}

bool CShadowMap::CreateTarget(D3DFORMAT format, D3DFORMAT depthFormat, 
                              UINT levels, DWORD usage)
{
    assert(m_target == NULL);
    m_target = new CRenderTarget();
    return m_target->CreateTexture(GetDevice(), m_width, 
        m_height, format, 
        depthFormat, levels, usage);
}

bool CShadowMap::BeginRender(CGameLight* light)
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

    m_target->BeginRenderTarget(0);
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

bool CShadowMap::EndRender()
{
    m_effect->EndPass();
    m_effect->End();
    GetDevice()->EndScene();
    m_target->EndRenderTarget();
    return true;
}

void CShadowMap::ReleaseTarget()
{
    delete m_target;
    m_target = NULL;
}

void CShadowMap::SetObjectTransform( const D3DXMATRIX& mat )
{
    D3DXMATRIXA16 mTransform = mat * m_lightMat;

    m_effect->SetMatrix("g_mProjectToLight", &mTransform);
    m_effect->CommitChanges();

}