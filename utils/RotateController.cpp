#include "RotateController.h"
#include "GameEngine.h"
#include "MyUtils.h"

CRotateController::CRotateController()
: m_theta(0), m_phi(0), m_axisX(1, 0, 0), m_axisY(0, 1, 0), m_axisZ(0, 0, 1)
{
    Reset();
}

CRotateController::~CRotateController()
{

}

void CRotateController::SetAxis(const Point3& x, const Point3& y, const Point3& z)
{
    m_axisX = x;
    m_axisY = y;
    m_axisZ = z;
}

void CRotateController::Reset()
{
    m_lastDown[0] = false;
    m_lastDown[1] = false;
    m_moved = false;
}

Point3 CRotateController::GetDir() const
{
    return Angle2Dir(m_theta, m_phi, m_axisX, m_axisY, m_axisZ);
}

void CRotateController::SetDir(const Point3& dir)
{
    Point3 p = Dir2Angle(dir, m_axisX, m_axisY, m_axisZ);
    m_theta = p.x;
    m_phi = p.y;
}

void CRotateController::Update(float elapsedTime)
{
    CMouseManager* mouse = GetMouse();

    bool down[2] = {mouse->IsPressed(0), mouse->IsPressed(1)};
    int x = mouse->GetX();
    int y = mouse->GetY();

    int dx = x - m_lastx;
    int dy = y - m_lasty;

    m_moved = false;

    if (m_lastDown[1] && down[1] && (abs(dx) + abs(dy) > 0))
    {
        m_theta += dx * 0.01f; // * elapsedTime;
        m_phi += dy * 0.01f;// * elapsedTime;

        m_theta = NormAngle(m_theta);
        m_phi = NormAngle(m_phi);
        m_moved = true;
    }

    m_lastDown[0] = down[0];
    m_lastDown[1] = down[1];
    m_lastx = x;
    m_lasty = y;

}

bool CRotateController::IsMoved() const
{
    return m_moved;
}

D3DXMATRIX CRotateController::GetRotateMatrix() const
{
    return Angle2Matrix(m_theta, m_phi, m_axisX, m_axisY, m_axisZ);
}