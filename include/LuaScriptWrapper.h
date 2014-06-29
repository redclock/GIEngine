#pragma once
#include <LuaPlus.h>
#include "Point3.h"
#include "BoundBox.h"
#include <d3dx9.h>

using namespace LuaPlus;

class LuaScriptWrapper
{
private:
    LuaStateOwner m_state;
    
    void RegisterLuaFunctions(LuaObject& owner);

    float LuaPoint3Length(LuaObject obj);
    LuaObject LuaPoint3Norm(LuaObject obj);
    LuaObject LuaPoint3Minus(LuaObject obj);
    LuaObject LuaPoint3Add(LuaObject obj1, LuaObject obj2);
    LuaObject LuaPoint3Sub(LuaObject obj1, LuaObject obj2);
    float LuaPoint3Dot(LuaObject obj1, LuaObject obj2);
    LuaObject LuaPoint3Cross(LuaObject obj1, LuaObject obj2);
    LuaObject LuaPoint3Scale(float k, LuaObject obj);

    LuaObject LuaBoundBoxUnion(LuaObject box1, LuaObject box2);

    LuaObject LuaMatrixTranslate(float x, float y, float z);
    LuaObject LuaMatrixScale(float x, float y, float z);
    LuaObject LuaMatrixRotate(LuaObject flag, float angle);
    LuaObject LuaMatrixMultiply(LuaObject mat1, LuaObject mat2);

public:

    LuaScriptWrapper();
    ~LuaScriptWrapper();

    LuaState* GetState() { return m_state; };
    LuaObject Point3ToLua(const Point3& p);
    LuaObject LuaCreatePoint3(float x, float y, float z);
    LuaObject BoundBoxToLua(const BoundBox& box);
    LuaObject LuaCreateBoundBox(LuaObject minp, LuaObject maxp);
    LuaObject MatrixToLua(const D3DXMATRIX& mat);
    D3DXMATRIX LuaToMatrix(LuaObject mat);

};

Point3 LuaToPoint3(LuaObject& obj);
LuaObject Point3ToLua(LuaObject& owner, const char* name, const Point3& p);
BoundBox LuaToBoundBox(LuaObject& obj);
LuaObject BoundBoxToLua(LuaObject& owner, const char* name, const BoundBox& box);