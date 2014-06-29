#pragma once
#include "GameLight.h"


class CPointLight: public CGameLight
{

public:
    CPointLight();
    ~CPointLight();

    float fov;

    const D3DXMATRIX& ComputeShadowMatrix(MeshList& meshes, D3DXMATRIX& matWorld);
    void SetPara(float p);
    CGameLight* Clone() const;
//    const float* GetTotalPower() const;
};