#include "PhotonMap.h"
#include "GameEngine.h"
#include "MyUtils.h"
#include "Material.h"

static const UINT TEX_SIZE = 512;

CPhotonMap::CPhotonMap()
{
    D3DVERTEXELEMENT9 dwDecl[] = 
    {
        {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1},
        {0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0,  36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()
    };

    GetDevice()->CreateVertexDeclaration(dwDecl, &m_vertexDecl);

    D3DVERTEXELEMENT9 dwDecl2[] = 
    {
        {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()
    };
    GetDevice()->CreateVertexDeclaration(dwDecl2, &m_vertexDeclTexture);

    m_pVB = NULL;
    m_pIB = NULL;

    m_effect = LoadEffect("shaders/Photons.fx");
    m_effectImage = LoadEffect("shaders/PhotonImage.fx");
    m_query.CreateQuery(GetDevice());

    m_photonCount = 0;
    m_photonMaxCount = 0;

    m_powerScale = 1;

    m_useTexture = false;

    CreateFilterTexture();

}

CPhotonMap::~CPhotonMap()
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pIB);
    SAFE_RELEASE(m_vertexDecl);
    SAFE_RELEASE(m_vertexDeclTexture);
}

void CPhotonMap::SetSize( UINT width, UINT height )
{
    m_width = width;
    m_height = height;
}

bool CPhotonMap::LoadPhotonList(const char* filename)
{
    m_useTexture = false;
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Unable to read file: %s.\n", filename);
        return false;
    }

    fread(&m_photonMaxCount, sizeof(m_photonMaxCount), 1, fp);
    printf("%d photons read.\n", m_photonMaxCount);

    m_photonCount = m_photonMaxCount;

    DWORD SizeVertices = 4 * (DWORD) m_photonMaxCount * sizeof(PhotonVertex);

    if (m_pVB)
    {
        SAFE_RELEASE(m_pVB);
    }

    if (FAILED(GetDevice()->CreateVertexBuffer(
        SizeVertices, 
        0,
        0,
        D3DPOOL_MANAGED,
        &m_pVB,
        NULL)))
    {
        MessageBox(GetWindow(), "CreateVB Error", "error", 0);
        return false;
    }

    PhotonVertex* vertices;
    if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&vertices, 0 ) ) ) 
    {
        MessageBox(GetWindow(), "LockVB Error", "error", 0);
        return false;
    }

    for (size_t i = 0; i < m_photonMaxCount; i++)
    {
        Photon p;
        fread(&p, sizeof(Photon), 1, fp);
        PhotonToVertex(vertices, p);
    }

    m_pVB->Unlock();

    fclose(fp);
    
    InitIndexBuffer();

    return true;
}

void CPhotonMap::PhotonToVertex(CPhotonMap::PhotonVertex* & vertex, 
                                const CPhotonMap::Photon& p)
{
    Point3 px, py, pz = p.normal;
    coordinateSystemRandom(pz, &py, &px);
    float theta = p.theta * D3DX_PI / 256;
    float phi = p.phi * 2 * D3DX_PI / 256;
    Point3 normal; 
    normal.x = -sin(theta) * cos(phi);
    normal.y = -sin(theta) * sin(phi);
    normal.z = -cos(theta);

    vertex[0].pos = coordinateToNormal(Point3(-1, -1, 0), Point3(0, 0, 0), px, py, pz);
    vertex[1].pos = coordinateToNormal(Point3(1, -1, 0), Point3(0, 0, 0), px, py, pz);
    vertex[2].pos = coordinateToNormal(Point3(-1, 1, 0), Point3(0, 0, 0), px, py, pz);
    vertex[3].pos = coordinateToNormal(Point3(1, 1, 0), Point3(0, 0, 0), px, py, pz);

    for (int i = 0; i < 4; i++)
    {
        vertex[i].center = p.pos;
        vertex[i].normal = normal;
        vertex[i].power = p.power;
    }

    vertex += 4;
}


