#include "ParallelLight.h"
#include "MyUtils.h"

CParallelLight::CParallelLight() : CGameLight(LIGHT_PARALLEL)
{
    power[0] = 1;
    power[1] = 1;
    power[2] = 1;
}

CParallelLight::~CParallelLight()
{

}

const D3DXMATRIX& CParallelLight::ComputeShadowMatrix(MeshList& meshes, D3DXMATRIX& matWorld)
{
    Point3 vecZ(-dir[0], -dir[1], -dir[2]);//(-dx, -dy, -dz);
    Point3 vecX, vecY;
    coordinateSystem(vecZ, &vecX, &vecY);
    D3DXMATRIX matView
        (
        vecX.x, vecY.x, vecZ.x, 0,
        vecX.y, vecY.y, vecZ.y, 0,
        vecX.z, vecY.z, vecZ.z, 0,
        0, 0, 0, 1
        );
    D3DXMATRIX mWorldView = matWorld * matView; 
    
    for (size_t i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->GetVisible() && meshes[i]->GetName()[0] != '_')
        {
            D3DXMATRIX matA;
            D3DXMatrixMultiply(&matA, meshes[i]->GetWorldMatrix(), &mWorldView);
            meshes[i]->ComputeBoundBox(&matA);
        }
    }
    
    if (meshes.size() == 0)
        return matShadow;

    BoundBox wb = meshes[0]->GetBoundBox();
    bool first = true;
    for (size_t i = 1; i < meshes.size(); i++)
    {
        if (first && meshes[i - 1]->GetName()[0] == '_')
            wb = meshes[i]->GetBoundBox();
        else if (meshes[i]->GetVisible() && meshes[i]->GetName()[0] != '_')
        {
            wb = BoundBox::unionBoxes(meshes[i]->GetBoundBox(), wb);
        }
        first = first && meshes[i]->GetName()[0] == '_';
    }

    for (size_t i = 0; i < meshes.size(); i++)
    {
        if (meshes[i]->GetVisible())
        {
            meshes[i]->ComputeBoundBox();
        }
    }

    float xlen = wb.maxPoint.x - wb.minPoint.x;
    float ylen = wb.maxPoint.y - wb.minPoint.y;
    float zlen = wb.maxPoint.z - wb.minPoint.z;

    wb.expandDelta(xlen * 0.1f, ylen * 0.1f, zlen * 0.1f);

    printf("bound = (%g, %g, %g)-(%g, %g, %g)\n", 
        wb.minPoint.x, wb.minPoint.y, wb.minPoint.z,
        wb.maxPoint.x, wb.maxPoint.y, wb.maxPoint.z); 

    xlen = wb.maxPoint.x - wb.minPoint.x;
    ylen = wb.maxPoint.y - wb.minPoint.y;
    zlen = wb.maxPoint.z - wb.minPoint.z;

    float minx = wb.minPoint.x;
    float miny = wb.minPoint.y;
    float minz = wb.minPoint.z;

    D3DXMATRIX matProj
    (
        2.0f / xlen, 0, 0, 0,
        0, 2.0f / ylen, 0, 0,
        0, 0, 1.0f / zlen, 0,
        (-2 * minx / xlen - 1), (-2 * miny / ylen - 1), -minz / zlen, 1
    );



    matShadow = mWorldView * matProj;

    m_xlen = xlen;
    m_ylen = ylen;
    m_zlen = zlen;
    area = m_xlen * m_ylen;
    angle = 0;
    return matShadow;
}

const float* CParallelLight::GetTotalPower() const
{
    float sum = m_xlen * m_ylen * 1.3f;
    totalPower[0] = sum * power[0];
    totalPower[1] = sum * power[1];
    totalPower[2] = sum * power[2];
    return totalPower;
}

CGameLight* CParallelLight::Clone() const
{
    CParallelLight* light = new CParallelLight();
    *light = *this;
    return light;
}