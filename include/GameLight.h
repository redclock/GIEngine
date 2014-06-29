#pragma once
#include <string>
#include <d3dx9.h>
#include "MeshList.h"

enum LightType
{
    LIGHT_POINT = 0,
    LIGHT_PARALLEL = 1
};

class CGameLight
{
private:
    LightType m_type;

protected:
    float power[3];
    float dir[3];
    float pos[3];
    float area;
    float angle;
    D3DXMATRIX matShadow;

    CGameLight(LightType type);

public:
    virtual ~CGameLight();

    const LightType Type() const { return m_type; }

    void GetPower(float* r, float* g, float* b) const;
    const float* GetPower() const { return power; };
    void SetPower(float r, float g, float b);

    void GetDir(float* x, float* y, float* z) const;
    const float* GetDir() const { return dir; };
    void SetDir(float x, float y, float z);

    void GetPos(float* x, float* y, float* z) const;
    const float* GetPos() const { return pos; };
    void SetPos(float x, float y, float z);
    
    float GetArea() const { return area; }
    float GetAngle() const { return angle; }
    const D3DXMATRIX& GetShadowMatrix() const { return matShadow; }
    virtual const D3DXMATRIX& ComputeShadowMatrix(MeshList& meshes, D3DXMATRIX& matWorld) = 0;
    virtual const float* GetTotalPower() const { return power; };
    virtual void SetPara(float p) { };
    virtual CGameLight* Clone() const = 0;
};