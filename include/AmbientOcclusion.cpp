#include "AmbientOcclusion.h"
#include "GameEngine.h"
#include "sequencegen.h"
#include "MyUtils.h"
#include "DeferredShade.h"

CAmbientOcclusion::CAmbientOcclusion()
{
    m_effect = LoadEffect("shaders/SSAO.fx");
    m_samples = 32;
    m_patternCount = 1024;
    
    CreateRandomTexture(m_samples, m_patternCount);
    float offset[2] = {1.0f / m_samples, 1.0f / m_patternCount};
    m_effect->SetFloatArray("g_RandomOffset", offset, 2);
    m_effect->SetInt("g_Samples", m_samples);
    m_effect->SetTexture("g_texRandom", m_texRandom.GetTextureDefault());
}

CAmbientOcclusion::~CAmbientOcclusion()
{

}

void CAmbientOcclusion::CreateRandomTexture(UINT samples, UINT count)
{
    CSequenceGenerator seqgen;
    UINT total = samples * count;
    seqgen.genSequence(total, HAMMERSLEY);
    Point3* dirs = new Point3[total];
    for (UINT i = 0; i < count; i++)
    {
        for (UINT j = 0; j < samples; j++)
        {
            float u = (j + random()) / samples;
            float v = seqgen.randnum[i * samples + j];
            dirs[i * samples + j] = uniformSampleSphere(random(), random());
        }
    }

    m_texRandom.CreateTexture(GetDevice(), samples, count, D3DFMT_A16B16G16R16F);

    D3DLOCKED_RECT lrRandom;
    m_texRandom.LockRect(&lrRandom, NULL, D3DLOCK_DISCARD);

    D3DXFLOAT16* buf = (D3DXFLOAT16*) lrRandom.pBits;

    for (UINT i = 0; i < count; i++)
    {
        for (UINT j = 0; j < samples; j++)
        {
            UINT index = i * samples + j;
            buf[j * 4 + 0] = dirs[index].x;
            buf[j * 4 + 1] = dirs[index].y;
            buf[j * 4 + 2] = dirs[index].z;
            buf[j * 4 + 3] = random();
        }
        buf += lrRandom.Pitch / 2;
    }

    m_texRandom.UnlockRect();
    m_texRandom.Update(NULL, NULL);


    m_texSSRandom.CreateTexture(GetDevice(), 512, 512, D3DFMT_R16F);

    m_texSSRandom.LockRect(&lrRandom, NULL, D3DLOCK_DISCARD);

    buf = (D3DXFLOAT16*) lrRandom.pBits;

    for (UINT i = 0; i < 512; i++)
    {
        for (UINT j = 0; j < 512; j++)
        {
            buf[j] = RandomRange(0, (float)count - 1);
        }
        buf += lrRandom.Pitch / 2;
    }

    m_texSSRandom.UnlockRect();
    m_texSSRandom.Update(NULL, NULL);


    delete [] dirs;
}

void CAmbientOcclusion::RenderAO(UINT width, UINT height, CDeferredShade& defer, 
                                 const Point3& eyepos, float radius)
{
    float fOffsetX = 0.5f + (0.5f / defer.GetWidth());
    float fOffsetY = 0.5f + (0.5f / defer.GetHeight());
    D3DXMATRIX texScaleBiasMat(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        fOffsetX, fOffsetY, 0.0f, 1.0f );
    D3DXMATRIX textureMat = defer.GetWVPMatrix() * texScaleBiasMat;

    m_effect->SetMatrix("g_matWorldViewProjection", &textureMat);
    m_effect->SetTexture("g_texPosmap", defer.GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer.GetTexture(CDeferredShade::NORMAL));
    m_effect->SetFloatArray("g_EyePos", &eyepos.x, 3);
    m_effect->SetFloat("g_Radius", radius);
    m_effect->SetTechnique("SSAO");
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return;

}


