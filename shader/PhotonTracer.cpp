#include "PhotonTracer.h"
#include "GameEngine.h"
#include "MyUtils.h"

CPhotonTracer::CPhotonTracer()
{
    m_photonCount = 0;
    m_texWidth = 0;
    m_texHeight = 0;
    m_blockWidth = 0;
    m_blockHeight = 0;
    m_bounceCount = 1;

    m_cubeSize = 256;
    m_cubemapPos = NULL;
    m_cubemapMat = NULL;
    m_cubemapCount = 0;
    m_effect = LoadEffect("shaders/Trace.fx");
    m_baseBounce = 0;

    m_selector.SetTracer(this);

    m_showDebug = false;
    spectObj = NULL;
}

CPhotonTracer::~CPhotonTracer()
{
   DeleteCubeMaps();
}

void CPhotonTracer::Init(CMeshObjects* meshes, UINT photonCount, UINT bounce, const BoundBox& box,
                         UINT lightCount, int candEMC, int useEMC)
{
    if (lightCount == 0)
        lightCount = 1;

    m_blockWidth = (UINT)(sqrt((double)photonCount / bounce / lightCount));
    m_blockHeight = m_blockWidth;
    m_blockWidth *= lightCount;

    m_texWidth = m_blockWidth * 1;
    m_texHeight = m_blockHeight * bounce;

    m_photonCount = m_texWidth * m_texHeight;
    m_bounceCount = bounce;
    m_lightCount = lightCount;
   
    printf("Tracer: photon count = %d * %d * %d * %d = %d\n", 
        m_blockWidth, m_blockHeight, m_bounceCount, m_lightCount, m_photonCount);

    InitBuffers();
    m_centers.clear();
    for (size_t i = 0; i < meshes->Count(); i++)
    {
        CObject3D* obj = meshes->Get(i);
        std::string name = obj->GetName();
        if (name.length() > 8 && name.substr(0, 8) == "__center")
        {
            obj->SetVisible(false);
            m_centers.push_back(obj->GetBoundBox().center());
        }
    }

    //m_centers.push_back(Point3(-30, 0, 0));
    //m_centers.push_back(Point3(0, 0, 0));
    //m_centers.push_back(Point3(-60, 0, 0));
    //// m_centers.push_back(Point3(0, 0, 70));
    //m_centers.push_back(Point3(37, 0, 0));
    //m_centers.push_back(Point3(60, 0, -50));
    //m_centers.push_back(Point3(0, 40, 0));

    //m_centers.push_back(Point3(10, -9, -61));

    //m_centers.push_back(Point3(-30, -39, -20));
    //m_centers.push_back(Point3(67, -50, 20));
    //m_centers.push_back(Point3(67, 0, 20));
    //m_centers.push_back(Point3(-30, 30, -20));
    //m_centers.push_back(Point3(30, -40, 0));
    m_usePredefined = true;
    if (m_centers.size() == 0)
    {
        //BoundBox box = meshes->ComputeBoundBox();
        //for (size_t i = 0; i < 15; i++)
        //{
        //    m_centers.push_back(Point3(
        //        RandomRange(box.minPoint.x, box.maxPoint.x),
        //        RandomRange(box.minPoint.y, box.maxPoint.y),
        //        RandomRange(box.minPoint.z, box.maxPoint.z)
        //        ));
        //}
        m_usePredefined = false;
    }

    m_selector.Init(32);
    m_selector.SetParam(m_centers, box, candEMC, useEMC);
    m_selector.UpdateCenters();

    CreateCubeMaps(m_centers.size());

}

LPDIRECT3DTEXTURE9 CPhotonTracer::GetPosTexture() const
{
    return m_targetFinalPosbuf.GetTexture();
}

LPDIRECT3DTEXTURE9 CPhotonTracer::GetDirTexture() const
{
    return m_targetFinalDirbuf.GetTexture();
}

LPDIRECT3DTEXTURE9 CPhotonTracer::GetPowerTexture() const
{
    return m_targetFinalPowerbuf.GetTexture();
}

LPDIRECT3DTEXTURE9 CPhotonTracer::GetNormalTexture() const
{
    return m_targetFinalNormalbuf.GetTexture();
}


