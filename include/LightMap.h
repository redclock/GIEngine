#pragma once
#include "MyDIB.h"
#include "Terrain.h"

KDIB* GenLightmap(float cellLen, const Array2D<Point3> & normals, const Array2D<VERTEX_TERRAIN> & vertices, const Point3 & lightDir, float fAmbient);
