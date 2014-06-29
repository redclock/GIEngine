#pragma once
#include "Point3.h"
#include <d3dx9.h>

class CRotateController
{
private:
    float m_theta, m_phi;
    int m_lastx, m_lasty;
    bool m_lastDown[2];
    Point3 m_axisX;
    Point3 m_axisY;
    Point3 m_axisZ;
    bool m_moved;

public:
    CRotateController();
    ~CRotateController();

    void SetAxis(const Point3& x, const Point3& y, const Point3& z);
    void Reset();
    Point3 GetDir() const;
    void SetDir(const Point3& dir);
    D3DXMATRIX GetRotateMatrix() const;
    void Update(float elapsedTime);
    bool IsMoved() const;
};