bool CPhotonTracer::LoadPhotonList( const char* filename )
{
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Unable to read file: %s.\n", filename);
        return false;
    }

    fread(&m_photonCount, sizeof(m_photonCount), 1, fp);
    printf("%d photons read.\n", m_photonCount);

    m_photonCount = min(m_photonCount, m_texWidth * m_texHeight);

    D3DLOCKED_RECT lrBufPos, lrBufDir, lrBufPower;
    CDynamicTexture offbufPos, offbufDir, offbufPower;
    offbufPos.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F);
    offbufDir.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F);
    offbufPower.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F);

    if (!offbufPos.LockRect(&lrBufPos, NULL, D3DLOCK_DISCARD))
    {
        MessageBox(NULL, "Lock buffer error", "ERROR", 0);
        return false;
    }
    if (!offbufDir.LockRect(&lrBufDir, NULL, D3DLOCK_DISCARD))
    {
        MessageBox(NULL, "Lock buffer error", "ERROR", 0);
        return false;
    }
    if (!offbufPower.LockRect(&lrBufPower, NULL, D3DLOCK_DISCARD))
    {
        MessageBox(NULL, "Lock buffer error", "ERROR", 0);
        return false;
    }

    float* bufPos = (float*) lrBufPos.pBits;
    float* bufDir = (float*) lrBufDir.pBits;
    float* bufPower = (float*) lrBufPower.pBits;
    
    ZeroMemory(bufPos, lrBufPos.Pitch * m_texHeight);
    ZeroMemory(bufDir, lrBufDir.Pitch * m_texHeight);
    ZeroMemory(bufPower, lrBufPower.Pitch * m_texHeight);

    size_t index = 0;
    for (UINT i = 0; i < m_texHeight; i++)
    {
        float* texelPos = bufPos; 
        float* texelDir = bufDir; 
        float* texelPower = bufPower; 
        if (index >= m_photonCount)
            break;
        for (UINT j = 0; j < m_texWidth; j++)
        {
            if (index >= m_photonCount)
            {
                break;
            }
            Photon p;
            fread(&p, sizeof(Photon), 1, fp);

            texelPos[0] = p.pos.x;
            texelPos[1] = p.pos.y;
            texelPos[2] = p.pos.z;

            float theta = p.theta * D3DX_PI / 256;
            float phi = p.phi * 2 * D3DX_PI / 256;
            Point3 normal; 
            texelDir[0] = -sin(theta) * cos(phi);
            texelDir[1] = -sin(theta) * sin(phi);
            texelDir[2] = -cos(theta);

            texelPower[0] = p.power.x;
            texelPower[1] = p.power.y;
            texelPower[2] = p.power.z;

            texelPos[3] = p.normal.x;
            texelDir[3] = p.normal.y;
            texelPower[3] = p.normal.z;

            texelPos += 4;
            texelDir += 4;
            texelPower += 4;
            index ++;
        }
        bufPos += lrBufPos.Pitch / 4;
        bufDir += lrBufDir.Pitch / 4;
        bufPower += lrBufPower.Pitch / 4;
    }

    offbufPos.UnlockRect();
    offbufDir.UnlockRect();
    offbufPower.UnlockRect();

    fclose(fp);

    GetDevice()->UpdateSurface(offbufPos.GetSurfaceSystem(), NULL, m_targetPosbuf[0].GetSurface(), NULL);
    GetDevice()->UpdateSurface(offbufDir.GetSurfaceSystem(), NULL, m_targetDirbuf[0].GetSurface(), NULL);
    GetDevice()->UpdateSurface(offbufPower.GetSurfaceSystem(), NULL, m_targetPowerbuf[0].GetSurface(), NULL);


    return true;
}

void CPhotonTracer::InitBuffers()
{
    m_targetPosbuf[0].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, 
        D3DFMT_A16B16G16R16F, D3DFMT_D24S8);
    m_targetDirbuf[0].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F);
    m_targetPowerbuf[0].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F);
    //m_targetMaterialbuf[0].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A8R8G8B8);

    m_targetPosbuf[1].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F, D3DFMT_D24S8);
    m_targetDirbuf[1].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F);
    m_targetPowerbuf[1].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F);
    //m_targetMaterialbuf[1].CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A8R8G8B8);

    m_targetEnvmapId.CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F, D3DFMT_D24S8);
    m_targetNormbuf.CreateTexture(GetDevice(), m_blockWidth, m_blockHeight, D3DFMT_A16B16G16R16F, D3DFMT_D24S8);
    
    m_targetFinalPosbuf.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F, D3DFMT_D24S8);
    m_targetFinalDirbuf.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F);
    m_targetFinalPowerbuf.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F);
    m_targetFinalNormalbuf.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A32B32G32R32F);

    m_deferCube.SetSize(m_cubeSize, m_cubeSize);
    //m_deferCube.CreateTarget(CDeferredShade::NORMAL, D3DFMT_A16B16G16R16F, D3DFMT_UNKNOWN, 0);
    //m_deferCube.CreateTarget(CDeferredShade::MATERIAL, D3DFMT_A16B16G16R16F, D3DFMT_UNKNOWN, 0);

    m_cubemapPos = NULL;
    m_cubemapMat = NULL;
    m_cubemapCount = 0;

    CreateRandomTexture();
}

