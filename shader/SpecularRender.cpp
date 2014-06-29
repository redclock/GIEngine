#include "SpecularRender.h"
#include "DeferredShade.h"
#include "GameEngine.h"
#include "CubeMap.h"
#include "GameLight.h"

#include <assert.h>

CSpecularRender::CSpecularRender()
{
    m_effect = LoadEffect("shaders/Specular.fx");
}

CSpecularRender::~CSpecularRender()
{

}

bool CSpecularRender::Render(UINT width, UINT height, 
                             const Point3& eyePos, 
                             const Point3& center,
                             CDeferredShade* defer, CCubeMap* cube,
                             CGameLight * light
                             )
{
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effect->SetFloatArray("g_Center", &center.x, 3);

    m_effect->SetFloatArray("g_LightPos", light->GetPos(), 3);
    m_effect->SetFloatArray("g_LightDir", light->GetDir(), 3);
    m_effect->SetFloatArray("g_LightPower", light->GetPower(), 3);
    m_effect->SetInt("g_LightType", light->Type());

    m_effect->SetTexture("g_texPosmap", defer->GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer->GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer->GetTexture(CDeferredShade::MATERIAL));

    m_effect->SetTexture("g_texCube", cube->GetTexture());

    m_effect->SetTechnique("RenderSpecular");

    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return true;

}

void CSpecularRender::SetSpecularProperty(const Point3& reflect, const Point3& refract, 
                                          float index)
{
    m_effect->SetFloatArray("g_specReflect", &reflect.x, 3);
    m_effect->SetFloatArray("g_specRefract", &refract.x, 3);
    m_effect->SetFloat("g_specIndex", index);
}