#include "GameLight.h"

CGameLight::CGameLight(LightType type)
: m_type(type)
{

}

CGameLight::~CGameLight()
{

}

void CGameLight::GetPower(float* r, float* g, float* b) const
{
    *r = power[0];
    *g = power[1];
    *b = power[2];
}

void CGameLight::SetPower(float r, float g, float b)
{
    power[0] = r;
    power[1] = g;
    power[2] = b;
}   

void CGameLight::GetDir(float* x, float* y, float* z) const
{
    *x = dir[0];
    *y = dir[1];
    *z = dir[2];
}

void CGameLight::SetDir(float x, float y, float z)
{
    float l = sqrt(x*x + y * y + z * z);
    dir[0] = x / l;
    dir[1] = y / l;
    dir[2] = z / l;
}

void CGameLight::GetPos(float* x, float* y, float* z) const
{
    *x = pos[0];
    *y = pos[1];
    *z = pos[2];
}

void CGameLight::SetPos(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}