void CPhotonTracer::CreateRandomTexture()
{
    m_texRandom.CreateTexture(GetDevice(), m_texWidth, m_texHeight, D3DFMT_A16B16G16R16);
    D3DLOCKED_RECT lrBuf;

    if (!m_texRandom.LockRect(&lrBuf, NULL, D3DLOCK_DISCARD))
    {
        MessageBox(NULL, "Lock buffer error", "ERROR", 0);
        return;
    }

    WORD* buf = (WORD*) lrBuf.pBits;

    ZeroMemory(buf, lrBuf.Pitch * m_texHeight);

    size_t index = 0;
    for (UINT i = 0; i < m_texHeight; i++)
    {
        WORD* texel = (WORD*) buf; 
        for (UINT j = 0; j < m_texWidth; j++)
        {
            float u = random();
            float v = random();
            Point3 dir = cosSampleHemisphere(u, v);
            texel[0] = WORD((dir.x + 1.0f) / 2 * 65535);
            texel[1] = WORD((dir.y + 1.0f) / 2 * 65535);
            texel[2] = WORD((dir.z + 1.0f) / 2 * 65535);
            texel[3] = 0;
            texel += 4;
        }
        buf += lrBuf.Pitch / 2;
    }

    m_texRandom.UnlockRect();
    m_texRandom.Update(NULL, NULL);
}


bool CPhotonTracer::BeginEmitPhotons(bool caustic, CGameLight* light, UINT lightIndex,
                                     const D3DXMATRIX& matWorld)
{
    m_worldMat = matWorld;
    m_light = light;
    m_effect->SetMatrix("g_mTransform", &matWorld);
    m_effect->SetMatrix("g_mProjectToLight", &light->GetShadowMatrix());
    m_effect->SetFloatArray("g_LightPos", light->GetPos(), 3);
    m_effect->SetFloatArray("g_LightDir", light->GetDir(), 3);
    m_effect->SetFloatArray("g_LightPower", light->GetTotalPower(), 3);
    m_effect->SetInt("g_LightType", light->Type());
    m_effect->SetBool("g_Caustic", caustic);

    int emitCount = (int)m_photonCount / (m_bounceCount);
    m_effect->SetFloat("g_InitAngle", m_light->GetAngle() / sqrt((float)emitCount));
    m_effect->SetFloat("g_InitDia", sqrt(m_light->GetArea() / emitCount));
    //m_photonCount = m_texWidth * m_texHeight;
    m_effect->SetInt("g_EmitPhotonCount", emitCount);
    m_effect->SetTechnique("EmitPhotons2");
    m_effect->SetTexture("g_texRandom", m_texRandom.GetTextureDefault());

    float size[2] = {(float) m_blockWidth / m_lightCount, (float) m_blockHeight};
    m_effect->SetFloatArray("g_ScreenSize", size, 2);
    
    if (lightIndex == 0)
    {
        m_targetPosbuf[0].Fill(0, 0, 0, 0);
        m_targetDirbuf[0].Fill(0, 0, 0, 0);
        m_targetPowerbuf[0].Fill(0, 0, 0, 0);
        //m_targetMaterialbuf[0].Fill(0, 0, 0, 0);
    }

    m_targetPosbuf[0].BeginRenderTarget(0);
    m_targetDirbuf[0].BeginRenderTarget(1);
    m_targetPowerbuf[0].BeginRenderTarget(2);
    //m_targetMaterialbuf[0].BeginRenderTarget(3);
    
    if (lightIndex == 0)
        GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
             0x00000000, 1.0f, 0);

    if (FAILED(GetDevice()->BeginScene()))
        return false;
    UINT cPasses;
    if (FAILED(m_effect->Begin(&cPasses, 0)))
    {
        GetDevice()->EndScene();
        return false;
    }
    m_effect->BeginPass(0);

    D3DVIEWPORT9 oldport;
    GetDevice()->GetViewport(&oldport);
    D3DVIEWPORT9 newport = oldport;
    newport.Width = m_blockWidth / m_lightCount;
    newport.Height = m_blockHeight;
    newport.X = lightIndex * newport.Width;
    GetDevice()->SetViewport(&newport);
    return true;
}

