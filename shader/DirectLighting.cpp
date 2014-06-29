#include "GameEngine.h"

#include "DirectLighting.h"
#include "GameLight.h"
#include "DeferredShade.h"
#include "ShadowMap.h"
#include "FilterAA.h"

#include <assert.h>

CDirectLighting::CDirectLighting()
{
    m_effect = LoadEffect("shaders/DS_Direct.fx");
}

CDirectLighting::~CDirectLighting()
{
}

bool CDirectLighting::Render(UINT width, UINT height,
                             const Point3& eyePos,
                             CGameLight* light, CDeferredShade* defer, 
                             CShadowMap* shadow, float ambient)
{
    m_effect->SetFloatArray("g_LightPos", light->GetPos(), 3);
    m_effect->SetFloatArray("g_LightDir", light->GetDir(), 3);
    m_effect->SetFloatArray("g_LightPower", light->GetPower(), 3);
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetInt("g_LightType", light->Type());
    m_effect->SetFloat("g_Ambient", ambient);

    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));
    if (shadow)
    {
        m_effect->SetTexture("g_texShadowmap", shadow->GetTexture());
        m_effect->SetMatrix("g_mProjectToLight", &shadow->GetTextureMatrix());
        m_effect->SetTechnique("RenderDirectLightShadow");
    }
    else
    {
        m_effect->SetTechnique("RenderDirectLightNoShadow");
    }

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);
    
    return true;

}

bool CDirectLighting::RenderWithEnvLight(UINT width, UINT height, 
                                         const Point3& eyePos, 
                                         LPDIRECT3DTEXTURE9 front, LPDIRECT3DTEXTURE9 back, 
                                         CDeferredShade* defer)
{
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);

    m_effect->SetTexture("g_texFront", front);
    m_effect->SetTexture("g_texBack", back);
    m_effect->SetTexture("g_texSphere", front);
    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));
    m_effect->SetTechnique("RenderDirectLightEnv");

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return true;
}

bool CDirectLighting::RenderAA(UINT width, UINT height, const Point3& eyePos, 
                               CGameLight* light, CDeferredShade* defer, float ambient)
{
    m_effect->SetFloatArray("g_LightPos", light->GetPos(), 3);
    m_effect->SetFloatArray("g_LightDir", light->GetDir(), 3);
    m_effect->SetFloatArray("g_LightPower", light->GetPower(), 3);
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetInt("g_LightType", light->Type());
    m_effect->SetFloat("g_Ambient", ambient);

    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));

    m_effect->SetTechnique("RenderDirectLightAA");

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return true;
}

bool CDirectLighting::RenderAAFilter(UINT width, UINT height, const Point3& eyePos, 
                                     CGameLight* light, CFilterAA* filter, float ambient)
{
    m_effect->SetTexture("g_texPosmap", filter->GetTexture(0));
    m_effect->SetTexture("g_texNormalmap", filter->GetTexture(1));
    m_effect->SetTexture("g_texDiffusemap", filter->GetTexture(2));
    
    m_effect->SetFloatArray("g_LightPos", light->GetPos(), 3);
    m_effect->SetFloatArray("g_LightDir", light->GetDir(), 3);
    m_effect->SetFloatArray("g_LightPower", light->GetPower(), 3);
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetInt("g_LightType", light->Type());
    m_effect->SetFloat("g_Ambient", ambient);

    m_effect->SetTechnique("RenderDirectLightAAFilter");

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return true;

}

bool CDirectLighting::RenderAATexture(UINT width, UINT height, const Point3& eyePos, 
                                      LPDIRECT3DTEXTURE9 texLight, int lightCount, 
                                      float lightOffset, CDeferredShade* defer, float ambient)
{
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetFloat("g_Ambient", ambient);
    m_effect->SetInt("g_LightCount", lightCount);
    m_effect->SetFloat("g_LightOffset", lightOffset);

    m_effect->SetTexture("g_texLightInfo", texLight);
    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));

    m_effect->SetTechnique("RenderDirectLightAATexture");

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return true;
}

bool CDirectLighting::RenderAAFilterTexture(UINT width, UINT height, const Point3& eyePos, 
                                            LPDIRECT3DTEXTURE9 texLight, int lightCount, 
                                            float lightOffset, CFilterAA* filter, float ambient)
{
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetFloat("g_Ambient", ambient);
    m_effect->SetInt("g_LightCount", lightCount);
    m_effect->SetFloat("g_LightOffset", lightOffset);

    m_effect->SetTexture("g_texLightInfo", texLight);
    m_effect->SetTexture("g_texPosmap", filter->GetTexture(0));
    m_effect->SetTexture("g_texNormalmap", filter->GetTexture(1));
    m_effect->SetTexture("g_texDiffusemap", filter->GetTexture(2));

    m_effect->SetTechnique("RenderDirectLightAAFilterTexture");

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return true;

}