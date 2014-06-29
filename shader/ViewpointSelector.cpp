#include "ViewpointSelector.h"
#include "PhotonTracer.h"
#include "GameEngine.h"
#include "MyUtils.h"

CViewpointSelector::CViewpointSelector()
{
    m_points = NULL;
    m_tracer = NULL;
    m_compareIndices = NULL;
    m_used = NULL;
    m_effect = LoadEffect("shaders/ViewPoint.fx");
    m_query.CreateQuery(GetDevice());
    m_curCount = 0;
}

CViewpointSelector::~CViewpointSelector()
{
    delete [] m_points;
    delete [] m_used;
    delete [] m_compareIndices;
}

void CViewpointSelector::Init(UINT size)
{
    m_size = size;
    m_levels = (int)(log((double)size) / log(2.0));
    UINT d = 1 << m_levels;
    m_snapWidth = size * 3 / d;
    m_snapHeight = size * 2 / d; 
    m_target.CreateTexture(GetDevice(), size * 3, size * 2, D3DFMT_R32F, D3DFMT_UNKNOWN, 
        0, D3DUSAGE_AUTOGENMIPMAP);
    m_target.GetTexture()->SetAutoGenFilterType(D3DTEXF_LINEAR);
}

void CViewpointSelector::SetParam(const std::vector<Point3>& predefines, 
                                  const BoundBox& box, int count, int used)
{
    delete [] m_points;
    delete [] m_used;
    delete [] m_compareIndices;

    if (m_targetForCount.GetTexture())
        m_targetForCount.ReleaseTexture();
    if (m_texForCount.GetSurfaceSystem())
        m_texForCount.ReleaseTextures();
    
    m_predefinedCount = (int) predefines.size();
    m_pointCount = max(m_predefinedCount, count);

    int leftCount = m_pointCount - m_predefinedCount;
    std::vector<Point3> poslist = JitterSampling(leftCount, box);
    leftCount = (int)poslist.size();
    printf("generate %d viewpoints\n", leftCount);
    
    m_pointCount = m_predefinedCount + leftCount;

    m_usedCount = max(m_predefinedCount, used);
    m_box = box;
    m_points = new CEnvViewPoint[m_pointCount];
    m_used = new CEnvViewPoint*[m_usedCount + 1];
    m_compareIndices = new int[m_pointCount];
    
    m_targetForCount.CreateTexture(GetDevice(), m_pointCount, 1, D3DFMT_R32F);
    m_texForCount.CreateTexture(GetDevice(), m_pointCount, 1, D3DFMT_R32F);


    for (int i = 0; i < m_predefinedCount; i++)
    {
        m_points[i].SetPosition(predefines[i]);
        m_used[i] = &m_points[i];
        m_points[i].used = true;
        m_points[i].predefined = true;
    }

    for (int i = m_predefinedCount; i < m_pointCount; i++)
    {
        m_points[i].predefined = false;
        m_points[i].SetPosition(poslist[i - m_predefinedCount]);
        if (i < m_usedCount)
        {
            m_used[i] = &m_points[i];
            m_points[i].used = true;
        }
        else
        {
            m_points[i].used = false;
        }
    }
    m_curCount = m_usedCount;
}

void CViewpointSelector::UpdateCenters()
{
    m_tracer->m_centers.resize(m_usedCount + 1);
    for (int i = 0; i < m_usedCount; i++)
    {
        m_tracer->m_centers[i] = m_used[i]->pos;
    }
}