bool CPhotonTracer::EndEmitPhotons()
{
    m_effect->EndPass();
    m_effect->End();
    GetDevice()->EndScene();

    m_targetPosbuf[0].EndRenderTarget();
    m_targetDirbuf[0].EndRenderTarget();
    m_targetPowerbuf[0].EndRenderTarget();
    //m_targetMaterialbuf[0].EndRenderTarget();

    return true;
}

void CPhotonTracer::SetTexture(LPDIRECT3DTEXTURE9 texture)
{
    m_effect->SetTexture("g_Texture", texture);
    m_effect->CommitChanges();
}

void CPhotonTracer::CreateCubeMaps(size_t count)
{
    DeleteCubeMaps();
    m_cubemapPos = new CCubeMap*[count + 1];
    m_cubemapMat = new CCubeMap*[count + 1];
    m_cubemapCount = count;
    for (size_t i = 0; i < count + 1; i++)
    {
        m_cubemapPos[i] = new CCubeMap();
        m_cubemapMat[i] = new CCubeMap();
        m_cubemapPos[i]->CreateTexture(m_cubeSize, D3DFMT_A16B16G16R16F, D3DFMT_D24S8);
        //m_cubemapPos[i].SetCenter(centers[i].x, centers[i].y, centers[i].z);
        m_cubemapMat[i]->CreateTexture(m_cubeSize, D3DFMT_A16B16G16R16F);
        //m_cubemapMat[i].SetCenter(centers[i].x, centers[i].y, centers[i].z);
    }
}

void CPhotonTracer::DeleteCubeMaps()
{
    if (m_cubemapMat == NULL)
        return;
    for (size_t i = 0; i < m_cubemapCount + 1; i++)
    {
        delete m_cubemapPos[i];
        delete m_cubemapMat[i];
    }
    delete[] m_cubemapPos;
    delete[] m_cubemapMat;
    m_cubemapPos = NULL;
    m_cubemapMat = NULL;
    m_cubemapCount = 0;
}

void CPhotonTracer::RenderCubeMaps(CMeshObjects* meshes, BoundBox worldBound,
                                   const D3DXMATRIX& matWorld)
{
    if (m_cubemapCount == 0)
        return;
    m_meshes = meshes;
    m_worldMat = matWorld;

    for (int j = 0; j < m_selector.GetCurCount(); j ++)
    {
        Point3& center = m_centers[j];
        m_cubemapPos[j]->SetCenter(center.x, center.y, center.z);
        m_cubemapMat[j]->SetCenter(center.x, center.y, center.z);
    }

    for (int i = 0; i < m_selector.GetCurCount(); i++)
    {
        RenderCubeMap(m_cubemapPos[i], m_cubemapMat[i]);
    }
}

void CPhotonTracer::RenderCubeMap(CCubeMap* cubemapPos, CCubeMap* cubemapMat)
{
    const Point3& center = cubemapPos->GetCenter();
    m_projectMat = cubemapPos->GetProjMatrix();

    UINT size = cubemapPos->GetSize();

    for (int i = 0; i < 6; i++)
    {
        m_viewMat = cubemapPos->GetViewMatrix(i);


        cubemapPos->UpdateSurface(i, 0);
        cubemapMat->UpdateSurface(i, 0);
        cubemapPos->Fill(0, 0, 0, 0);
        cubemapMat->Fill(0, 0, 0, 0);
        cubemapPos->BeginRenderTarget(0);
        cubemapMat->BeginRenderTarget(1);

       // GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
       //     0x00000000, 1.0f, 0);

        RenderGBuffers(m_deferCube, center, NULL, 0);

        cubemapMat->EndRenderTarget();
        cubemapPos->EndRenderTarget();
        //LPDIRECT3DSURFACE9 suf;
        //char txt[100];
        //wsprintf(txt, "AA%d.jpg", i);
        //cubemapMat->GetTexture()->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &suf);

        //////D3DXSaveTextureToFile(txt, D3DXIFF_JPG, m_targetIndirectSwapCube[0].GetTexture(), NULL);
        //D3DXSaveSurfaceToFile(txt,  D3DXIFF_JPG, suf, NULL, NULL);
    }


}

