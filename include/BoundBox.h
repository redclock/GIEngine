#pragma once

#include "Point3.h"

/**
 * A simple cubic bounding box class.
 */
class BoundBox
{
public:
    BoundBox(void);
    BoundBox(const Point3& point);
    BoundBox(const Point3& point1, const Point3& point2);
    BoundBox(const BoundBox& box);
    ~BoundBox(void);

public:
    /**
     * Detects if a point is inside the box.
     */
    bool isInside(const Point3& point) const;
    /**
     * Detects if the box overlaps with another one.
     */
    bool isOverlap(const BoundBox& box) const;
    /**
     * Unions the box with a point, and returns the modified box.
     */
    BoundBox& unionWithPoint(const Point3& point);
    /**
     * Expands the original box by pushing out its six bounding faces
     * in value @c delta, aligned with x, y, z coordinates, and returns
     * the modified box.
     */
    BoundBox& expandDelta(float delta);
    BoundBox& expandDelta(float deltaX, float deltaY, float deltaZ);
    /**
     * Contracts the original box by pushing in its six bounding faces
     * in value @c delta, aligned with x, y, z coordinates, and returns
     * the modified box.
     */
    BoundBox& contractDelta(float delta);
    /**
     * Gets volume of the box.
     */
    float volume(void) const;
    /**
     * Gets the center point.
     */
    Point3 center(void) const;
    /**
     * Gets the diagonal length.
     */
    float diagonalLength(void) const;
    /**
     * Operator=.
     */
    BoundBox& operator=(const BoundBox& box);

// public static member functions
public:
    /**
     * Unions two boxes together and returns a new box containing them.
     */
    static BoundBox unionBoxes(const BoundBox& box1, const BoundBox& box2);

// private data members
public:
    Point3 minPoint;
    Point3 maxPoint;
};