void CViewpointSelector::GenShadow(CEnvViewPoint** lights, int lightCount, const CEnvViewPoint& p)
{
    m_effect->SetFloat("g_WorldLength", m_box.diagonalLength());
    m_effect->SetFloatArray("g_EyePos", &p.pos.x, 3);
    int index = 0;
    for (int i = 0; i < lightCount; i++)
    {
        if (&p == lights[i]) 
            continue;
        char txt[100];
        wsprintf(txt, "g_Center%d", index);
        m_effect->SetFloatArray(txt, &lights[i]->pos.x, 3);
        wsprintf(txt, "g_texCube%d", index);
        m_effect->SetTexture(txt, m_tracer->m_cubemapPos[i]->GetTexture());
        index++;
    }

    m_effect->SetInt("g_CubeCount", lightCount);
    
    m_effect->SetTechnique("RenderShadow");
    
    D3DVIEWPORT9 oldport;
    GetDevice()->GetViewport(&oldport);
    D3DVIEWPORT9 newport = oldport;
    newport.Width = m_size;
    newport.Height = m_size;
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 2, 1.0f, 2.5f, 500.0f);
    //m_target.Fill(0, 0, 0, 0);
    m_target.BeginRenderTarget(0);

    GetDevice()->Clear(NULL, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
    if (SUCCEEDED(GetDevice()->BeginScene()))
    {
        for (int face = 0; face < 6; face++)
        {
            m_effect->SetFloatArray("g_LookVec", &CUBE_LOOK_VECTOR[face][0], 3);
            int gridx = face % 3;
            int gridy = face / 3;
            newport.X = gridx * m_size;
            newport.Y = gridy * m_size;

            GetDevice()->SetViewport(&newport);

            D3DXMATRIX matWorldViewProj;
            D3DXMATRIX matViewProj = p.matView[face] * matProj;

            DX_BEGIN_EFFECT(m_effect);

            for (size_t j = 0; j < m_tracer->m_meshes->Count(); j++)
            {
                CObject3D* obj = m_tracer->m_meshes->Get(j);

                if (!obj->GetVisible())
                    continue;

                obj->UpdateMatrix();
                matWorldViewProj = *obj->GetWorldMatrix() * matViewProj;
                m_effect->SetMatrix("g_mWorldViewProjection", &matWorldViewProj);
                m_effect->SetMatrix("g_mTransform", obj->GetWorldMatrix());
                m_effect->CommitChanges();
                obj->Render();
            }
            DX_END_EFFECT(m_effect);
        }
        GetDevice()->EndScene();
    }
    else
    {
        m_target.EndRenderTarget();
        return;
    }
    m_target.EndRenderTarget();

    float count = 0;
    //int divide = 1;
    //float d = 1.0f / divide;
    //for (int i = 0; i < divide; i ++)
    //{
    //    GetDevice()->BeginScene();
    //    m_query.Begin();
    //    CountTexture(m_target.GetTexture(), 0, 0, 0, m_target.GetWidth(), m_target.GetHeight(), 
    //       max(0.001f, d * i), d * (i + 1));
    //    m_query.End();
    //    DWORD num = m_query.GetResult();
    //    GetDevice()->EndScene();

    //    count += num * (i + 0.5f) * (i + 0.5f);
    //}
    //printf("(%g)", count);
    
    //count = CountTextureMipmap(m_target.GetTexture(), m_levels);
    GetDevice()->SetViewport(&oldport);
    //return count;
}

void CViewpointSelector::Refresh()
{
    if (m_pointCount <= m_usedCount)
        return;
    // Select unused point with largest new coverage.
    float unusedArea;
    int unusedIndex = FindMaxUnused(m_curCount, &unusedArea);

    m_used[m_curCount] = &m_points[unusedIndex];
    UpdateEnvMap(m_curCount, &m_points[unusedIndex]);
    
    // Select used point with smallest new coverage.
    for (int i = 0; i < m_curCount; i++)
    {
        GenShadow(m_used, m_curCount + 1, *m_used[i]);
        CountTextureMipmap(m_target.GetTexture(), m_levels, i);
    }
    float usedArea;
    int usedIndex = FindMaxIndex(m_curCount, false, &usedArea);
    if (unusedArea > usedArea)
    {
        printf("Swap %d and %d \n", unusedIndex, usedIndex);
        m_tracer->m_centers[usedIndex] =  m_points[unusedIndex].pos;
        CCubeMap* cubePos = m_tracer->m_cubemapPos[usedIndex];
        CCubeMap* cubeMat = m_tracer->m_cubemapMat[usedIndex];
        m_tracer->m_cubemapPos[usedIndex] = m_tracer->m_cubemapPos[m_curCount];
        m_tracer->m_cubemapMat[usedIndex] = m_tracer->m_cubemapMat[m_curCount];
        m_tracer->m_cubemapPos[m_curCount] = cubePos;
        m_tracer->m_cubemapMat[m_curCount] = cubeMat;
        m_used[usedIndex]->used = false;
        m_used[usedIndex] = &m_points[unusedIndex];
        m_used[usedIndex]->used = true;
    }
    //m_points[].predefined = true;
}