void CPhotonTracer::RenderGBuffers(CDeferredShade& defer, const Point3& eyePos, 
                                   CObject3D* hideObjs, int hideCount)
{
    LPDIRECT3DTEXTURE9 texWhite = LoadGameImage("white.bmp")->GetTexture();

    //defer.GetRenderTarget(CDeferredShade::NORMAL)->Fill(0, 0, 0, 0);
    //defer.GetRenderTarget(CDeferredShade::MATERIAL)->Fill(0, 0, 0, 0);
    if (defer.BeginRender(false, eyePos, CDeferredShade::FOR_TRACE, 
        m_worldMat, m_viewMat, m_projectMat))
    {
        for (size_t j = 0; j < m_meshes->Count(); j++)
        {
            CObject3D* obj = m_meshes->Get(j);

            if (obj == spectObj || !obj->GetVisible() || std::string(obj->GetName()) == "###wave")
                continue;

            bool hide = false;
            for (int k = 0; k < hideCount; k++)
            {
                if (obj == &hideObjs[k])
                {
                    hide = true;
                    break;
                }
            }
            if (hide) continue;

            if (obj->GetTexture() != NULL)
                defer.SetTexture(obj->GetTexture());
            else
                defer.SetTexture(texWhite);
            obj->UpdateMatrix();
            defer.SetObjectTransform(*obj->GetWorldMatrix());
            //defer.SetTexture(texWhite);

            obj->Render();
        }
        D3DXMATRIX mat;
        D3DXMatrixIdentity(&mat);
        defer.SetObjectTransform(mat);
        //DrawLight();
        //DrawCenterPoints();
        defer.EndRender();
    }
}