void CAmbientOcclusion::RenderDirect(UINT width, UINT height, CDeferredShade& defer, 
                                     const Point3& eyepos, LPDIRECT3DTEXTURE9 texLight, 
                                     int lightCount, float lightOffsetU, float lightOffsetV)
{
    float fOffsetX = 0.5f + (0.5f / defer.GetWidth());
    float fOffsetY = 0.5f + (0.5f / defer.GetHeight());
    D3DXMATRIX texScaleBiasMat(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        fOffsetX, fOffsetY, 0.0f, 1.0f );
    D3DXMATRIX textureMat = defer.GetWVPMatrix() * texScaleBiasMat;

    m_effect->SetMatrix("g_matWorldViewProjection", &textureMat);
    m_effect->SetTexture("g_texPosmap", defer.GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer.GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer.GetTexture(CDeferredShade::MATERIAL));
    m_effect->SetFloatArray("g_EyePos", &eyepos.x, 3);

    m_effect->SetInt("g_LightCount", lightCount);
    float loff[2] = {lightOffsetU, lightOffsetV};
    m_effect->SetFloatArray("g_LightOffset", loff, 2);

    m_effect->SetTexture("g_texLightInfo", texLight);

    m_effect->SetTechnique("Direct");
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return;

}

void CAmbientOcclusion::SetEnvMap(LPDIRECT3DTEXTURE9 tex)
{
    m_effect->SetTexture("g_texSphere", tex);
}

void CAmbientOcclusion::RenderDirectSplit(UINT width, UINT height, UINT gridw, UINT gridh, 
                                          CDeferredShade& defer, LPDIRECT3DTEXTURE9 texPos,
                                          const Point3& eyepos, float radius,
                                          LPDIRECT3DTEXTURE9 texLight, int lightCount, 
                                          float lightOffsetU, float lightOffsetV)
{
    float fOffsetX = 0.5f + (0.5f / defer.GetWidth());
    float fOffsetY = 0.5f + (0.5f / defer.GetHeight());
    D3DXMATRIX texScaleBiasMat(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        fOffsetX, fOffsetY, 0.0f, 1.0f );
    D3DXMATRIX textureMat = defer.GetWVPMatrix() * texScaleBiasMat;

    m_effect->SetMatrix("g_matWorldViewProjection", &textureMat);
    m_effect->SetTexture("g_texPosmaporg", texPos);
    m_effect->SetTexture("g_texPosmap", defer.GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer.GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", defer.GetTexture(CDeferredShade::MATERIAL));
    m_effect->SetFloatArray("g_EyePos", &eyepos.x, 3);
    m_effect->SetFloat("g_Radius", radius);

    m_effect->SetInt("g_LightCount", lightCount);
    float loff[2] = {lightOffsetU, lightOffsetV};
    int cell[2] = { width / gridw, height / gridh };
    m_effect->SetFloatArray("g_LightOffset", loff, 2);
    m_effect->SetIntArray("g_CellSize", cell, 2);

    m_effect->SetTexture("g_texLightInfo", texLight);

    m_effect->SetTechnique("DirectSplit");
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return;
}

void CAmbientOcclusion::RenderDiffuse(UINT width, UINT height, 
                                      CDeferredShade& defer, LPDIRECT3DTEXTURE9 texDiffuse)
{
    m_effect->SetTexture("g_texPosmap", defer.GetTexture(CDeferredShade::POSITION));
    m_effect->SetTexture("g_texNormalmap", defer.GetTexture(CDeferredShade::NORMAL));
    m_effect->SetTexture("g_texDiffusemap", texDiffuse);

    m_effect->SetTechnique("DirectDiff");
    DX_BEGIN_EFFECT(m_effect);

    RECT rt = {0, 0, (int)width, (int)height};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);

    DX_END_EFFECT(m_effect);

    return;

}

void CAmbientOcclusion::SetLightRotMat(const D3DXMATRIX& mat)
{
    D3DXMATRIX matout;
    D3DXMatrixInverse(&matout, NULL, &mat);
    m_effect->SetMatrix("g_matLightRotInv", &matout);
}