bool CPhotonMap::InitVertexTexture(size_t maxCount, UINT texW, UINT texH)
{
    m_photonMaxCount = maxCount;
    m_useTexture = true;
    DWORD SizeVertices = 4 * (DWORD) m_photonMaxCount * sizeof(PhotonVertexTexture);

    m_photonCount = m_photonMaxCount;

    if (m_pVB)
    {
        SAFE_RELEASE(m_pVB);
    }

    if (FAILED(GetDevice()->CreateVertexBuffer(
        SizeVertices, 
        0,
        0,
        D3DPOOL_MANAGED,
        &m_pVB,
        NULL)))
    {
        MessageBox(GetWindow(), "CreateVB Error", "error", 0);
        return false;
    }

    PhotonVertexTexture* vertices;
    if ( FAILED(m_pVB->Lock( 0, SizeVertices, (VOID **)&vertices, 0 ) ) ) 
    {
        MessageBox(GetWindow(), "LockVB Error", "error", 0);
        return false;
    }

    std::vector<size_t> randomIndex;
    for (size_t i = 0; i < m_photonMaxCount; i++)
    {
        randomIndex.push_back(i);
    }
    // Make random order
    for (size_t i = 0; i < m_photonMaxCount; i++)
    {
        size_t j = ((size_t) rand() * RAND_MAX + (size_t) rand()) % m_photonMaxCount;
        randomIndex[i] = j;
        randomIndex[j] = i;
    }

    for (size_t i = 0; i < m_photonMaxCount; i++)
    {
        Point3 center = Point3(0, 0, 0) + 0.0f * Point3(
            RandomRange(-0.5f, 0.5f), RandomRange(-0.5f, 0.5f), RandomRange(-0.5f, 0.5f));
        vertices[0].pos = Point3(-1, -1, 0) + center;
        vertices[1].pos = Point3(1, -1, 0) + center;
        vertices[2].pos = Point3(-1, 1, 0) + center;
        vertices[3].pos = Point3(1, 1, 0) + center;
        
        float texu = (randomIndex[i] % texW) / (float) texW;
        float texv = (randomIndex[i] / texW) / (float) texH;
        
        vertices[0].texu = vertices[1].texu 
            = vertices[2].texu = vertices[3].texu 
            = texu;
        vertices[0].texv = vertices[1].texv 
            = vertices[2].texv = vertices[3].texv 
            = texv;

        vertices += 4;
    }

    m_pVB->Unlock();

    InitIndexBuffer();
    return true;
}


bool CPhotonMap::InitIndexBuffer()
{
    if (m_pIB)
    {
        SAFE_RELEASE(m_pIB);
    }

    DWORD SizeIndices = 6 * (DWORD) m_photonMaxCount * sizeof(DWORD);

    if ( FAILED(GetDevice()->CreateIndexBuffer(SizeIndices, 0, D3DFMT_INDEX32, 
        D3DPOOL_MANAGED, &m_pIB, NULL) ) )
    {
        MessageBox(GetWindow(), "CreateIB Error", "error", 0);
        return false;
    }	

    DWORD * indices;
    if ( FAILED(m_pIB->Lock( 0, SizeIndices, (VOID **)&indices, 0 ) ) )
    {
        MessageBox(GetWindow(), "LockIB Error", "error", 0);
        return false;
    }
    DWORD index = 0;
    for (size_t i = 0; i < m_photonMaxCount; i++)
    {
        indices[0] = index + 0; indices[1] = index + 3; indices[2] = index + 1;
        indices[3] = index + 0; indices[4] = index + 2; indices[5] = index + 3;

        indices += 6;
        index += 4;
    }
    m_pIB->Unlock();
    return true;
}

void CPhotonMap::Render(const Point3& eyePos, int level, float photonSize)
{
    if (!m_pVB || !m_pIB)
        return;
    float size[2] = {(float) m_width / (1 << level), 
        (float) m_height / (1 << level)};

    MaskMaterial(0, 0, (int)size[0], (int)size[1], MAT_NULL, MAT_NULL, 2);

    m_effect->SetTexture("g_texFilter", m_texFilter.GetTextureDefault());

    m_effect->SetFloatArray("g_ScreenSize", size, 2);

    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    m_effectImage->SetFloatArray("g_EyePos", &eyePos.x, 3);

    m_effect->SetFloat("g_PhotonSize", photonSize);
    m_effect->SetInt("g_Level", level);

    if (m_photonCount != 0)
        m_effect->SetFloat("g_PhotonPower", 
        m_photonMaxCount / m_photonCount * m_powerScale);

    if (m_useTexture)
    {
        m_effect->SetTechnique("PhotonDiffuseTexture");
        GetDevice()->SetVertexDeclaration(m_vertexDeclTexture);
        GetDevice()->SetStreamSource(0, m_pVB, 0, sizeof(PhotonVertexTexture));
    }
    else
    {
        m_effect->SetTechnique("PhotonDiffuse");
        GetDevice()->SetVertexDeclaration(m_vertexDecl);
        GetDevice()->SetStreamSource(0, m_pVB, 0, sizeof(PhotonVertex));
    }

    GetDevice()->SetIndices(m_pIB);
    DX_BEGIN_EFFECT(m_effect);
    GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
        4 * (UINT)m_photonCount, 
        0, 
        2 * (UINT)m_photonCount);
    DX_END_EFFECT(m_effect);
}

void CPhotonMap::SetTransforms(const D3DXMATRIX& mWorld, const D3DXMATRIX& mView, const D3DXMATRIX& mProjection)
{
    D3DXMATRIXA16 mWVP = mWorld * mView * mProjection;

    m_effect->SetMatrix("g_mWorldViewProjection", &mWVP);
    m_effect->SetMatrix("g_mTransform", &mWorld);
}