void CViewpointSelector::InitSelect()
{

    if (GetKeyboard()->IsPressed(VK_CONTROL))
    {
        m_curCount = 0;
        for (int i = 0; i < m_usedCount; i++)
            m_used[i]->used = false;
    }
    if (m_curCount >= m_usedCount)
    {
        //m_curCount = 0;
        Refresh();
        return;
    }

   DWORD ot = GetTickCount();
    
   int index = FindMaxUnused(m_curCount, NULL);

   printf("curcount = %d, find %d\n", m_curCount, index);
   printf("time = %d\n", GetTickCount() - ot);
    
   m_points[index].used = true;
   m_used[m_curCount] = &m_points[index];
   UpdateEnvMap(m_curCount, &m_points[index]);
   m_curCount ++;

}

void CViewpointSelector::RenderDebug(const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, 
                                     const D3DXMATRIX& matProject, const Point3& eyePos,
                                     const Point3& lookAt)
{
    D3DXMATRIX matWorldViewProj;
    D3DXMATRIX matViewProj = matView * matProject;
    m_effect->SetInt("g_CubeCount", m_curCount);
    m_effect->SetFloatArray("g_EyePos", &eyePos.x, 3);
    Point3 look = (lookAt - eyePos).Normalize();
    m_effect->SetFloatArray("g_LookVec", &look.x, 3);

    int index = 0;
    for (int i = 0; i < m_usedCount; i++)
    {
        char txt[100];
        wsprintf(txt, "g_Center%d", i);
        m_effect->SetFloatArray(txt, &m_used[i]->pos.x, 3);
        wsprintf(txt, "g_texCube%d", i);
        m_effect->SetTexture(txt, m_tracer->m_cubemapPos[i]->GetTexture());
    }

    m_effect->SetTechnique("RenderShadow");

    DX_BEGIN_EFFECT(m_effect);

    for (size_t j = 0; j < m_tracer->m_meshes->Count(); j++)
    {
        CObject3D* obj = m_tracer->m_meshes->Get(j);

        if (!obj->GetVisible())
            continue;

        obj->UpdateMatrix();
        matWorldViewProj = *obj->GetWorldMatrix() * matViewProj;
        m_effect->SetMatrix("g_mWorldViewProjection", &matWorldViewProj);
        m_effect->SetMatrix("g_mTransform", obj->GetWorldMatrix());
        m_effect->CommitChanges();
        obj->Render();
    }
    DX_END_EFFECT(m_effect);
    //VisualizeTextureLevel(m_target.GetTexture(), 0, 100, 0, 200, 200);
    //VisualizeTextureLevel(m_target.GetTexture(), m_levels, 0, 0, 100, 100);

}

std::vector<Point3> CViewpointSelector::JitterSampling(int count, const BoundBox& box)
{
    std::vector<Point3> list;

    while (count > 0)
    {
        float a = pow(count / box.volume(), 1 / 3.0f);
        int countX = max(1, (int) (a * (box.maxPoint.x - box.minPoint.x)));
        int countY = max(1, (int) (a * (box.maxPoint.y - box.minPoint.y)));
        int countZ = max(1, (int) (a * (box.maxPoint.z - box.minPoint.z)));
        int count1 = countX * countY * countZ;
        a = pow(count1 / box.volume(), 1 / 3.0f);
        for (int x = 0; x < countX; x++)
            for (int y = 0; y < countY; y++)
                for (int z = 0; z < countZ; z++)
                {
                    Point3 p((x + random()) / a, 
                        (y + random()) / a, (z + random()) / a);
                    list.push_back(p + box.minPoint);
                }
        count -= count1;
    }
    return list;
}

