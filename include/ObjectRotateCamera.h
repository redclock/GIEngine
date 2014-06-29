#pragma once
#include "Camera.h"
#include "Point3.h"

class CObjectRotateCamera : public CCamera
{
private:
    Point3 m_axisX;
    Point3 m_axisY;
    Point3 m_axisZ;
    Point3 m_center;
    float m_radius;
    float m_anglex;
    float m_anglez;

    int m_lastx, m_lasty;
    bool m_lastDown[2];

    Point3 Angle2Pos(float x, float z);
    Point3 Dir2Angle(Point3 dir);

public:
    CObjectRotateCamera();
    ~CObjectRotateCamera();

    void Reset();

    void SetCenter(float x, float y, float z);
    void SetEyePos(float x, float y, float z);
    void SetEyeAngle(float x, float z);
    void SetUp(float x, float y, float z);

    const Point3& GetAxisX() const { return m_axisX; }
    const Point3& GetAxisY() const { return m_axisY; }
    const Point3& GetAxisZ() const { return m_axisZ; }

    float GetAngleX() const { return m_anglex; }
    float GetAngleZ() const { return m_anglez; }

    float GetRadius() const { return m_radius; }
    void SetRadius(float r);

    D3DXMATRIX GetProjectionMatrix(float sceneSize, float fovy) const;

    bool Update();
};