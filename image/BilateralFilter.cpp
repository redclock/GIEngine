#include "BilateralFilter.h"
#include "GameEngine.h"

CBilateralFilter::CBilateralFilter()
{
    m_effect = LoadEffect("shaders/Bilateral.fx");

    m_blurRadius     = 2;
    m_sharpness      = 16.0f;
    m_edgeThreshold  = 0.1f;
    m_sceneScale     = 1;

}

CBilateralFilter::~CBilateralFilter()
{

}

bool CBilateralFilter::Init(UINT width, UINT height)
{
    m_width = width;
    m_height = height;
    m_offset[0] = 1.0f / width;
    m_offset[1] = 1.0f / height;

    m_effect->SetFloatArray("g_Offset", m_offset, 2);
    SetSceneScale(m_sceneScale);
    UpdateBlur();

    m_targetTemp.CreateTexture(GetDevice(), width, height, D3DFMT_A16B16G16R16);
    return true;
}

void CBilateralFilter::Filter(LPDIRECT3DTEXTURE9 texSource, 
                              LPDIRECT3DTEXTURE9 texDepth, 
                              LPDIRECT3DTEXTURE9 texNormal,
                              float radius, float sharpness)
{
    m_blurRadius = radius;
    m_sharpness = sharpness;
    SetSceneScale(m_sceneScale);
    UpdateBlur();

    m_effect->SetTexture("g_texSource", texSource);
    m_effect->SetTexture("g_texDepth", texDepth);
    m_effect->SetTexture("g_texNormal", texNormal);
    m_targetTemp.BeginRenderTarget(0);

    m_effect->SetTechnique("BlurX");
    DX_BEGIN_EFFECT(m_effect);
    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);
    DX_END_EFFECT(m_effect);

    m_targetTemp.EndRenderTarget();

    m_effect->SetTexture("g_texSource", m_targetTemp.GetTexture());
    m_effect->SetTechnique("BlurY");
    DX_BEGIN_EFFECT(m_effect);
    RECT rt = {0, 0, (int)m_width, (int)m_height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);
    DX_END_EFFECT(m_effect);
}

void CBilateralFilter::UpdateBlur()
{
    float radius     = m_blurRadius;
    float sigma      = (radius+1) / 2;
    float inv_sigma2 = 1.0f / (2 * sigma * sigma);

    m_effect->SetFloat("g_BlurFalloff", inv_sigma2);
    m_effect->SetFloat("g_BlurRadius", radius);
    m_effect->SetFloat("g_EdgeThreshold", m_edgeThreshold);
}

inline float sqr(float x)
{
    return x * x;
}

void CBilateralFilter::UpdateSharpness()
{
    float sharpness = sqr(m_sharpness / m_sceneScale);
    m_effect->SetFloat("g_Sharpness", sharpness);
}

void CBilateralFilter::SetSceneScale(float scale)
{
    m_sceneScale = scale;
    UpdateSharpness();
}