#pragma once
#include "GameLight.h"


class CParallelLight: public CGameLight
{
private:
    mutable float totalPower[3];
    float m_xlen, m_ylen, m_zlen;
public:
    CParallelLight();
    ~CParallelLight();

    const D3DXMATRIX& ComputeShadowMatrix(MeshList& meshes, D3DXMATRIX& matWorld);
    const float* GetTotalPower() const;
    CGameLight* Clone() const;
};