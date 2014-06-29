#pragma once
#include "Point3.h"
#include "RenderTarget.h"
#include "DynamicTexture.h"
#include "OcclusionQuery.h"

class CPhotonMap
{
private:
    struct Photon
    {
        /** photon position */
        Point3 pos;
        Point3 normal;
        /** splitting plane for kd-tree */
        short plane;
        /** incoming direction */
        unsigned char theta, phi;
        /** photon power */
        Point3 power;
    };

    struct PhotonVertex
    {
        Point3 pos;
        Point3 center;
        Point3 normal;
        Point3 power;
    };

    struct PhotonVertexTexture
    {
        Point3 pos;
        float texu, texv;
    };

private:
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    LPDIRECT3DINDEXBUFFER9 m_pIB;
    LPDIRECT3DVERTEXDECLARATION9 m_vertexDecl;
    LPDIRECT3DVERTEXDECLARATION9 m_vertexDeclTexture;
    LPD3DXEFFECT m_effect;
    LPD3DXEFFECT m_effectImage;
    CDynamicTexture m_texFilter;

    size_t m_photonMaxCount;
    size_t m_photonCount;
    COcclusionQuery m_query;

    UINT m_width;
    UINT m_height;

    float m_powerScale;

    bool m_useTexture;

    bool m_hasGlossy;

    void PhotonToVertex(PhotonVertex* & vertex, const Photon& p);

    void RenderInterpolation(CRenderTarget* target, CRenderTarget* low, int level);
    void RenderBlur(CRenderTarget* target, CRenderTarget* src);
    bool InitIndexBuffer();
    void CreateFilterTexture();

public:
    CPhotonMap();
    ~CPhotonMap();

    void SetPowerScale(float p);

    bool LoadPhotonList(const char* filename);
    bool InitVertexTexture(size_t maxCount, UINT texW, UINT texH);

    void SetSize(UINT width, UINT height);

    void Render(const Point3& eyePos, 
        int level, float photonSize);

    void RenderOneLevel(CRenderTarget* target, DWORD clear,
        const Point3& eyePos, 
        int level, float photonSize);

    void RenderLevels(
        bool hasGlossy,
        CRenderTarget* targets[2],
        const Point3& eyePos, 
        int maxLevel, float photonSize);

    void SetTransforms(const D3DXMATRIX& mWorld,
        const D3DXMATRIX& mView, const D3DXMATRIX& mProjection);

    void SetGBuffers(LPDIRECT3DTEXTURE9 texPos,
        LPDIRECT3DTEXTURE9 texNorm, LPDIRECT3DTEXTURE9 texDiff);

    void SetBoundary(LPDIRECT3DTEXTURE9 texBoundary, float factor);

    void SetPhotonTextures(LPDIRECT3DTEXTURE9 texPos,
        LPDIRECT3DTEXTURE9 texDir, LPDIRECT3DTEXTURE9 texPower,
        LPDIRECT3DTEXTURE9 texNormal);

    void DrawPoints();

    size_t GetMaxCount() const { return m_photonMaxCount; }
    size_t GetCount() const { return m_photonCount; }
    void SetCount(size_t count) { m_photonCount = min(m_photonMaxCount, count); }

    void MaskMaterial(int x, int y, int w, int h, int minID, int maxID, int stencil);
};