void CPhotonMap::SetGBuffers(LPDIRECT3DTEXTURE9 texPos, LPDIRECT3DTEXTURE9 texNorm, LPDIRECT3DTEXTURE9 texDiff)
{
    m_effect->SetTexture("g_texPosmap", texPos);
    m_effect->SetTexture("g_texNormalmap", texNorm);
    m_effect->SetTexture("g_texDiffusemap", texDiff);
    m_effectImage->SetTexture("g_texPosmap", texPos);
    m_effectImage->SetTexture("g_texNormalmap", texNorm);
    m_effectImage->SetTexture("g_texDiffusemap", texDiff);
}

void CPhotonMap::SetPhotonTextures(LPDIRECT3DTEXTURE9 texPos, 
                                   LPDIRECT3DTEXTURE9 texDir, 
                                   LPDIRECT3DTEXTURE9 texPower,
                                   LPDIRECT3DTEXTURE9 texNormal)
{
    m_effect->SetTexture("g_texPhotonPos", texPos);
    m_effect->SetTexture("g_texPhotonDir", texDir);
    m_effect->SetTexture("g_texPhotonPower", texPower);
    m_effect->SetTexture("g_texPhotonNormal", texNormal);
}


void CPhotonMap::SetBoundary(LPDIRECT3DTEXTURE9 texBoundary, float factor)
{
    m_effectImage->SetTexture("g_texBoundary", texBoundary);
    m_effectImage->SetFloat("g_Boundary", factor);
}

void CPhotonMap::RenderLevels(bool hasGlossy, CRenderTarget* targets[2], 
                              const Point3& eyePos, int maxLevel, float photonSize)
{
    if (!m_pVB || !m_pIB)
        return;
   
    int curLevel = maxLevel;
    int index = curLevel % 2;
    targets[index]->UpdateSurface(curLevel);
    GetDevice()->ColorFill(targets[index]->GetSurface(), NULL, 0xFF000000);
    
    m_effect->SetBool("g_Glossy", false);
    RenderOneLevel(targets[index], D3DCLEAR_TARGET | D3DCLEAR_STENCIL, 
        eyePos, curLevel, photonSize); 

    while (curLevel > 0) 
    {
        curLevel--;
        index = 1 - index;
        targets[index]->UpdateSurface(curLevel);
        GetDevice()->ColorFill(targets[index]->GetSurface(), NULL, 0xFF000000);
        RenderInterpolation(targets[index], targets[1 - index], curLevel);
        if (curLevel > 0)
        {
            //MaskMaterial(0, 0, m_width / (1 << curLevel), 
            //    m_height / (1 << curLevel), MAT_MIN_GLOSSY, MAT_MAX_GLOSSY, 1);
            m_effect->SetBool("g_Glossy", false);
            RenderOneLevel(targets[index], 0, eyePos, curLevel, photonSize);
        }
        //if (curLevel == 0)
        //{
        //    MaskMaterial(0, 0, m_width / (1 << curLevel), 
        //        m_height / (1 << curLevel), MAT_MIN_GLOSSY, MAT_MAX_GLOSSY, 0);
        //    m_effect->SetBool("g_Glossy", true);
        //    RenderOneLevel(targets[index], 0, eyePos, curLevel, photonSize);
        //}
    }

    targets[0]->UpdateSurface(0);
    targets[1]->UpdateSurface(0);
    RenderBlur(targets[1], targets[0]);

    if (hasGlossy)
    {
        GetDevice()->BeginScene();
        MaskMaterial(0, 0, m_width, m_height, 0, 255, 1);
        // m_query.Begin();
        MaskMaterial(0, 0, m_width, m_height, MAT_MIN_GLOSSY, MAT_MAX_GLOSSY, 0);
        // m_query.End();
        GetDevice()->EndScene();
        //// if (m_query.GetResult() > 0)
         {
             m_effect->SetBool("g_Glossy", true);
             RenderOneLevel(targets[1], 0, eyePos, 0, photonSize / 2);
         }
    }

     //RenderPhotonsGlossy(m_targetIndirectSwap[1], m_psize);

}

void CPhotonMap::RenderOneLevel(CRenderTarget* target, DWORD clear, 
                                const Point3& eyePos, int level, float photonSize)
{
    target->BeginRenderTarget(0);

    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        if (clear)
            GetDevice()->Clear(0, NULL, clear, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0); 
        Render(eyePos, level, photonSize);
        GetDevice()->EndScene();
    }
    target->EndRenderTarget();
}

