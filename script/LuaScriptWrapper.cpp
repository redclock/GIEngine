#include "LuaScriptWrapper.h"

LuaScriptWrapper::LuaScriptWrapper()
{
    m_state->OpenLibs();
    RegisterLuaFunctions(m_state->GetGlobals());
}

LuaScriptWrapper::~LuaScriptWrapper()
{
}

void LuaScriptWrapper::RegisterLuaFunctions(LuaObject& owner)
{
    owner.RegisterDirect("point3", *this, &LuaScriptWrapper::LuaCreatePoint3);
    owner.RegisterDirect("p3len", *this, &LuaScriptWrapper::LuaPoint3Length);
    owner.RegisterDirect("p3norm", *this, &LuaScriptWrapper::LuaPoint3Norm);
    owner.RegisterDirect("p3minus", *this, &LuaScriptWrapper::LuaPoint3Minus);
    owner.RegisterDirect("p3add", *this, &LuaScriptWrapper::LuaPoint3Add);
    owner.RegisterDirect("p3sub", *this, &LuaScriptWrapper::LuaPoint3Sub);
    owner.RegisterDirect("p3dot", *this, &LuaScriptWrapper::LuaPoint3Dot);
    owner.RegisterDirect("p3cross", *this, &LuaScriptWrapper::LuaPoint3Cross);
    owner.RegisterDirect("p3scale", *this, &LuaScriptWrapper::LuaPoint3Scale);
    owner.RegisterDirect("p3scale", *this, &LuaScriptWrapper::LuaPoint3Scale);

    owner.RegisterDirect("boundbox", *this, &LuaScriptWrapper::LuaCreateBoundBox);
    owner.RegisterDirect("unionbox", *this, &LuaScriptWrapper::LuaBoundBoxUnion);

    owner.RegisterDirect("matrixTranslate", *this, &LuaScriptWrapper::LuaMatrixTranslate);
    owner.RegisterDirect("matrixScale", *this, &LuaScriptWrapper::LuaMatrixScale);
    owner.RegisterDirect("matrixRotate", *this, &LuaScriptWrapper::LuaMatrixRotate);
    owner.RegisterDirect("matrixMultiply", *this, &LuaScriptWrapper::LuaMatrixMultiply);

    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);
    owner.SetObject("IdentityMatrix", MatrixToLua(mat));
}

LuaPlus::LuaObject LuaScriptWrapper::Point3ToLua(const Point3& p)
{
    LuaObject tobj;
    tobj.AssignNewTable(m_state);
    tobj.SetNumber("x", p.x);
    tobj.SetNumber("y", p.y);
    tobj.SetNumber("z", p.z);
    return tobj;
}

LuaObject LuaScriptWrapper::LuaCreatePoint3(float x, float y, float z)
{
    LuaObject tobj;
    tobj.AssignNewTable(m_state);
    tobj.SetNumber("x", x);
    tobj.SetNumber("y", y);
    tobj.SetNumber("z", z);
    return tobj;
}

float LuaScriptWrapper::LuaPoint3Length(LuaObject obj)
{
	return LuaToPoint3(obj).GetLength();
}

LuaObject LuaScriptWrapper::LuaPoint3Norm(LuaObject obj)
{
    return Point3ToLua(LuaToPoint3(obj).Normalize());
}

LuaObject LuaScriptWrapper::LuaPoint3Minus(LuaObject obj)
{
    return Point3ToLua(-LuaToPoint3(obj));
}

LuaObject LuaScriptWrapper::LuaPoint3Add(LuaObject obj1, LuaObject obj2)
{
    return Point3ToLua(LuaToPoint3(obj1) + LuaToPoint3(obj2));
}

LuaObject LuaScriptWrapper::LuaPoint3Sub(LuaObject obj1, LuaObject obj2)
{
    return Point3ToLua(LuaToPoint3(obj1) - LuaToPoint3(obj2));
}

LuaObject LuaScriptWrapper::LuaPoint3Cross(LuaObject obj1, LuaObject obj2)
{
    return Point3ToLua(LuaToPoint3(obj1) % LuaToPoint3(obj2));
}

float LuaScriptWrapper::LuaPoint3Dot(LuaObject obj1, LuaObject obj2)
{
    return LuaToPoint3(obj1) * LuaToPoint3(obj2);
}

LuaObject LuaScriptWrapper::LuaPoint3Scale(float k, LuaObject obj)
{
    return Point3ToLua(k * LuaToPoint3(obj));
}

LuaObject LuaScriptWrapper::BoundBoxToLua(const BoundBox& box)
{
    LuaObject tobj;
    tobj.AssignNewTable(m_state);
    tobj.SetObject("min", Point3ToLua(box.minPoint));
    tobj.SetObject("max", Point3ToLua(box.maxPoint));
    return tobj;
}

LuaObject LuaScriptWrapper::LuaCreateBoundBox(LuaObject minp, LuaObject maxp)
{
    LuaObject tobj;
    tobj.AssignNewTable(m_state);
    Point3 p = LuaToPoint3(minp);
    tobj.SetObject("min", Point3ToLua(p));
    p = LuaToPoint3(maxp);
    tobj.SetObject("max", Point3ToLua(p));
    return tobj;
}

LuaObject LuaScriptWrapper::LuaBoundBoxUnion( LuaObject box1, LuaObject box2 )
{
    return BoundBoxToLua(BoundBox::unionBoxes(LuaToBoundBox(box1), LuaToBoundBox(box2)));    
}

LuaObject LuaScriptWrapper::MatrixToLua(const D3DXMATRIX& mat)
{
    LuaObject tobj;
    tobj.AssignNewTable(m_state);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            tobj.SetNumber(i * 4 + j, mat.m[i][j]);
        }
    }
    return tobj;
}

D3DXMATRIX LuaScriptWrapper::LuaToMatrix(LuaObject mat)
{
    D3DXMATRIX m;
    try
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m.m[i][j] = mat[i * 4 + j].GetFloat();
            }
        }
    }
    catch (LuaException*)
    {
    	
    }
    return m;
}

Point3 LuaToPoint3(LuaObject& obj)
{
    Point3 p;
    if (obj.IsTable())
    {
        LuaObject a = obj["x"];
        if (a.IsNumber()) p.x = a.GetFloat();
        a = obj["y"];
        if (a.IsNumber()) p.y = a.GetFloat();
        a = obj["z"];
        if (a.IsNumber()) p.z = a.GetFloat();
    }
    return p;
}

LuaObject Point3ToLua(LuaObject& owner, 
                      const char* name, const Point3& p)
{
    LuaObject tobj = owner.CreateTable(name);
    tobj.SetNumber("x", p.x);
    tobj.SetNumber("y", p.y);
    tobj.SetNumber("z", p.z);
    return tobj;
}

BoundBox LuaToBoundBox(LuaObject& obj)
{
    BoundBox box;
    if (obj.IsTable())
    {
        LuaObject a = obj["min"];
        if (a.IsTable())
            box.minPoint = LuaToPoint3(a);
        a = obj["max"];
        if (a.IsTable())
            box.maxPoint = LuaToPoint3(a);
    }
    return box;
}

LuaObject BoundBoxToLua(LuaObject& owner, 
                        const char* name, const BoundBox& box)
{
    LuaObject tobj = owner.CreateTable(name);
    Point3ToLua(tobj, "min", box.minPoint);
    Point3ToLua(tobj, "max", box.maxPoint);
    return tobj;
}