void CPhotonTracer::UpdatePhotonBuffers(int itr)
{
    int sourceIndex = itr & 1;
    int targetIndex = 1 - sourceIndex;
    m_effect->SetTexture("g_texPhotonPos", m_targetPosbuf[sourceIndex].GetTexture());
    m_effect->SetTexture("g_texPhotonDir", m_targetDirbuf[sourceIndex].GetTexture());
    m_effect->SetTexture("g_texPhotonPower", m_targetPowerbuf[sourceIndex].GetTexture());
    m_effect->SetTexture("g_texRandom", m_texRandom.GetTextureDefault());
    //m_effect->SetTexture("g_texPhotonMaterial", m_targetMaterialbuf[sourceIndex].GetTexture());
    
    TracePhotonBuffers();

    m_targetPosbuf[targetIndex].Fill(0, 0, 0, 0);
    m_targetDirbuf[targetIndex].Fill(0, 0, 0, 0);
    m_targetPowerbuf[targetIndex].Fill(0, 0, 0, 0);
    m_targetNormbuf.Fill(0, 0, 0, 0);
    //m_targetMaterialbuf[targetIndex].Fill(0, 0, 0, 0);
    m_targetPosbuf[targetIndex].BeginRenderTarget(0);
    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
        0x00000000, 1.0f, 0);
    m_targetPosbuf[targetIndex].EndRenderTarget();

    //GetDevice()->BeginScene();
    //MaskTextureLevel(m_targetMaterialbuf[sourceIndex].GetTexture(), 0, 0, 0, m_texWidth, m_texHeight,
    //    -0.2f / 255, 0.2f / 255, 1);
    //GetDevice()->EndScene();

    m_effect->SetTechnique("TracePhotonsReflect");

    for (int i = 0; i < m_selector.GetCurCount(); i++)
    {
        m_effect->SetTexture("g_texEnvId", m_targetEnvmapId.GetTexture());
        m_effect->SetTexture("g_texCubeNorm", m_cubemapPos[i]->GetTexture());
        m_effect->SetTexture("g_texCubeMat", m_cubemapMat[i]->GetTexture());
        m_effect->SetFloatArray("g_Center", &m_cubemapPos[i]->GetCenter().x, 3);
        m_effect->SetInt("g_EnvMapId", (int)i);
        m_targetPosbuf[targetIndex].BeginRenderTarget(0);
        m_targetDirbuf[targetIndex].BeginRenderTarget(1);
        m_targetPowerbuf[targetIndex].BeginRenderTarget(2);
        m_targetNormbuf.BeginRenderTarget(3);
        //m_targetMaterialbuf[targetIndex].BeginRenderTarget(3);
        if (SUCCEEDED(GetDevice()->BeginScene()))
        {
            GetDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 
                0x00000000, 1.0f, 0);
            MaskTextureLevel(m_targetEnvmapId.GetTexture(), 0, 0, 0, m_blockWidth, m_blockHeight,
                i - 0.1f, i + 0.1f, 1);

            DX_BEGIN_EFFECT(m_effect);

            RECT rt = {0, 0, (int)m_blockWidth, (int)m_blockHeight};
            Get2D()->DrawRect(&rt, 0x00FFFFFF);

            DX_END_EFFECT(m_effect);
            GetDevice()->EndScene();
        }
        m_targetPosbuf[targetIndex].EndRenderTarget();
        m_targetDirbuf[targetIndex].EndRenderTarget();
        m_targetPowerbuf[targetIndex].EndRenderTarget();
        m_targetNormbuf.EndRenderTarget();
        //m_targetMaterialbuf[targetIndex].EndRenderTarget();
    }
    
    // Render to final texture
    m_effect->SetTexture("g_texPhotonPos", m_targetPosbuf[targetIndex].GetTexture());
    m_effect->SetTexture("g_texPhotonDir", m_targetDirbuf[sourceIndex].GetTexture());
    m_effect->SetTexture("g_texPhotonPower", m_targetPowerbuf[sourceIndex].GetTexture());
    m_effect->SetTexture("g_texEnvId", m_targetPowerbuf[targetIndex].GetTexture());
    m_effect->SetTexture("g_texPhotonNormal", m_targetNormbuf.GetTexture());

    m_effect->SetTechnique("FinalResult");

    m_targetFinalPosbuf.BeginRenderTarget(0);
    m_targetFinalDirbuf.BeginRenderTarget(1);
    m_targetFinalPowerbuf.BeginRenderTarget(2);
    m_targetFinalNormalbuf.BeginRenderTarget(3);
    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        //GetDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 
        //    0x00000000, 1.0f, 0);
        DX_BEGIN_EFFECT(m_effect);

        RECT rt = {0, (itr + m_baseBounce) * (int)m_blockHeight, 
            (int)m_blockWidth, (itr + 1 + m_baseBounce) * (int)m_blockHeight};
        Get2D()->DrawRect(&rt, 0x00FFFFFF);

        DX_END_EFFECT(m_effect);
        GetDevice()->EndScene();
    }
    m_targetFinalPosbuf.EndRenderTarget();
    m_targetFinalDirbuf.EndRenderTarget();
    m_targetFinalPowerbuf.EndRenderTarget();
    m_targetFinalNormalbuf.EndRenderTarget();
}


void CPhotonTracer::TracePhotonBuffers()
{
    //int sourceIndex = 1 - targetIndex;

    m_targetEnvmapId.Fill(0, 0, 0, 1000);
    m_targetEnvmapId.BeginRenderTarget(0);
    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
        0x00000000, 1.0f, 0);
    m_targetEnvmapId.EndRenderTarget();

    m_effect->SetTechnique("TracePhotonsDensity");

    for (int i = 0; i < m_selector.GetCurCount(); i++)
    {
        m_effect->SetTexture("g_texCubeNorm", m_cubemapPos[i]->GetTexture());
        m_effect->SetTexture("g_texCubeMat", m_cubemapMat[i]->GetTexture());
        m_effect->SetFloatArray("g_Center", &m_cubemapPos[i]->GetCenter().x, 3);
        m_effect->SetInt("g_EnvMapId", (int)i);
        m_targetEnvmapId.BeginRenderTarget(0);
        //m_targetMaterialbuf[targetIndex].BeginRenderTarget(3);
        if (SUCCEEDED(GetDevice()->BeginScene()))
        {
            DX_BEGIN_EFFECT(m_effect);

            RECT rt = {0, 0, (int)m_blockWidth, (int)m_blockHeight};
            Get2D()->DrawRect(&rt, 0x00FFFFFF);

            DX_END_EFFECT(m_effect);
            GetDevice()->EndScene();
        }
        m_targetEnvmapId.EndRenderTarget();

        //m_targetMaterialbuf[targetIndex].EndRenderTarget();
    }
}