void CPhotonMap::RenderInterpolation(CRenderTarget* target, CRenderTarget* low, int level)
{
    //    GetDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 1), 1.0f, 0); 
    target->BeginRenderTarget(0);
    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
            D3DCOLOR_RGBA(0, 0, 0, 1), 1.0f, 0); 
        
       
        CGame2D* g2d = Get2D();
        float size[2] = {(float) m_width / (1 << level), 
            (float) m_height / (1 << level)};

        //MaskMaterial(MAT_NULL, MAT_NULL, (int)size[0], (int)size[1], 0, 0, 2);

        m_effectImage->SetFloatArray("g_ScreenSize", size, 2);
        m_effectImage->SetTexture("g_texLow", low->GetTexture());
        m_effectImage->SetInt("g_Level", level);
        m_effectImage->SetTechnique("Interpolate");
        

        DX_BEGIN_EFFECT(m_effectImage);
        RECT rt = {0, 0, (int)size[0], (int)size[1]};
        g2d->DrawRect(&rt, 0x00FFFFFF);
        DX_END_EFFECT(m_effectImage);
        GetDevice()->EndScene();
    }
    target->EndRenderTarget();
}

void CPhotonMap::RenderBlur(CRenderTarget* target, CRenderTarget* src)
{
    target->BeginRenderTarget(0);
    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
            D3DCOLOR_RGBA(0, 0, 0, 1), 1.0f, 0); 

        CGame2D* g2d = Get2D();
        float size[2] = {(float) m_width, (float) m_height};
        
        
        m_effectImage->SetFloatArray("g_ScreenSize", size, 2);
        m_effectImage->SetTexture("g_Image", src->GetTexture());
        

        m_effectImage->SetTechnique("Blur");
        DX_BEGIN_EFFECT(m_effectImage);
        RECT rt = {0, 0, (int) m_width, (int) m_height};
        g2d->DrawRect(&rt, 0x00FFFFFF);
        DX_END_EFFECT(m_effectImage);
        GetDevice()->EndScene();
    }
    target->EndRenderTarget();

}

void CPhotonMap::MaskMaterial(int x, int y, int w, int h, int minID, int maxID, int stencil)
{
    m_effectImage->SetInt("g_MinMaterialID", minID);
    m_effectImage->SetInt("g_MaxMaterialID", maxID);
    m_effectImage->SetInt("g_Mask", stencil);

    m_effectImage->SetTechnique("MaskMaterial");

    DX_BEGIN_EFFECT(m_effectImage);
    RECT rt = {x, y, w, h};
    Get2D()->DrawRect(&rt, 0x00FFFFFF);
    DX_END_EFFECT(m_effectImage);
}

void CPhotonMap::SetPowerScale( float p )
{
    m_powerScale = p;
}

void CPhotonMap::DrawPoints()
{
    GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(1, 1, 1, 1), 1.0f, 0); 
    if (m_useTexture)
        m_effect->SetTechnique("Photon_PointTexture");
    else
        m_effect->SetTechnique("Photon_Point");
    m_effect->SetFloat("g_PhotonSize", 0);
    GetDevice()->SetRenderState(D3DRS_POINTSIZE, FtoDW(1));
    GetDevice()->SetVertexDeclaration(m_vertexDeclTexture);
    if (m_useTexture)
        GetDevice()->SetStreamSource(0, m_pVB, 0, sizeof(PhotonVertexTexture));
    else
        GetDevice()->SetStreamSource(0, m_pVB, 0, sizeof(PhotonVertex));
    GetDevice()->SetIndices(m_pIB);
    DX_BEGIN_EFFECT(m_effect);
    GetDevice()->DrawPrimitive(D3DPT_POINTLIST, 0, 4 * (UINT)m_photonCount);
    DX_END_EFFECT(m_effect);

}

void CPhotonMap::CreateFilterTexture()
{
    UINT texWidth = 256;
    m_texFilter.CreateTexture(GetDevice(), texWidth, 1, D3DFMT_R32F);
    D3DLOCKED_RECT lrBuf;

    if (!m_texFilter.LockRect(&lrBuf, NULL, D3DLOCK_DISCARD))
    {
        MessageBox(NULL, "Lock buffer error", "ERROR", 0);
        return;
    }

    float* buf = (float*) lrBuf.pBits;

    ZeroMemory(buf, lrBuf.Pitch);

    static const float alpha = 0.918f;
    static const float beta = 1.935f;
    static float eb = exp(-beta);

    float* texel = buf; 
    for (UINT j = 0; j < texWidth; j++)
    {
        float u = (float) j / (float) texWidth;
        float x = exp(-beta / 2 * u * u); // Gaussian filter.
        *texel = alpha * (1 - (1 - x) / (1 - eb));
        texel += 1;
    }

    m_texFilter.UnlockRect();
    m_texFilter.Update(NULL, NULL);
}
