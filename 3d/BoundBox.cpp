
#include "boundbox.h"

// C++ headers
#include <cassert>
#include <cstddef>
#include <cmath>
#include <memory>

BoundBox::BoundBox(void)
{
}

BoundBox::BoundBox(const Point3& point): minPoint(point), maxPoint(point)
{
}

BoundBox::BoundBox(const Point3& point1, const Point3& point2)
{
    minPoint.x = point1.x < point2.x ? point1.x : point2.x;
    minPoint.y = point1.y < point2.y ? point1.y : point2.y;
    minPoint.z = point1.z < point2.z ? point1.z : point2.z;
    maxPoint.x = point1.x > point2.x ? point1.x : point2.x;
    maxPoint.y = point1.y > point2.y ? point1.y : point2.y;
    maxPoint.z = point1.z > point2.z ? point1.z : point2.z;
}

BoundBox::BoundBox(const BoundBox& box)
{
    memcpy(this, &box, sizeof(BoundBox));
}

BoundBox::~BoundBox(void)
{
}

bool BoundBox::isInside(const Point3& point) const
{
    if (point.x >= minPoint.x && point.x <= maxPoint.x
            && point.y >= minPoint.y && point.y <= maxPoint.y
            && point.z >= minPoint.z && point.z <= maxPoint.z)
        return true;

    return false;
}

bool BoundBox::isOverlap(const BoundBox& box) const
{
    if (maxPoint.x >= box.minPoint.x && minPoint.x <= box.maxPoint.x
        && maxPoint.y >= box.minPoint.y && minPoint.y <= box.maxPoint.y
        && maxPoint.z >= box.minPoint.z && minPoint.z <= box.maxPoint.z)
        return true;

    return false;
}

BoundBox& BoundBox::unionWithPoint(const Point3& point)
{
    minPoint.x = point.x < minPoint.x ? point.x : minPoint.x;
    minPoint.y = point.y < minPoint.y ? point.y : minPoint.y;
    minPoint.z = point.z < minPoint.z ? point.z : minPoint.z;
    maxPoint.x = point.x > maxPoint.x ? point.x : maxPoint.x;
    maxPoint.y = point.y > maxPoint.y ? point.y : maxPoint.y;
    maxPoint.z = point.z > maxPoint.z ? point.z : maxPoint.z;

    return *this;
}

BoundBox& BoundBox::expandDelta(float delta)
{
    assert(-delta <= (maxPoint.x - minPoint.x) / 2.0
               && -delta <= (maxPoint.y - minPoint.y) / 2.0
               && -delta <= (maxPoint.z - minPoint.z) / 2.0);

    minPoint.x -= delta;
    minPoint.y -= delta;
    minPoint.z -= delta;
    maxPoint.x += delta;
    maxPoint.y += delta;
    maxPoint.z += delta;

    return *this;
}

BoundBox& BoundBox::expandDelta(float deltaX, float deltaY, float deltaZ)
{
    assert(-deltaX <= (maxPoint.x - minPoint.x) / 2.0
        && -deltaY <= (maxPoint.y - minPoint.y) / 2.0
        && -deltaZ <= (maxPoint.z - minPoint.z) / 2.0);

    minPoint.x -= deltaX;
    minPoint.y -= deltaY;
    minPoint.z -= deltaZ;
    maxPoint.x += deltaX;
    maxPoint.y += deltaY;
    maxPoint.z += deltaZ;

    return *this;
}

BoundBox& BoundBox::contractDelta(float delta)
{
    assert(delta <= (maxPoint.x - minPoint.x) / 2.0
               && delta <= (maxPoint.y - minPoint.y) / 2.0
               && delta <= (maxPoint.z - minPoint.z) / 2.0);

    minPoint.x += delta;
    minPoint.y += delta;
    minPoint.z += delta;
    maxPoint.x -= delta;
    maxPoint.y -= delta;
    maxPoint.z -= delta;

    return *this;
}

float BoundBox::volume(void) const
{
    return (maxPoint.x - minPoint.x) *
        (maxPoint.y - minPoint.y) * (maxPoint.z - minPoint.z);
}

Point3 BoundBox::center(void) const
{
    return Point3((maxPoint.x + minPoint.x) * 0.5f,
                  (maxPoint.y + minPoint.y) * 0.5f,
                  (maxPoint.z + minPoint.z) * 0.5f);
}

float BoundBox::diagonalLength(void) const
{
    float d = maxPoint.x - minPoint.x;
    float d2 = d * d;
    d = maxPoint.y - minPoint.y;
    d2 += d * d;
    d = maxPoint.z - minPoint.z;
    d2 += d * d;
    return sqrt(d2);
}

BoundBox& BoundBox::operator=(const BoundBox& box)
{
    memcpy(this, &box, sizeof(BoundBox));

    return *this;
}

BoundBox BoundBox::unionBoxes(const BoundBox& box1, const BoundBox& box2)
{
    BoundBox result;
    result.minPoint.x = box1.minPoint.x < box2.minPoint.x
                            ? box1.minPoint.x : box2.minPoint.x;
    result.minPoint.y = box1.minPoint.y < box2.minPoint.y
                            ? box1.minPoint.y : box2.minPoint.y;
    result.minPoint.z = box1.minPoint.z < box2.minPoint.z
                            ? box1.minPoint.z : box2.minPoint.z;
    result.maxPoint.x = box1.maxPoint.x > box2.maxPoint.x
                            ? box1.maxPoint.x : box2.maxPoint.x;
    result.maxPoint.y = box1.maxPoint.y > box2.maxPoint.y
                            ? box1.maxPoint.y : box2.maxPoint.y;
    result.maxPoint.z = box1.maxPoint.z > box2.maxPoint.z
                            ? box1.maxPoint.z : box2.maxPoint.z;

    return result;
}
