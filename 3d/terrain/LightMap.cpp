#include "LightMap.h"

float IsInShadow(int i, int j, float cellLen, const Array2D<VERTEX_TERRAIN> & vertices, const Point3 & lightDir)
{
	float d,h,D;
	Point3 v = vertices[i][j].pos, dir = lightDir;
	dir.y = 0;
	dir = dir.Normalize() * cellLen;

	Point3 lightPos = lightDir * 2000.0f + v;

	D = (Point3(vertices[i][j].pos.x, 0, vertices[i][j].pos.z) - Point3(lightPos.x, 0, lightPos.z)).GetLength();
	d = 0;
	int ii = (int)(v.z / cellLen + 0.5f);
	int jj = (int)(v.x / cellLen + 0.5f);
	
	bool bShadow = false;

	while (!(( jj >= vertices.GetCols() ) || ( jj <= 0 ) || ( ii >= vertices.GetRows() ) || ( ii <= 0 )))
	{
		h = v.y + ( d * (lightPos.y - v.y)) / D;             // X(P) point

		if (h > 10) return 0;
		// check if height in point P is bigger than point X's height
		float py = vertices[ii][jj].pos.y;

		if (py > h)
		{
			if (bShadow)
			{
				return py - h;

			}else
				bShadow = true;
		}
		v += dir;   // fetch new working point

		ii = (int)(v.z / cellLen + 0.5f);
		jj = (int)(v.x / cellLen + 0.5f);
		d += cellLen;

	};
	return 0;

}

KDIB* GenLightmap(float cellLen, const Array2D<Point3> & normals, const Array2D<VERTEX_TERRAIN> & vertices, const Point3 & lightDir, float fAmbient)
{
	KDIB * lightmap = new KDIB();

	lightmap->Create(vertices.GetCols(), vertices.GetRows(), 32);
	for (int i = 0; i < vertices.GetRows(); i++)
	{
		BYTE * line = lightmap->GetScanLine(i);
		for (int j = 0; j < vertices.GetCols(); j++)
		{
			float f = fAmbient;
			float s = IsInShadow(i, j, cellLen, vertices, lightDir);
			f += (1 - s) * 0.5f * (1.0f + normals[i][j].Normalize() * lightDir.Normalize());
			if (f > 1) f = 1;
			if (f < 0) f = 0;
			BYTE c = (BYTE) (255 * f);
			line[j * 4] = c;
			line[j * 4 + 1] = c;
			line[j * 4 + 2] = c;
		}
	}

	return lightmap;
}
