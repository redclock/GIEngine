#include "ObjectRotateCamera.h"
#include "MyUtils.h"
#include "GameEngine.h"

CObjectRotateCamera::CObjectRotateCamera()
{
    m_axisX = Point3(1, 0, 0);
    m_axisY = Point3(0, 1, 0);
    m_axisZ = Point3(0, 0, 1);
    
    m_anglex = 0;
    m_anglez = 0;
    m_radius = 10;
    SetLookat(0, 0, 0);
    m_center = Point3(0, 0, 0);

    vUpVec = D3DXVECTOR3(0, 0, 1);

    Point3 eye = Angle2Pos(m_anglex, m_anglez);
    SetEye(eye.x, eye.y, eye.z);

    m_lastDown[0] = false;
    m_lastDown[1] = false;

    m_lastx = 0;
    m_lasty = 0;

}

CObjectRotateCamera::~CObjectRotateCamera()
{

}

Point3 CObjectRotateCamera::Angle2Pos(float x, float z)
{
    Point3 p = Point3(
        m_radius * cos(x) * sin(z), 
        m_radius * sin(x) * sin(z),
        -m_radius * cos(z));
    Point3 eyePos = m_center + p.x * m_axisX + p.y * m_axisY + p.z * m_axisZ;
    return eyePos;
}

Point3 CObjectRotateCamera::Dir2Angle(Point3 dir)
{
    Point3 pview = Point3(dir * m_axisX, dir * m_axisY, dir * m_axisZ).Normalize();
    float x = atan2(pview.y, pview.x);
    float z = acos(pview.z);
    return Point3(x, z, 0);
}

void CObjectRotateCamera::SetCenter(float x, float y, float z)
{
    SetLookat(x, y, z);
    m_center = Point3(x, y, z);
    Point3 eye = Point3(vEyePt.x, vEyePt.y, vEyePt.z);
    Point3 dir = eye - m_center;
    m_radius = dir.GetLength();
    Point3 angle = Dir2Angle(eye - m_center);
    m_anglex = angle.x;
    m_anglez = angle.y;
}

void CObjectRotateCamera::SetEyePos(float x, float y, float z)
{
    SetEye(x, y, z);
    Point3 dir = Point3(x, y, z) - m_center;
    m_radius = dir.GetLength();
    Point3 angle = Dir2Angle(dir);
    m_anglex = angle.x;
    m_anglez = angle.y;
}

void CObjectRotateCamera::SetEyeAngle(float x, float z)
{
    Point3 eye = Angle2Pos(x, z);
    m_anglex = x;
    m_anglez = z;
    SetEye(eye.x, eye.y, eye.z);         
}

void CObjectRotateCamera::SetUp(float x, float y, float z)
{
    m_axisZ = Point3(x, y, z).Normalize();
    coordinateSystem(m_axisZ, &m_axisX, &m_axisY);
    Point3 eye = Point3(vEyePt.x, vEyePt.y, vEyePt.z);
    Point3 dir = eye - m_center;
    Point3 angle = Dir2Angle(eye - m_center);
    vUpVec.x = x;
    vUpVec.y = y;
    vUpVec.z = z;
    m_anglex = angle.x;
    m_anglez = angle.y;
}

D3DXMATRIX CObjectRotateCamera::GetProjectionMatrix(float sceneSize, float fovy) const
{
    float fAspectRatio = GetScreenWidth() / (float) GetScreenHeight();
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, fovy, fAspectRatio, 
        max(m_radius - sceneSize, 20.1f), m_radius + sceneSize);
    return matProj;
}

void CObjectRotateCamera::SetRadius(float r)
{
    m_radius = r;
    Point3 eye = Angle2Pos(m_anglex, m_anglez);
    SetEye(eye.x, eye.y, eye.z);
}

bool CObjectRotateCamera::Update()
{
    CMouseManager* mouse = GetMouse();
    bool down[2] = {mouse->IsPressed(0), mouse->IsPressed(1)};
    int x = mouse->GetX();
    int y = mouse->GetY();

    int dx = x - m_lastx;
    int dy = y - m_lasty;
    
    bool needUpdate = false;

    if (!GetKeyboard()->IsPressed(VK_SHIFT) && m_lastDown[0] && down[0] && (abs(dx) + abs(dy) > 0))
    {
        m_anglex += dx * 0.01f; // * elapsedTime;
        m_anglez += dy * 0.01f;// * elapsedTime;

        m_anglex = NormAngle(m_anglex);
        m_anglez = NormAngle(m_anglez);
        Point3 eye = Angle2Pos(m_anglex, m_anglez);
        SetEye(eye.x, eye.y, eye.z);         
        needUpdate = true;
    }

    if (GetMouse()->GetWheel() != 0)
    {
        m_radius *= max(0.01f, 1 - GetMouse()->GetWheel() * 0.0003f);
        Point3 eye = Angle2Pos(m_anglex, m_anglez);
        SetEye(eye.x, eye.y, eye.z);
        needUpdate = true;
    }

    m_lastDown[0] = down[0];
    m_lastDown[1] = down[1];
    m_lastx = x;
    m_lasty = y;
   
    return needUpdate;
}

void CObjectRotateCamera::Reset()
{
    m_lastDown[0] = false;
    m_lastDown[1] = false;
}