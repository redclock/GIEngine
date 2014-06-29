#pragma once

#include "EnvViewPoint.h"
#include "RenderTarget.h"
#include "OcclusionQuery.h"
#include "BoundBox.h"
#include "DynamicTexture.h"

#include <vector>

class CPhotonTracer;

class CViewpointSelector
{
private:
    CPhotonTracer* m_tracer;
    int m_pointCount;
    int m_usedCount;
    CEnvViewPoint* m_points;
    CEnvViewPoint** m_used;
    int* m_compareIndices;
    CRenderTarget m_target;
    CDynamicTexture m_texForCount;
    CRenderTarget m_targetForCount;
    COcclusionQuery m_query;
    UINT m_size;
    UINT m_snapWidth;
    UINT m_snapHeight;
    int m_levels;
    BoundBox m_box;
    LPD3DXEFFECT m_effect;
    int m_predefinedCount;
    int m_curCount;

    void GenShadow(CEnvViewPoint** lights, 
        int lightCount, const CEnvViewPoint& p);
    std::vector<Point3> JitterSampling(int count, const BoundBox& box);
    void CountTexture(LPDIRECT3DTEXTURE9 tex, int level, int x, int y, int w, int h,
        float minMask, float maxMask);
    void CountTextureMipmap(LPDIRECT3DTEXTURE9 tex, int level, int index);
    int FindMaxIndex(int count, bool largest, float* area);
    int FindMaxUnused(int usedCount, float* area);
    void UpdateEnvMap(int index, CEnvViewPoint* point);

public:

    CViewpointSelector();
    ~CViewpointSelector();
    void Init(UINT size);
    void SetTracer(CPhotonTracer* tracer) { m_tracer = tracer; };
    void SetParam(const std::vector<Point3>& predefines, 
        const BoundBox& box, int count, int used);
    void UpdateCenters();

    int GetCurCount() const { return m_curCount; }

    CEnvViewPoint* GetPoints() const { return m_points; };
    int GetCount() const { return m_pointCount; }
    int GetUsedCount() const { return m_usedCount; }
    COcclusionQuery& GetQuery() { return m_query; }
    void Refresh();

    void InitSelect();

    CRenderTarget* GetTarget() { return & m_target; }

    void RenderDebug(
        const D3DXMATRIX& matWorld,
        const D3DXMATRIX& matView,
        const D3DXMATRIX& matProject,
        const Point3& eyePos,
        const Point3& lookAt);
};