void CViewpointSelector::CountTexture(LPDIRECT3DTEXTURE9 tex, int level, 
                                      int x, int y, int w, int h, 
                                      float minMask, float maxMask )
{
    CGame2D* g2d = Get2D();
    m_effect->SetTexture("g_texCount", tex);
    m_effect->SetInt("g_Level", level);
    m_effect->SetFloat("g_MinMask", minMask);
    m_effect->SetFloat("g_MaxMask", maxMask);
    m_effect->SetTechnique("CountTexture");
    DX_BEGIN_EFFECT(m_effect);
    g2d->DrawRect(x, y, w, h, 0x00FFFFFF);
    DX_END_EFFECT(m_effect);
}

void CViewpointSelector::CountTextureMipmap(LPDIRECT3DTEXTURE9 tex, int level, int index)
{
    m_effect->SetTexture("g_texCount", tex);
    m_effect->SetInt("g_Level", level);
    m_effect->SetTechnique("CountTextureMipmap");
    m_targetForCount.BeginRenderTarget(0);
    GetDevice()->BeginScene();
        CGame2D* g2d = Get2D();
        DX_BEGIN_EFFECT(m_effect);
            g2d->DrawRect(index, 0, index + 1, 1, 0xFFFFFFFF);
        DX_END_EFFECT(m_effect);
    GetDevice()->EndScene();
    m_targetForCount.EndRenderTarget();
}

int CViewpointSelector::FindMaxIndex(int count, bool largest, float* area)
{
    LPDIRECT3DSURFACE9 surf;

    if (FAILED(m_targetForCount.GetTexture()->GetSurfaceLevel(0, &surf)))
    {
        MessageBox(GetWindow(), "Error getting surface", "Error", 0);
        return 0;
    }

    if (FAILED(GetDevice()->GetRenderTargetData(surf, m_texForCount.GetSurfaceSystem())))
    {
        MessageBox(GetWindow(), "Error getting render target data", "Error", 0);
        surf->Release();
        return 0;
    }
    D3DLOCKED_RECT lrBuf;

    if (!m_texForCount.LockRect(&lrBuf, NULL, D3DLOCK_DISCARD))
    {
        MessageBox(NULL, "Lock buffer error", "ERROR", 0);
        surf->Release();
        return 0;
    }

    float* buf = (float*) lrBuf.pBits;

    int index = 0;
    for (int i = 0; i < count; i++)
    {
        printf("%d:%g ", i, buf[i]);
        if (largest)    // 找最大的
        {
            if (buf[i] > buf[index])
                index = i;
        }
        else            // 找最小的
        {
            if (buf[i] < buf[index])
            index = i;
        }
    }
    printf("%s %d:%g\n", largest ? "max" : "min", index, buf[index]);
    
    if (area) *area = buf[index];

    m_texForCount.UnlockRect();
    surf->Release();
    return index;

}

// Select unused point with largest new coverage.
int CViewpointSelector::FindMaxUnused(int usedCount, float* area)
{
    int comcount = 0;
    for (int i = 0; i < m_pointCount; i++)
    {
        if (m_points[i].used)
            continue;
        GenShadow(m_used, usedCount, m_points[i]);
        CountTextureMipmap(m_target.GetTexture(), m_levels, comcount);
        m_compareIndices[comcount] = i;
        comcount ++;
    }

    int index = FindMaxIndex(comcount, true, area);
    return m_compareIndices[index];
}

void CViewpointSelector::UpdateEnvMap(int index, CEnvViewPoint* point)
{
    m_tracer->m_centers[index] = point->pos;
    m_tracer->m_cubemapPos[index]->SetCenter(point->pos.x, point->pos.y, point->pos.z);
    m_tracer->m_cubemapMat[index]->SetCenter(point->pos.x, point->pos.y, point->pos.z);
    m_tracer->RenderCubeMap(m_tracer->m_cubemapPos[index], m_tracer->m_cubemapMat[index]);
}