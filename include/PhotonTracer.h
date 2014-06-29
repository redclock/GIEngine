#pragma once
#include "Point3.h"
#include "RenderTarget.h"
#include "DynamicTexture.h"
#include "GameLight.h"
#include "CubeMap.h"
#include "DeferredShade.h"
#include "MeshObjects.h"
#include "ViewpointSelector.h"

class CPhotonTracer
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

    UINT m_texWidth;
    UINT m_texHeight;
    
    UINT m_blockWidth;
    UINT m_blockHeight;

    UINT m_bounceCount;


    CRenderTarget m_targetPosbuf[2];
    CRenderTarget m_targetDirbuf[2];
    CRenderTarget m_targetPowerbuf[2];
    //CRenderTarget m_targetMaterialbuf[2];
    CRenderTarget m_targetEnvmapId;
    CRenderTarget m_targetNormbuf;
    
    CRenderTarget m_targetFinalPosbuf;
    CRenderTarget m_targetFinalDirbuf;
    CRenderTarget m_targetFinalPowerbuf;
    CRenderTarget m_targetFinalNormalbuf;

    CCubeMap** m_cubemapPos;
    CCubeMap** m_cubemapMat;
    CDeferredShade m_deferCube;
    size_t m_cubemapCount;
    UINT m_cubeSize;
    UINT m_lightCount;

    CViewpointSelector m_selector;

    size_t m_photonCount;
    LPD3DXEFFECT m_effect;

    D3DXMATRIX m_worldMat;
    D3DXMATRIX m_viewMat;
    D3DXMATRIX m_projectMat;
    CMeshObjects* m_meshes;

    CDynamicTexture m_texRandom;
    CGameLight* m_light;
    bool m_caustic;

    bool m_usePredefined;

    bool m_showDebug;

    int m_baseBounce;

    void InitBuffers();
    void CreateCubeMaps(size_t count);
    void DeleteCubeMaps();
    void RenderCubeMap(CCubeMap* cubemap, CCubeMap* cubemapMat);
    void RenderGBuffers(CDeferredShade& defer,
        const Point3& eyePos, CObject3D* hideObjs, int hideCount);
    void CreateRandomTexture();
    void UpdatePhotonBuffers(int itr);
    void TracePhotonBuffers();

    void InitViewpoints();
public:
    std::vector<Point3> m_centers;
    CObject3D* spectObj;

    CPhotonTracer();
    ~CPhotonTracer();

    void Init(CMeshObjects* meshes, UINT photonCount, UINT bounce, const BoundBox& box, 
        UINT lightCount, int candEMC, int useEMC);

    bool LoadPhotonList(const char* filename);

    UINT GetWidth() const { return m_texWidth; }
    UINT GetHeight() const { return m_texHeight; }
    size_t GetPhotonCount() const { return m_photonCount; }
    
    CViewpointSelector* GetSelector() { return &m_selector; }

    LPDIRECT3DTEXTURE9 GetPosTexture() const;
    LPDIRECT3DTEXTURE9 GetDirTexture() const;
    LPDIRECT3DTEXTURE9 GetPowerTexture() const;
    LPDIRECT3DTEXTURE9 GetNormalTexture() const;


    bool BeginEmitPhotons(bool caustic, CGameLight* light, UINT lightIndex, const D3DXMATRIX& matWorld); 
    bool EndEmitPhotons(); 
    void SetTexture(LPDIRECT3DTEXTURE9 texture);

    void RenderCubeMaps(CMeshObjects* meshes, BoundBox worldBound,
                        const D3DXMATRIX& matWorld);
    void TracePhotons(bool caustic);

    void SetObjectTransform(const D3DXMATRIX& mat);

    void DebugInfo();
    void Update(float elapsedTime);

    int CountValidPhoton();
    void SetReflect(bool r);
    void SetBaseBounce(int b) { m_baseBounce = b; }
    void SetSpecularProperty(const Point3& reflect, const Point3& refract, float index);
private:
    friend class CViewpointSelector;
};