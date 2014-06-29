#pragma once
#include "Point3.h"
#include <string>

inline float random()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline UINT randomInt()
{
    return (UINT) rand() * RAND_MAX + (UINT) rand();
}


inline void coordinateSystemRandom(const Point3& v1, Point3* v2, Point3* v3)
{
    do 
    {
        float theta = random() * D3DX_PI;
        float phi = random() * D3DX_PI * 2;
        v2->x = cos(theta) * sin(phi);
        v2->y = cos(theta) * cos(phi);
        v2->z = sin(theta);
        *v3 = v1 % *v2;
    } while (v3->GetLength() < 1e-3);
    *v3 = v3->Normalize();
    *v2 = *v3 % v1;
}


inline void coordinateSystem(const Point3& v1, Point3* v2, Point3* v3)
{
    float invLen;
    if (fabs(v1.x) > fabs(v1.y))
    {
        invLen = 1.0f / sqrt(v1.x * v1.x + v1.z * v1.z);
        *v2 = Point3(-v1.z * invLen, 0.0, v1.x * invLen);
    }
    else if (v1.y != 0.0f || v1.x != 0.0f || v1.z != 0.0f)
    {
        invLen = 1.0f / sqrt(v1.y * v1.y + v1.z * v1.z);
        *v2 = Point3(0.0f, v1.z * invLen, -v1.y * invLen);
    }
    else
    {
        *v2 = Point3(0.0f, 1.0f, 0.0f);
    }
    *v3 = v1 % (*v2);
}

inline Point3 coordinateToNormal(const Point3& p,
                                 const Point3& orign,
                                 const Point3& vx, 
                                 const Point3& vy, 
                                 const Point3& vz)
{
    return p.x * vx + p.y * vy +p.z * vz + orign;
}

inline Point3 normalToCoordinate(const Point3& p, 
                                 const Point3& orign,
                                 const Point3& vx, 
                                 const Point3& vy, 
                                 const Point3& vz)
{
    Point3 pp = p - orign;
    return Point3(pp * vx, pp * vy, pp * vz);
}

inline const char* getExtName(const char* filename)
{
    size_t len = strlen(filename);
    const char* s = filename + len - 1;
    while (s >= filename)
    {
        if (*s == '.')
            return s + 1;
        s--;
    }
    return filename + len;
}

inline const char* getFileName(const char* filename)
{
    size_t len = strlen(filename);
    const char* s = filename + len - 1;
    while (s >= filename)
    {
        if (*s == '\\' || *s == '/')
            return s + 1;
        s--;
    }
    return filename;
}

inline std::string getFilePath(const char* filename)
{
    size_t len = strlen(filename);
    const char* s = filename + len - 1;
    while (s >= filename)
    {
        if (*s == '\\' || *s == '/')
        {
            return std::string(filename).substr(0, s - filename);
        }
        s--;
    }
    return "";
}

class C3DSModel;
bool ReadMaterialList(const char* filename, C3DSModel* model);

inline bool EqualNoCase(const char * s1, const char * s2)
{
    while (*s1 != 0 || *s2 != 0)
    {
        if (toupper(*s1) != toupper(*s2)) return false;
        s1++; s2++;
    }
    return true;
}

inline Point3 Angle2Dir(float x, float z, const Point3& axisX,
                        const Point3& axisY, const Point3& axisZ)
{
    Point3 p = Point3(
        cos(x) * sin(z), 
        sin(x) * sin(z),
        cos(z));
    Point3 dir = p.x * axisX + p.y * axisY + p.z * axisZ;
    return dir.Normalize();
}

inline D3DXMATRIX Angle2Matrix(float x, float z, const Point3& axisX,
                               const Point3& axisY, const Point3& axisZ)
{
    D3DXMATRIX mat1, mat2;
    D3DXMatrixRotationY(&mat1, z);
    D3DXMatrixRotationZ(&mat2, x);
    return mat1 * mat2;
}

inline Point3 Dir2Angle(const Point3& dir, const Point3& axisX,
                        const Point3& axisY, const Point3& axisZ)
{
    Point3 pview = Point3(dir * axisX, dir * axisY, dir * axisZ).Normalize();
    float x = atan2(pview.y, pview.x);
    float z = acos(pview.z);
    return Point3(x, z, 0);
}

inline Point3 Vec2Point3(const D3DXVECTOR3& vec)
{
    return Point3(vec.x, vec.y, vec.z);
}

inline void concentricSampleDisk(float u1, float u2, float* dx, float* dy)
{
    float r, theta;
    // Map uniform random numbers to $[-1,1]^2$
    float sx = 2 * u1 - 1;
    float sy = 2 * u2 - 1;
    // Map square to $(r,\theta)$
    // Handle degeneracy at the origin
    if (fabs(sx) < 1e-6f && fabs(sy) < 1e-6f)
    {
        *dx = 0.0;
        *dy = 0.0;
    }
    else
    {
        if (sx >= -sy)
        {
            if (sx > sy)
            {
                // Handle first region of disk
                r = sx;
                if (sy > 0.0f)
                    theta = sy / r;
                else
                    theta = 8.0f + sy / r;
            }
            else
            {
                // Handle second region of disk
                r = sy;
                theta = 2.0f - sx / r;
            }
        }
        else
        {
            if (sx <= sy)
            {
                // Handle third region of disk
                r = -sx;
                theta = 4.0f - sy / r;
            }
            else
            {
                // Handle fourth region of disk
                r = -sy;
                theta = 6.0f + sx / r;
            }
        }
        theta *= D3DX_PI / 4.0f;
        *dx = r * cos(theta);
        *dy = r * sin(theta);
    }
}

inline Point3 cosSampleHemisphere(float u1, float u2)
{
    Point3 result;
    float x, y;
    concentricSampleDisk(u1, u2, &x, &y);
    float sint = sqrt(1 - u1 * u1);
    x = sint * cos(u2 * 2 * D3DX_PI);
    y = sint * sin(u2 * 2 * D3DX_PI);
    result.x = x;
    result.y = y;
    result.z = sqrt(max(0.0f, 1.0f - x * x - y * y));

    return result;
}

inline Point3 uniformSampleSphere(float u1, float u2)
{
    Point3 p;
    p.z = 1.0f - 2.0f * u1;
    float a = 1.0f - (p.z) * (p.z);
    float r = sqrt(max(0.0f, a));
    float phi = 2.0f * D3DX_PI * u2;
    p.x = r * cos(phi);
    p.y = r * sin(phi);
    return p;
}