void CPhotonTracer::TracePhotons(bool caustic)
{
    m_caustic = caustic;
    m_effect->SetBool("g_Caustic", caustic);
    m_effect->SetInt("g_MaxBounce", m_bounceCount);
    if (m_baseBounce == 0)
    {
        m_targetFinalPosbuf.Fill(0, 0, 0, 0);
        //m_targetFinalPosbuf.ApplyDepthBuffer();
        m_targetFinalDirbuf.Fill(0, 0, 0, 0);
        m_targetFinalPowerbuf.Fill(0, 0, 0, 0);
        m_targetFinalNormalbuf.Fill(0, 0, 0, 0);
    }
    //m_targetFinalPosbuf.RestoreDepthBuffer();
    UINT b = caustic ? 1 : m_bounceCount;
    for (UINT i = 0; i < b; i++)
    {
        m_effect->SetInt("g_Bounce", i);
        UpdatePhotonBuffers(i);
    }
}

void CPhotonTracer::SetObjectTransform( const D3DXMATRIX& mat )
{
    D3DXMATRIX matT = mat * m_worldMat;
    D3DXMATRIX matL = mat * m_light->GetShadowMatrix();
    m_effect->SetMatrix("g_mTransform", &matT);
    m_effect->SetMatrix("g_mProjectToLight", &matL);

    m_effect->CommitChanges();
}

void CPhotonTracer::DebugInfo()
{
    //LPDIRECT3DSURFACE9 suf;
    //char txt[100];
    //wsprintf(txt, "AA%d.jpg", i);

    //m_cubemapPos->GetTexture()->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Z, 0, suf);

    //////D3DXSaveTextureToFile(txt, D3DXIFF_JPG, m_targetIndirectSwapCube[0].GetTexture(), NULL);
    //D3DXSaveSurfaceToFile(txt,  D3DXIFF_JPG, suf, NULL, NULL);
    //if (GetKeyboard()->IsJustPressed('E'))
    //{
    //    D3DXSaveTextureToFile("p.png", D3DXIFF_PNG, m_targetFinalPosbuf.GetTexture(), NULL);
    //}
    if (GetKeyboard()->IsJustPressed('P'))
    {
        printf("PHOTON COUNT = %d\n", CountValidPhoton());
    }

    if (m_showDebug)
    {
        //VisualizeTextureLevel(m_selector.GetTarget()->GetTexture(), 0, 0, 0,
        //    GetScreenWidth(), GetScreenHeight());

        //VisualizeTextureLevel(m_texRandom.GetTextureDefault(), 0, 0, 0,
        //   GetScreenWidth(), GetScreenHeight());
        VisualizeTextureLevel(m_targetFinalDirbuf.GetTexture(), 0, 0, 0,
            GetScreenWidth(), GetScreenHeight());
    }
}

void CPhotonTracer::Update(float elapsedTime)
{
    //if (GetKeyboard()->IsJustPressed('R'))
    {
        //m_selector.Refresh();
    }
    if (GetKeyboard()->IsJustPressed('D'))
    {
        m_showDebug = ! m_showDebug;
    }
    if (GetKeyboard()->IsJustPressed('I'))
    {
        m_selector.InitSelect();
    }
}

int CPhotonTracer::CountValidPhoton()
{
    m_effect->SetTechnique("CountValid");
    m_effect->SetTexture("g_texPhotonDir", m_targetFinalDirbuf.GetTexture());

    m_targetFinalPosbuf.BeginRenderTarget(0);

    m_selector.GetQuery().Begin();
    DX_BEGIN_EFFECT(m_effect);
    RECT rt = {0, 0, (int)m_texWidth, (int)m_texHeight};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);
    DX_END_EFFECT(m_effect);
    m_selector.GetQuery().End();

    m_targetFinalPosbuf.EndRenderTarget();

    return m_selector.GetQuery().GetResult();
}

void CPhotonTracer::SetReflect(bool r)
{
    m_effect->SetBool("g_Reflect", r);

}

void CPhotonTracer::SetSpecularProperty(const Point3& reflect, const Point3& refract, 
                                        float index)
{
    m_effect->SetFloatArray("g_specReflect", &reflect.x, 3);
    m_effect->SetFloatArray("g_specRefract", &refract.x, 3);
    m_effect->SetFloat("g_specIndex", index);
}