#include "PointLight.h"
#include "MyUtils.h"

CPointLight::CPointLight() : CGameLight(LIGHT_POINT)
{
    fov = D3DX_PI / 2.0f;
    power[0] = 10000;
    power[1] = 10000;
    power[2] = 10000;
}

CPointLight::~CPointLight()
{

}

const D3DXMATRIX& CPointLight::ComputeShadowMatrix(MeshList& meshes, D3DXMATRIX& matWorld)
{
    D3DXVECTOR3 vecPos(pos[0], pos[1], pos[2]);

    D3DXVECTOR3 vecLookat = vecPos + D3DXVECTOR3(-dir[0], -dir[1], -dir[2]) * 10;
    Point3 vecZ(-dir[0], -dir[1], -dir[2]);
    Point3 vecX, vecY;
    coordinateSystem(vecZ, &vecX, &vecY);
    D3DXVECTOR3 vecUp(vecY.x, vecY.y, vecY.z);
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &vecPos, &vecLookat, &vecUp);
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, fov, 1, 5.1f, 500.0f);

    matShadow = matWorld * matView * matProj;
    area = 0;
    angle = D3DX_PI / 2.0f;
    return matShadow;
}

CGameLight* CPointLight::Clone() const
{
    CPointLight* light = new CPointLight();
    *light = *this;
    return light;
}

void CPointLight::SetPara( float p )
{
    fov = p;
}